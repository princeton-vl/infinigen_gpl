# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright 2026 Princeton University
#
# This extension exposes the accompanying Blender-derived particle sampler to
# Infinigen. See _particle_sampler_core.hpp for upstream source attribution.

# cython: boundscheck=False, wraparound=False, cdivision=True, language_level=3

import numpy as np
cimport numpy as cnp
from libc.stdint cimport int32_t, uint8_t, uint32_t

cnp.import_array()

cdef extern from "_particle_sampler_core.hpp" namespace "infinigen::particle_sampler":
    void sample_points(
        const float *vertices,
        const float *normals,
        const int32_t *faces,
        const uint8_t *smooth,
        int face_count,
        const float *matrix,
        int point_count,
        uint32_t seed,
        bint volume,
        float *output,
        int32_t *source_output,
    ) except + nogil


def sample_mesh(
    cnp.ndarray[cnp.float32_t, ndim=2, mode="c"] vertices,
    cnp.ndarray[cnp.float32_t, ndim=2, mode="c"] normals,
    cnp.ndarray[cnp.int32_t, ndim=2, mode="c"] faces,
    cnp.ndarray[cnp.uint8_t, ndim=1, mode="c"] smooth,
    cnp.ndarray[cnp.float32_t, ndim=2, mode="c"] matrix,
    int count,
    int seed,
    bint volume=True,
    bint return_source_faces=False,
):
    """Sample Blender-compatible random points from triangle/quad mesh buffers."""
    if vertices.shape[1] != 3 or normals.shape[1] != 3:
        raise ValueError("vertices and normals must have shape (N, 3)")
    if normals.shape[0] != vertices.shape[0]:
        raise ValueError("normals must have one row per vertex")
    if faces.shape[1] != 4:
        raise ValueError("faces must have shape (F, 4)")
    if smooth.shape[0] != faces.shape[0]:
        raise ValueError("smooth must have one value per face")
    if matrix.shape[0] != 4 or matrix.shape[1] != 4:
        raise ValueError("matrix must have shape (4, 4)")
    if faces.shape[0] == 0:
        raise ValueError("cannot sample a mesh without faces")
    if count < 0:
        raise ValueError("count must be non-negative")

    cdef cnp.ndarray[cnp.float32_t, ndim=2, mode="c"] output = np.empty(
        (count, 3), dtype=np.float32
    )
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode="c"] source_faces = np.empty(
        count, dtype=np.int32
    )
    if count:
        with nogil:
            sample_points(
                &vertices[0, 0],
                &normals[0, 0],
                &faces[0, 0],
                &smooth[0],
                faces.shape[0],
                &matrix[0, 0],
                count,
                <uint32_t>seed,
                volume,
                &output[0, 0],
                &source_faces[0],
            )
    if return_source_faces:
        return output, source_faces
    return output
