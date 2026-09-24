// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright 2026 Princeton University
// Specialized full-turn Z spin adapted from Blender 4.2.0 bmo_spin_exec
// (source/blender/bmesh/operators/bmo_dupe.cc) and boundary face winding from
// bmo_extrude.cc. Fixed index traversal replaces pointer-hash traversal.
#pragma once
#include <cmath>
#include <cstdint>
namespace infinigen { namespace spin {
inline void rings(const float *input, int n, int steps, bool duplicate, float *out) {
  const float angle = -float(6.28318530717958647692);
  const float phi = angle / steps;
  const float c = cosf(phi), s = sinf(phi);
  for (int i=0;i<n*3;++i) out[i]=input[i];
  const int count = duplicate ? steps+1 : steps;
  for (int r=1;r<count;++r) for(int v=0;v<n;++v) {
    const float *p=out+((r-1)*n+v)*3;
    float *q=out+(r*n+v)*3;
    q[0]=(c*p[0] + (-s)*p[1]) + 0.0f*p[2];
    q[1]=(s*p[0] + c*p[1]) + 0.0f*p[2];
    q[2]=(0.0f*p[0] + 0.0f*p[1]) + p[2];
  }
}
inline void quads(const int32_t *edges, int n, int ne, int steps, int32_t *out) {
  for (int r=0;r<steps;++r) for(int e=0;e<ne;++e) {
    const int a=edges[2*e], b=edges[2*e+1];
    int32_t *f=out+(r*ne+e)*4;
    f[0]=r*n+a; f[1]=r*n+b;
    f[2]=((r+1)%steps)*n+b; f[3]=((r+1)%steps)*n+a;
  }
}
}}
