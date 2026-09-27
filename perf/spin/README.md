# Native mushroom spin

GPL-2.0-or-later; see `../../LICENSE`. The ring-generation and face-winding logic
is adapted from Blender 4.2.0:

- `source/blender/bmesh/operators/bmo_dupe.cc`, `bmo_spin_exec`
- `source/blender/bmesh/operators/bmo_extrude.cc`, `bmo_extrude_face_region_exec`
- `source/blender/editors/mesh/editmesh_extrude_spin.cc`, `edbm_spin_exec`
- `source/blender/blenlib/intern/math_rotation.c`, `axis_angle_normalized_to_mat3`

Upstream: https://github.com/blender/blender/tree/v4.2.0/source/blender

Built by Infinigen's `setup.py` when `INFINIGEN_INSTALL_PERF=True` and this
submodule is checked out. Runtime never compiles code.

Only the optimized mushroom (`MUSHROOM`) calls this: its copy of `draw.spin`
(`infinigen.perf.assets.objects.mushroom.meshing`) extrudes with `spin_mesh`
in place of Blender's spin operator. With `MUSHROOM` not selected nothing here
runs.

This implements the full-turn, positive-Z-axis, identity-transform use case in
mushrooms. It accepts unannotated wire profiles for extrusion and filled profiles
for duplication. Unsupported axes, transforms and custom attributes raise an
error. It is not a general replacement for Blender's interactive spin operator.

Native C++ generates successive float32 rings with Blender's negative editor
angle and incremental arithmetic. Extrusion connects rings in input edge order;
it never iterates a pointer hash. The final extrusion ring connects to the first
ring, matching automatic full-turn merge. Duplication includes the last rotated
copy, matching legacy duplication before remove-doubles. Python assembles the
Blender mesh buffers, retaining the object and mesh datablock. Curve conversion,
pre-weld, optional fill/remesh, and final remove-doubles stay in that copy of
`draw.spin`.

Changing legacy face ordering establishes a new baseline. It is not claimed
byte-identical to legacy complete mushrooms, nor does it make `.blend` file
serialization deterministic. Repeatability claims are scoped to the tested
Blender version, build and platform. Visual acceptance belongs to the user.
