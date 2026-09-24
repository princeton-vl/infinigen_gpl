# Mushroom field kernels

GPL-2.0-or-later AND BSD-3-Clause; see `../../LICENSE` and
`LICENSE-BSD-3-Clause.txt`. Adapted from Blender 4.2.0:

- `source/blender/blenlib/intern/noise.cc`: Jenkins hashes, 3D Voronoi
  distance-to-edge, signed Perlin noise and fBM arithmetic.
- `source/blender/blenlib/intern/noise.c`: original Blender noise tables and
  evaluator.
- `source/blender/render/intern/texture_procedural.cc`: STUCCI and MARBLE.
- `source/blender/render/intern/texture_common.h`: default intensity/contrast.

Upstream: https://github.com/blender/blender/tree/v4.2.0/source/blender

Built by Infinigen's `setup.py` when `INFINIGEN_INSTALL_PERF=True` and this
submodule is checked out. Runtime never compiles code.
Fast math and contraction are disabled explicitly.

Only the optimized mushroom (`MUSHROOM`) calls these; there they are on unless
`INFINIGEN_MUSHROOM_NATIVE_FIELDS=0`, which keeps Blender's modifiers and
geometry nodes.
Unsupported texture bases, waves, ramps, or intensity/contrast use Blender.
The extension kernel implements the particular two-octave signed fBM used by
`geo_extension`, not arbitrary geometry node graphs. These are Blender 4.2
arithmetic implementations, not cross-version promises.

The Voronoi cache is local to one field evaluation. It caches lattice feature
points and bisector planes and recomputes original arithmetic near the mask
threshold. It does not share mushroom parameters, geometry or deformations.
Texture/extension evaluation still samples every vertex at the matched mesh
resolution. The extension replacement can differ by float32 rounding; see the
numerical validation report. Visual acceptance belongs to the user.
