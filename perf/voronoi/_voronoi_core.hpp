// SPDX-FileCopyrightText: 2009-2010 Sony Pictures Imageworks Inc., et al.
// SPDX-FileCopyrightText: 2011 Blender Authors
// SPDX-License-Identifier: GPL-2.0-or-later AND BSD-3-Clause
// Hashing and Voronoi arithmetic adapted from Blender v4.2.0 noise.cc.
// Per-call cell caches and guarded plane tests added for mushroom masks.
#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <unordered_map>
namespace mushroom_voronoi
{
inline uint32_t rot(uint32_t x, int n)
{
    return (x << n) | (x >> (32 - n));
}
inline void mix(uint32_t &a, uint32_t &b, uint32_t &c)
{
    a -= c;
    a ^= rot(c, 4);
    c += b;
    b -= a;
    b ^= rot(a, 6);
    a += c;
    c -= b;
    c ^= rot(b, 8);
    b += a;
    a -= c;
    a ^= rot(c, 16);
    c += b;
    b -= a;
    b ^= rot(a, 19);
    a += c;
    c -= b;
    c ^= rot(b, 4);
    b += a;
}
inline void final(uint32_t &a, uint32_t &b, uint32_t &c)
{
    c ^= b;
    c -= rot(b, 14);
    a ^= c;
    a -= rot(c, 11);
    b ^= a;
    b -= rot(a, 25);
    c ^= b;
    c -= rot(b, 16);
    a ^= c;
    a -= rot(c, 4);
    b ^= a;
    b -= rot(a, 14);
    c ^= b;
    c -= rot(b, 24);
}
inline uint32_t bits(float x)
{
    uint32_t b;
    std::memcpy(&b, &x, 4);
    return b;
}
inline float hash3(float x, float y, float z)
{
    uint32_t a = 0xdeadbeef + 12 + 13, b = a, c = a;
    a += bits(x);
    b += bits(y);
    c += bits(z);
    final(a, b, c);
    return float(c) / float(0xffffffffu);
}
inline float hash4(float x, float y, float z, float w)
{
    uint32_t a = 0xdeadbeef + 16 + 13, b = a, c = a;
    a += bits(x);
    b += bits(y);
    c += bits(z);
    mix(a, b, c);
    a += bits(w);
    final(a, b, c);
    return float(c) / float(0xffffffffu);
}
struct V
{
    float x, y, z;
};
inline V add(V a, V b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
inline V sub(V a, V b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
inline V mul(V a, float b)
{
    return {a.x * b, a.y * b, a.z * b};
}
inline float dot(V a, V b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
struct Key
{
    int x, y, z;
    bool operator==(const Key &b) const
    {
        return x == b.x && y == b.y && z == b.z;
    }
};
struct KeyHash
{
    size_t operator()(const Key &k) const
    {
        return uint32_t(k.x) * 73856093u ^ uint32_t(k.y) * 19349663u ^ uint32_t(k.z) * 83492791u;
    }
};
struct Plane
{
    V normal;
    float intercept;
    bool active;
};
struct Cell
{
    std::array<V, 27> points;
    std::array<std::array<Plane, 27>, 27> planes;
    std::array<bool, 27> ready{};
    Cell(Key cell, float randomness)
    {
        int n = 0;
        for (int z = -1; z <= 1; z++)
            for (int y = -1; y <= 1; y++)
                for (int x = -1; x <= 1; x++)
                {
                    float a = float(cell.x) + float(x), b = float(cell.y) + float(y),
                          c = float(cell.z) + float(z);
                    points[n++] = {float(x) + hash3(a, b, c) * randomness,
                                   float(y) + hash4(a, b, c, 1.f) * randomness,
                                   float(z) + hash4(a, b, c, 2.f) * randomness};
                }
    }
    void prepare(int nearest)
    {
        if (ready[nearest])
            return;
        V p = points[nearest];
        for (int i = 0; i < 27; i++)
        {
            V delta = sub(points[i], p);
            float length2 = dot(delta, delta);
            auto &plane = planes[nearest][i];
            plane.active = length2 > .0001f;
            if (plane.active)
            {
                plane.normal = mul(delta, 1.f / std::sqrt(length2));
                plane.intercept = dot(mul(add(p, points[i]), .5f), plane.normal);
            }
        }
        ready[nearest] = true;
    }
    bool select(V local, float threshold)
    {
        int nearest = 0;
        float best = std::numeric_limits<float>::max();
        V closest{};
        for (int i = 0; i < 27; i++)
        {
            V d = sub(points[i], local);
            float value = dot(d, d);
            if (value < best)
            {
                best = value;
                nearest = i;
                closest = d;
            }
        }
        prepare(nearest);
        float distance = std::numeric_limits<float>::max();
        for (const auto &plane : planes[nearest])
            if (plane.active)
                distance = std::min(distance, plane.intercept - dot(local, plane.normal));
        // Cancellation can affect a precomputed plane at a threshold. Re-evaluate
        // those rare points with Blender's original operation order.
        if (distance < threshold - .0001f)
            return true;
        if (distance > threshold + .0001f)
            return false;
        distance = std::numeric_limits<float>::max();
        for (V p : points)
        {
            V point = sub(p, local);
            V delta = sub(point, closest);
            float length2 = dot(delta, delta);
            if (length2 > .0001f)
            {
                V normal = mul(delta, 1.f / std::sqrt(length2));
                float value = dot(mul(add(closest, point), .5f), normal);
                distance = std::min(distance, value);
            }
        }
        return distance < threshold;
    }
};
inline void mask(const float *co, size_t count, float scale, float randomness, float threshold,
                 float *out)
{
    std::unordered_map<Key, std::unique_ptr<Cell>, KeyHash> cells;
    for (size_t i = 0; i < count; i++)
    {
        V p{co[3 * i] * scale, co[3 * i + 1] * scale, co[3 * i + 2] * scale};
        V floor{std::floor(p.x), std::floor(p.y), std::floor(p.z)};
        Key key{int(floor.x), int(floor.y), int(floor.z)};
        auto it = cells.find(key);
        if (it == cells.end())
            it = cells.emplace(key, std::make_unique<Cell>(key, randomness)).first;
        out[i] = it->second->select(sub(p, floor), threshold) ? 1.f : 0.f;
    }
}
} // namespace mushroom_voronoi
