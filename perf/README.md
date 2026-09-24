# Optimized-implementation kernels

Native kernels used by Infinigen's opt-in optimized implementations
(`infinigen.perf`), adapted from GPL-licensed Blender 4.2 sources. Each
file states its license and upstream source; all are GPL-2.0-or-later,
and `voronoi/` is additionally BSD-3-Clause where noted.

- `particle_sampler/`: tree point sampling (`TREES`)
- `spin/`: mushroom spin (`MUSHROOM`)
- `voronoi/`: mushroom field kernels (`MUSHROOM`)
- `pinecone/`: STUCCI/normals and twist/bend (`MONOCOT`)

Built by Infinigen's `setup.py` when `INFINIGEN_INSTALL_PERF=True` and this
submodule is checked out. Runtime never compiles code.

Every implementation that uses one of these falls back to Infinigen's
default implementation when the kernel is not built, so Infinigen never
requires this directory.
