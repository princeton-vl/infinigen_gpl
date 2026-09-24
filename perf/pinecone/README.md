# Pinecone Blender-derived kernels

- `stucci.cpp`: Blender 4.2 noise tables/evaluator and mesh normal weighting; GPL-2.0-or-later.
- `deform.cpp`: Blender 4.2 Simple Deform twist/bend equations; GPL-2.0-or-later.

Loaded with ctypes by the optimized pinecone (`MONOCOT`), which falls back to
the default implementation when they are not built. Its other kernels are
BSD-3-Clause and live in Infinigen itself.

Built by Infinigen's `setup.py` when `INFINIGEN_INSTALL_PERF=True` and this
submodule is checked out. Runtime never compiles code.
