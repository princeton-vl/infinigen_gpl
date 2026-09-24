// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright 2026 Princeton University
//
// Derived from Blender 4.2's GPL-licensed particle distribution and random
// number generation code:
//   source/blender/blenkernel/intern/particle_distribute.cc
//   source/blender/blenlib/intern/rand.cc
// Copyright Blender Authors.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cfloat>
#include <vector>

namespace infinigen::particle_sampler {

struct Vec3 {
  float x, y, z;
};

struct VecLen {
  Vec3 direction;
  float length;
};

class BlenderRNG {
 public:
  explicit BlenderRNG(uint32_t seed) { set_seed(seed); }

  void set_seed(uint32_t seed) { state_ = (uint64_t(seed) << 16) | UINT64_C(0x330e); }

  uint32_t next_uint() {
    state_ = (UINT64_C(0x5deece66d) * state_ + UINT64_C(0xb)) &
             UINT64_C(0x0000ffffffffffff);
    return uint32_t(state_ >> 17);
  }

  float next_float() { return float(int32_t(next_uint())) / 2147483648.0f; }

  void set_randomized_seed(uint32_t seed) {
    for (int i = 0; i < 2; ++i) {
      set_seed(seed + noise_hash_[seed & 255]);
      seed = next_uint();
    }
    set_seed(seed + noise_hash_[seed & 255]);
  }

