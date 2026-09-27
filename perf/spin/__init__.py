# SPDX-License-Identifier: GPL-2.0-or-later
"""Deterministic native full-turn Z spin for mushroom construction."""

import numpy as np


def spin_mesh(obj, steps, axis, dupli):
    """Replace extrusion/duplication only; draw.spin retains curve, fill and weld.

    Explicitly restricted to the identity-transform Z-axis mushroom use case.
    Unsupported inputs raise rather than silently reverting to unstable spin.
    """
    import bpy
    from mathutils import Matrix

    from ._spin import spin_buffers

    if tuple(axis) != (0, 0, 1) or obj.matrix_world != Matrix.Identity(4):
        raise ValueError("native mushroom spin requires Z axis and identity transform")
    mesh = obj.data
    if not dupli and len(mesh.polygons):
        raise ValueError("native extrusion requires an unfilled wire profile")
    if len(obj.vertex_groups) or any(
        a.name
        not in {"position", ".edge_verts", ".corner_vert", ".corner_edge", "sharp_face"}
        for a in mesh.attributes
    ):
        raise ValueError(
            f"native spin expects an unannotated profile: {[a.name for a in mesh.attributes]}, groups={list(obj.vertex_groups)}"
        )
    sharp = None
    if "sharp_face" in mesh.attributes:
        sharp = np.empty(len(mesh.polygons), np.bool_)
        mesh.attributes["sharp_face"].data.foreach_get("value", sharp)
    co = np.empty((len(mesh.vertices), 3), np.float32)
    edges = np.empty((len(mesh.edges), 2), np.int32)
    mesh.vertices.foreach_get("co", co.ravel())
    mesh.edges.foreach_get("vertices", edges.ravel())
    new_co, quads = spin_buffers(co, edges, int(steps), bool(dupli))
    if dupli:
        loops = np.empty(len(mesh.loops), np.int32)
        counts = np.empty(len(mesh.polygons), np.int32)
        mesh.loops.foreach_get("vertex_index", loops)
        mesh.polygons.foreach_get("loop_total", counts)
        new_loops = np.concatenate([loops + r * len(co) for r in range(steps + 1)])
        counts = np.tile(counts, steps + 1)
        new_edges = np.concatenate([edges + r * len(co) for r in range(steps + 1)])
    else:
        new_loops = quads.ravel()
        counts = np.full(len(quads), 4, np.int32)
        new_edges = np.empty((0, 2), np.int32)
    # Reuse the mesh datablock to preserve naming and ownership.
    mesh.clear_geometry()
    mesh.vertices.add(len(new_co))
    mesh.vertices.foreach_set("co", new_co.ravel())
    mesh.edges.add(len(new_edges))
    mesh.edges.foreach_set("vertices", new_edges.ravel())
    mesh.loops.add(len(new_loops))
    mesh.loops.foreach_set("vertex_index", new_loops)
    mesh.polygons.add(len(counts))
    mesh.polygons.foreach_set("loop_start", np.cumsum(counts, dtype=np.int32) - counts)
    mesh.polygons.foreach_set("loop_total", counts)
    if sharp is not None:
        attr = mesh.attributes.get("sharp_face")
        if attr is None:
            attr = mesh.attributes.new("sharp_face", "BOOLEAN", "FACE")
        attr.data.foreach_set("value", np.tile(sharp, steps + 1))
    mesh.update(calc_edges=True)
