# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright 2026 Princeton University
# cython: language_level=3, boundscheck=False, wraparound=False
import numpy as np
cimport numpy as cnp
from libc.stdint cimport int32_t
cnp.import_array()
cdef extern from "_spin_core.hpp" namespace "infinigen::spin":
    void rings(const float*, int, int, bint, float*) noexcept nogil
    void quads(const int32_t*, int, int, int, int32_t*) noexcept nogil

def spin_buffers(cnp.ndarray[cnp.float32_t, ndim=2, mode='c'] vertices,
                 cnp.ndarray[cnp.int32_t, ndim=2, mode='c'] edges,
                 int steps, bint duplicate=False):
    if vertices.shape[1] != 3 or edges.shape[1] != 2:
        raise ValueError('expected vertices (N,3) and edges (E,2)')
    if not 3 <= steps <= 10000 or vertices.shape[0] == 0:
        raise ValueError('spin requires vertices and 3..10000 steps')
    if not np.isfinite(vertices).all():
        raise ValueError('vertices must be finite')
    if edges.size and (edges.min() < 0 or edges.max() >= vertices.shape[0]):
        raise ValueError('edge index out of range')
    if vertices.shape[0] * (steps+1) > 2147483647:
        raise ValueError('spin exceeds int32 index range')
    cdef int n=vertices.shape[0], ne=edges.shape[0]
    cdef cnp.ndarray[cnp.float32_t, ndim=2, mode='c'] co=np.empty(((steps+1 if duplicate else steps)*n,3),np.float32)
    cdef cnp.ndarray[cnp.int32_t, ndim=2, mode='c'] faces=np.empty((0 if duplicate else steps*ne,4),np.int32)
    with nogil:
        rings(<float*>vertices.data,n,steps,duplicate,<float*>co.data)
        if not duplicate and ne:
            quads(<int32_t*>edges.data,n,ne,steps,<int32_t*>faces.data)
    return co,faces