 private:
  uint64_t state_;
  static constexpr uint8_t noise_hash_[256] = {
      0xa2, 0xa0, 0x19, 0x3b, 0xf8, 0xeb, 0xaa, 0xee, 0xf3, 0x1c, 0x67, 0x28,
      0x1d, 0xed, 0x00, 0xde, 0x95, 0x2e, 0xdc, 0x3f, 0x3a, 0x82, 0x35, 0x4d,
      0x6c, 0xba, 0x36, 0xd0, 0xf6, 0x0c, 0x79, 0x32, 0xd1, 0x59, 0xf4, 0x08,
      0x8b, 0x63, 0x89, 0x2f, 0xb8, 0xb4, 0x97, 0x83, 0xf2, 0x8f, 0x18, 0xc7,
      0x51, 0x14, 0x65, 0x87, 0x48, 0x20, 0x42, 0xa8, 0x80, 0xb5, 0x40, 0x13,
      0xb2, 0x22, 0x7e, 0x57, 0xbc, 0x7f, 0x6b, 0x9d, 0x86, 0x4c, 0xc8, 0xdb,
      0x7c, 0xd5, 0x25, 0x4e, 0x5a, 0x55, 0x74, 0x50, 0xcd, 0xb3, 0x7a, 0xbb,
      0xc3, 0xcb, 0xb6, 0xe2, 0xe4, 0xec, 0xfd, 0x98, 0x0b, 0x96, 0xd3, 0x9e,
      0x5c, 0xa1, 0x64, 0xf1, 0x81, 0x61, 0xe1, 0xc4, 0x24, 0x72, 0x49, 0x8c,
      0x90, 0x4b, 0x84, 0x34, 0x38, 0xab, 0x78, 0xca, 0x1f, 0x01, 0xd7, 0x93,
      0x11, 0xc1, 0x58, 0xa9, 0x31, 0xf9, 0x44, 0x6d, 0xbf, 0x33, 0x9c, 0x5f,
      0x09, 0x94, 0xa3, 0x85, 0x06, 0xc6, 0x9a, 0x1e, 0x7b, 0x46, 0x15, 0x30,
      0x27, 0x2b, 0x1b, 0x71, 0x3c, 0x5b, 0xd6, 0x6f, 0x62, 0xac, 0x4f, 0xc2,
      0xc0, 0x0e, 0xb1, 0x23, 0xa7, 0xdf, 0x47, 0xb0, 0x77, 0x69, 0x05, 0xe9,
      0xe6, 0xe7, 0x76, 0x73, 0x0f, 0xfe, 0x6e, 0x9b, 0x56, 0xef, 0x12, 0xa5,
      0x37, 0xfc, 0xae, 0xd9, 0x03, 0x8e, 0xdd, 0x10, 0xb9, 0xce, 0xc9, 0x8d,
      0xda, 0x2a, 0xbd, 0x68, 0x17, 0x9f, 0xbe, 0xd4, 0x0a, 0xcc, 0xd2, 0xe8,
      0x43, 0x3d, 0x70, 0xb7, 0x02, 0x7d, 0x99, 0xd8, 0x0d, 0x60, 0x8a, 0x04,
      0x2c, 0x3e, 0x92, 0xe5, 0xaf, 0x53, 0x07, 0xe0, 0x29, 0xa6, 0xc5, 0xe3,
      0xf5, 0xf7, 0x4a, 0x41, 0x26, 0x6a, 0x16, 0x5e, 0x52, 0x2d, 0x21, 0xad,
      0xf0, 0x91, 0xff, 0xea, 0x54, 0xfa, 0x66, 0x1a, 0x45, 0x39, 0xcf, 0x75,
      0xa4, 0x88, 0xfb, 0x5d};
};

constexpr uint8_t BlenderRNG::noise_hash_[256];

inline Vec3 add(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 subtract(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 scale(Vec3 a, float factor) { return {a.x * factor, a.y * factor, a.z * factor}; }
inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
          a.x * b.y - a.y * b.x};
}
inline float length_squared(Vec3 a) { return dot(a, a); }
inline float length(Vec3 a) { return std::sqrt(length_squared(a)); }
inline Vec3 normalize(Vec3 a) {
  const float magnitude = length(a);
  return magnitude == 0.0f ? Vec3{0.0f, 0.0f, 0.0f} : scale(a, 1.0f / magnitude);
}

inline Vec3 vertex(const float *vertices, int index) {
  return {vertices[3 * index], vertices[3 * index + 1], vertices[3 * index + 2]};
}

inline VecLen vector_and_length(Vec3 point, Vec3 origin) {
  Vec3 direction = subtract(point, origin);
  return {direction, length(direction)};
}

inline float half_tangent(const VecLen &first, const VecLen &second) {
  const float area = length(cross(first.direction, second.direction));
  if (area != 0.0f) {
    const float result =
        (first.length * second.length - dot(first.direction, second.direction)) / area;
    if (std::isfinite(result)) {
      return result;
    }
  }
  return 0.0f;
}

inline float point_segment_distance_squared(Vec3 point, Vec3 start, Vec3 end) {
  const Vec3 segment = subtract(end, start);
  const float segment_length_squared = length_squared(segment);
  if (segment_length_squared == 0.0f) {
    return length_squared(subtract(point, start));
  }
  float factor = dot(subtract(point, start), segment) / segment_length_squared;
  factor = std::max(0.0f, std::min(1.0f, factor));
  return length_squared(subtract(point, add(start, scale(segment, factor))));
}

inline void interpolation_weights(float u, float v, int corners, float weights[4]) {
  if (corners == 3) {
    if (u + v > 1.0f) {
      v = 1.0f - v;
    } else {
      u = 1.0f - u;
    }
  }
  const Vec3 canonical[4] = {
      {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
      {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
  const Vec3 point{u, v, 0.0f};
  float max_value = 0.0f;
  for (int i = 0; i < corners; ++i) {
    max_value = std::max(max_value, std::fabs(canonical[i].x - point.x));
    max_value = std::max(max_value, std::fabs(canonical[i].y - point.y));
  }
  const float epsilon = 16.0f * FLT_EPSILON * max_value;
  const float epsilon_squared = epsilon * epsilon;
  int current = corners - 1;
  Vec3 current_vertex = canonical[current];
  VecLen previous_direction = vector_and_length(canonical[corners - 2], point);
  VecLen current_direction = vector_and_length(current_vertex, point);
  float previous_half_tangent = half_tangent(previous_direction, current_direction);
  float total = 0.0f;
  bool exact_point = false;
  bool exact_segment = false;
  int next = 0;
  for (; next < corners; ++next) {
    const Vec3 next_vertex = canonical[next];
    if (current_direction.length < epsilon) {
      exact_point = true;
      break;
    }
    if (point_segment_distance_squared(point, current_vertex, next_vertex) < epsilon_squared) {
      exact_segment = true;
      break;
    }
    previous_direction = current_direction;
    current_direction = vector_and_length(next_vertex, point);
    const float current_half_tangent = half_tangent(previous_direction, current_direction);
    weights[current] =
        (previous_half_tangent + current_half_tangent) / previous_direction.length;
    total += weights[current];
    current = next;
    current_vertex = next_vertex;
    previous_half_tangent = current_half_tangent;
  }
  if (exact_point || exact_segment) {
    for (int i = 0; i < corners; ++i) {
      weights[i] = 0.0f;
    }
    if (exact_point) {
      weights[current] = 1.0f;
    } else {
      const Vec3 segment = subtract(canonical[next], current_vertex);
      float factor = dot(subtract(point, current_vertex), segment) / length_squared(segment);
      factor = std::max(0.0f, std::min(1.0f, factor));
      weights[current] = 1.0f - factor;
      weights[next] = factor;
    }
  } else if (total != 0.0f) {
    for (int i = 0; i < corners; ++i) {
      weights[i] /= total;
    }
  }
  if (corners == 3) {
    weights[3] = 0.0f;
  }
}

inline Vec3 interpolate(const float *values, const int32_t *face, int corners,
                        const float weights[4]) {
  Vec3 result{0.0f, 0.0f, 0.0f};
  for (int i = 0; i < corners; ++i) {
    result.x += values[3 * face[i]] * weights[i];
    result.y += values[3 * face[i] + 1] * weights[i];
    result.z += values[3 * face[i] + 2] * weights[i];
  }
  return result;
}

inline Vec3 face_normal(const float *vertices, const int32_t *face, int corners) {
  const Vec3 first = vertex(vertices, face[0]);
  const Vec3 second = vertex(vertices, face[1]);
  const Vec3 third = vertex(vertices, face[2]);
  if (corners == 3) {
    return normalize(cross(subtract(first, second), subtract(second, third)));
  }
  const Vec3 fourth = vertex(vertices, face[3]);
  return normalize(cross(subtract(first, third), subtract(second, fourth)));
}

inline Vec3 surface_normal(const float *vertices, const float *normals,
                           const int32_t *face, int corners, bool smooth,
                           const float weights[4]) {
  return smooth ? interpolate(normals, face, corners, weights)
                : face_normal(vertices, face, corners);
}

inline void add_newell_term(Vec3 &normal, Vec3 previous, Vec3 current) {
  normal.x += (previous.y - current.y) * (previous.z + current.z);
  normal.y += (previous.z - current.z) * (previous.x + current.x);
  normal.z += (previous.x - current.x) * (previous.y + current.y);
}

inline float face_area(const float *vertices, const int32_t *face, int corners) {
  if (corners == 3) {
    return 0.5f * length(cross(subtract(vertex(vertices, face[0]), vertex(vertices, face[1])),
                               subtract(vertex(vertices, face[1]), vertex(vertices, face[2]))));
  }
  Vec3 normal{0.0f, 0.0f, 0.0f};
  Vec3 previous = vertex(vertices, face[corners - 1]);
  for (int i = 0; i < corners; ++i) {
    const Vec3 current = vertex(vertices, face[i]);
    add_newell_term(normal, previous, current);
    previous = current;
  }
  return 0.5f * length(normal);
}

inline bool ray_triangle(Vec3 origin, Vec3 direction, Vec3 v0, Vec3 v1, Vec3 v2,
                         float &distance) {
  constexpr float epsilon = 0.00000001f;
  const Vec3 edge1 = subtract(v1, v0);
  const Vec3 edge2 = subtract(v2, v0);
  const Vec3 p = cross(direction, edge2);
  const float determinant = dot(edge1, p);
  if (determinant > -epsilon && determinant < epsilon) {
    return false;
  }
  const float inverse = 1.0f / determinant;
  const Vec3 s = subtract(origin, v0);
  const float u = inverse * dot(s, p);
  if (u < 0.0f || u > 1.0f) {
    return false;
  }
  const Vec3 q = cross(s, edge1);
  const float v = inverse * dot(direction, q);
  if (v < 0.0f || u + v > 1.0f) {
    return false;
  }
  distance = inverse * dot(edge2, q);
  return distance >= 0.0f;
}

struct TriangleRef {
  int face_index;
  int order;
  int32_t a, b, c;
  Vec3 minimum;
  Vec3 maximum;
  Vec3 centroid;
};

struct BVHNode {
  Vec3 minimum;
  Vec3 maximum;
  int left = -1;
  int right = -1;
  int start = 0;
  int count = 0;
};

inline float component(Vec3 value, int axis) {
  return axis == 0 ? value.x : (axis == 1 ? value.y : value.z);
}

inline Vec3 min_components(Vec3 a, Vec3 b) {
  return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

inline Vec3 max_components(Vec3 a, Vec3 b) {
  return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}

inline int build_bvh(std::vector<BVHNode> &nodes, std::vector<TriangleRef> &triangles,
                     int start, int end) {
  Vec3 minimum{FLT_MAX, FLT_MAX, FLT_MAX};
  Vec3 maximum{-FLT_MAX, -FLT_MAX, -FLT_MAX};
  Vec3 centroid_minimum{FLT_MAX, FLT_MAX, FLT_MAX};
  Vec3 centroid_maximum{-FLT_MAX, -FLT_MAX, -FLT_MAX};
  for (int i = start; i < end; ++i) {
    minimum = min_components(minimum, triangles[i].minimum);
    maximum = max_components(maximum, triangles[i].maximum);
    centroid_minimum = min_components(centroid_minimum, triangles[i].centroid);
    centroid_maximum = max_components(centroid_maximum, triangles[i].centroid);
  }

  const float padding = 64.0f * FLT_EPSILON *
                        (1.0f + std::max({std::fabs(minimum.x), std::fabs(minimum.y),
                                          std::fabs(minimum.z), std::fabs(maximum.x),
                                          std::fabs(maximum.y), std::fabs(maximum.z)}));
  minimum = {minimum.x - padding, minimum.y - padding, minimum.z - padding};
  maximum = {maximum.x + padding, maximum.y + padding, maximum.z + padding};

  const int node_index = int(nodes.size());
  nodes.push_back({minimum, maximum, -1, -1, start, end - start});
  if (end - start <= 8) {
    return node_index;
  }

  const Vec3 extent = subtract(centroid_maximum, centroid_minimum);
  const int axis = extent.y > extent.x ? (extent.z > extent.y ? 2 : 1)
                                        : (extent.z > extent.x ? 2 : 0);
  const int middle = start + (end - start) / 2;
  std::nth_element(
      triangles.begin() + start, triangles.begin() + middle, triangles.begin() + end,
      [axis](const TriangleRef &a, const TriangleRef &b) {
        return component(a.centroid, axis) < component(b.centroid, axis);
      });
  const int left = build_bvh(nodes, triangles, start, middle);
  const int right = build_bvh(nodes, triangles, middle, end);
  nodes[node_index].left = left;
  nodes[node_index].right = right;
  nodes[node_index].count = 0;
  return node_index;
}

inline bool ray_box(Vec3 origin, Vec3 direction, const BVHNode &node,
                    float maximum_distance) {
  double near_distance = 0.0;
  double far_distance = double(maximum_distance);
  for (int axis = 0; axis < 3; ++axis) {
    const double o = component(origin, axis);
    const double d = component(direction, axis);
    const double minimum = component(node.minimum, axis);
    const double maximum = component(node.maximum, axis);
    if (d == 0.0) {
      if (o < minimum || o > maximum) {
        return false;
      }
      continue;
    }
    double first = (minimum - o) / d;
    double second = (maximum - o) / d;
    if (first > second) {
      std::swap(first, second);
    }
    near_distance = std::max(near_distance, first);
    far_distance = std::min(far_distance, second);
    if (far_distance < near_distance) {
      return false;
    }
  }
  return far_distance >= 0.0;
}

inline int find_face(const std::vector<float> &cumulative, float position) {
  int low = 0;
  int high = int(cumulative.size()) - 1;
  if (high == low || cumulative[low] >= position) {
    return low;
  }
  if (cumulative[high - 1] < position) {
    return high;
  }
  while (low < high) {
    const int middle = (low + high) / 2;
    if (cumulative[middle] >= position && cumulative[middle - 1] < position) {
      return middle;
    }
    if (cumulative[middle] > position) {
      high = middle - 1;
    } else {
      low = middle + 1;
    }
  }
  return low;
}

inline Vec3 transform_point(const float *matrix, Vec3 point) {
  return {
      matrix[0] * point.x + matrix[1] * point.y + matrix[2] * point.z + matrix[3],
      matrix[4] * point.x + matrix[5] * point.y + matrix[6] * point.z + matrix[7],
      matrix[8] * point.x + matrix[9] * point.y + matrix[10] * point.z + matrix[11]};
}

inline void sample_points(const float *vertices, const float *normals,
                          const int32_t *faces, const uint8_t *smooth, int face_count,
                          const float *matrix, int point_count, uint32_t seed,
                          bool volume, float *output, int32_t *source_output) {
  std::vector<float> weights(face_count);
  float total_area = 0.0f;
  for (int i = 0; i < face_count; ++i) {
    const int32_t *face = faces + 4 * i;
    const int corners = face[3] < 0 ? 3 : 4;
    weights[i] = face_area(vertices, face, corners);
    total_area += weights[i];
  }
  for (float &weight : weights) {
    weight /= total_area;
  }
  float total_weight = 0.0f;
  for (const float weight : weights) {
    if (weight > 0.0f) {
      total_weight += weight;
    }
  }
  const float inverse_total_weight = 1.0f / total_weight;
  std::vector<float> cumulative;
  std::vector<int> face_map;
  cumulative.reserve(face_count);
  face_map.reserve(face_count);
  for (int i = 0; i < face_count; ++i) {
    if (weights[i] <= 0.0f) {
      continue;
    }
    const float next = (cumulative.empty() ? 0.0f : cumulative.back()) +
                       weights[i] * inverse_total_weight;
    if (cumulative.empty() || next > cumulative.back()) {
      cumulative.push_back(next);
      face_map.push_back(i);
    }
  }

  BlenderRNG face_rng(0);
  face_rng.set_randomized_seed(uint32_t(31415926) + seed);
  std::vector<int> selected_faces(point_count);
  for (int point_index = 0; point_index < point_count; ++point_index) {
    const float position = face_rng.next_float() * cumulative.back();
    selected_faces[point_index] = face_map[find_face(cumulative, position)];
    source_output[point_index] = selected_faces[point_index];
  }

  std::vector<TriangleRef> triangles;
  triangles.reserve(2 * face_count);
  for (int face_index = 0; face_index < face_count; ++face_index) {
    const int32_t *face = faces + 4 * face_index;
    const int triangle_count = face[3] < 0 ? 1 : 2;
    const int32_t triangle_vertices[2][3] = {
        {face[1], face[2], face[0]}, {face[3], face[0], face[2]}};
    for (int triangle_index = 0; triangle_index < triangle_count; ++triangle_index) {
      const int32_t a = triangle_vertices[triangle_index][0];
      const int32_t b = triangle_vertices[triangle_index][1];
      const int32_t c = triangle_vertices[triangle_index][2];
      const Vec3 va = vertex(vertices, a);
      const Vec3 vb = vertex(vertices, b);
      const Vec3 vc = vertex(vertices, c);
      const Vec3 minimum = min_components(va, min_components(vb, vc));
      const Vec3 maximum = max_components(va, max_components(vb, vc));
      const Vec3 centroid = scale(add(va, add(vb, vc)), 1.0f / 3.0f);
      triangles.push_back(
          {face_index, int(triangles.size()), a, b, c, minimum, maximum, centroid});
    }
  }
  std::vector<BVHNode> bvh;
  bvh.reserve(2 * triangles.size());
  if (!triangles.empty()) {
    build_bvh(bvh, triangles, 0, int(triangles.size()));
  }
  std::vector<int> traversal_stack;
  traversal_stack.reserve(bvh.size());
  std::vector<int> candidate_triangles;
  candidate_triangles.reserve(64);

  BlenderRNG coordinate_rng(uint32_t(31415926) + seed);
  for (int point_index = 0; point_index < point_count; ++point_index) {
    const float random_u = coordinate_rng.next_float();
    const float random_v = coordinate_rng.next_float();
    const float random_depth = coordinate_rng.next_float();
    const int source_index = selected_faces[point_index];
    const int32_t *source_face = faces + 4 * source_index;
    const int source_corners = source_face[3] < 0 ? 3 : 4;
    float interpolation[4];
    interpolation_weights(random_u, random_v, source_corners, interpolation);
    const Vec3 surface = interpolate(vertices, source_face, source_corners, interpolation);
    Vec3 local_point = surface;

    if (volume) {
      Vec3 inward = normalize(surface_normal(vertices, normals, source_face, source_corners,
                                             smooth[source_index] != 0, interpolation));
      inward = scale(inward, -1.0f);
      float closest = FLT_MAX;
      bool intersected = false;
      traversal_stack.clear();
      candidate_triangles.clear();
      traversal_stack.push_back(0);
      while (!traversal_stack.empty()) {
        const int node_index = traversal_stack.back();
        traversal_stack.pop_back();
        const BVHNode &node = bvh[node_index];
        if (!ray_box(surface, inward, node, closest)) {
          continue;
        }
        if (node.count == 0) {
          traversal_stack.push_back(node.left);
          traversal_stack.push_back(node.right);
          continue;
        }
        for (int candidate_index = node.start;
             candidate_index < node.start + node.count; ++candidate_index) {
          const TriangleRef &candidate = triangles[candidate_index];
          if (candidate.face_index == source_index) {
            continue;
          }
          candidate_triangles.push_back(candidate_index);
        }
      }
      std::sort(candidate_triangles.begin(), candidate_triangles.end(),
                [&triangles](int first, int second) {
                  return triangles[first].order < triangles[second].order;
                });
      for (const int candidate_index : candidate_triangles) {
        const TriangleRef &candidate = triangles[candidate_index];
        float distance;
        if (ray_triangle(surface, inward, vertex(vertices, candidate.a),
                         vertex(vertices, candidate.b), vertex(vertices, candidate.c),
                         distance) &&
            distance < closest) {
          closest = distance;
          intersected = true;
        }
      }
      if (intersected) {
        const float offset = closest * 0.5f * random_depth;
        Vec3 final_normal = normalize(surface_normal(
            vertices, normals, source_face, source_corners, smooth[source_index] != 0,
            interpolation));
        local_point = add(surface, scale(final_normal, -offset));
      }
    }

    const Vec3 world_point = transform_point(matrix, local_point);
    output[3 * point_index] = world_point.x;
    output[3 * point_index + 1] = world_point.y;
    output[3 * point_index + 2] = world_point.z;
  }
}

}  // namespace infinigen::particle_sampler
