# Procedural Terrain Generation System

![Terrain overview](docs/high-oct.png)

A real-time terrain generation system built with **C++20** and **OpenGL
4.5**, developed as part of the **CSD3183 -- Artificial Intelligence for
Games** curriculum.\
This project explores how classical algorithms---**Poisson Disk
Sampling**, **Delaunay Triangulation**, and **Perlin Noise**---can be
combined to generate expansive, natural-looking 3D landscapes in real
time.

The focus is on *controllable procedural content generation*: producing
terrains that are deterministic, repeatable, and infinitely variable,
while remaining efficient enough for interactive applications such as
games and simulations.

------------------------------------------------------------------------

## Implementation Details

This terrain generator follows a simple pipeline:

1.  **Sample points (Poisson Disk Sampling)**\
2.  **Triangulate the points (Delaunay Triangulation)**\
3.  **Build a renderable mesh (vertices + indices)**\
4.  **Apply height (Perlin Noise)**\
5.  **Recalculate normals and render (OpenGL)**

------------------------------------------------------------------------

### 1) Domain Setup

The terrain is generated over a 2D rectangular domain (XZ plane),
e.g. **200 × 200** units:

-   `x ∈ [0, width]`\
-   `z ∈ [0, depth]`\
-   `y` represents height (computed later)

Typical inputs include:

-   `width`, `depth` (e.g. 200, 200)\
-   `pointCount` (target number of points)\
-   `seed` (for deterministic generation)\
-   Noise parameters (frequency, amplitude, octaves, etc.)

------------------------------------------------------------------------

### 2) Even Point Distribution (Poisson Disk Sampling)

To avoid clumping (common with purely random points), Poisson Disk
Sampling is used to generate points that are:

-   **Evenly spaced**\
-   **Natural-looking**\
-   **Stable for triangulation**

Given a target `pointCount`, the sampler fills the domain while
enforcing a minimum distance between points (derived from the desired
density). This produces a set of 2D sample points:

    P = { (x0, z0), (x1, z1), ... }

These points represent the terrain's *control vertices* before height is
applied.

------------------------------------------------------------------------

### 3) Mesh Topology (Delaunay Triangulation)

Once points are sampled, Delaunay triangulation is performed to produce
a triangle mesh with strong geometric properties:

-   Avoids long, skinny triangles where possible\
-   Ensures robust connectivity across the domain\
-   Produces clean topology for large-scale terrain

The output is a list of triangle indices referencing the sampled points:

    T = { (i0, i1, i2), (i3, i4, i5), ... }

------------------------------------------------------------------------

### 4) Mesh Construction (Vertices + Indices)

The triangulation output is converted into a GPU-friendly mesh.

**Vertices** - Position: `(x, y, z)`\
- Normal: computed after height is applied\
- (Optional) UVs: derived from `(x / width, z / depth)` or a chosen
tiling scheme

**Indices** - Triangle index buffer built directly from the
triangulation result

At this stage, the mesh is flat (all `y = 0`).

------------------------------------------------------------------------

### 5) Height Variation (Perlin Noise)

To produce terrain features (mountains, valleys, slopes), Perlin noise
is sampled per vertex.

For each vertex `(x, z)`:

    height = Perlin(x * frequency, z * frequency) * amplitude
    y = height

Optional extensions include:

-   Multi-octave noise (FBM) for richer detail\
-   Domain warping or ridge noise for stylized terrain

Because the system is seed-driven, changing the seed produces an
entirely new terrain while remaining fully deterministic.

------------------------------------------------------------------------

### 6) Normals, Coloring, and Rendering (OpenGL)

After height displacement:

-   **Vertex normals are recomputed** from triangle faces for correct
    lighting.\
    Face normals are accumulated into each vertex normal, then
    normalized to produce smooth shading.

-   **Vertex coloring is generated from height (`y`)** to simulate
    landmass.\
    First, compute the terrain's `minY` and `maxY`. Each vertex height
    is normalized:

        t = clamp((y - minY) / (maxY - minY), 0, 1)

    Colors are assigned by height bands and smoothly interpolated:

    -   **Below 0** → Blue (water)\
    -   **Near 0** → Beige (sand / shoreline)\
    -   **Mid-range** → Green (grassland)\
    -   **Peaks** → White (snow / mountain tops)

-   **Mesh data is uploaded to the GPU** (VBO / IBO / VAO)\

-   **Rendering** via OpenGL 4.5 pipeline with lighting and shading

------------------------------------------------------------------------

## Summary

Given a domain (e.g. **200x200**) and a target point count:

-   **Poisson Disk Sampling** creates evenly spaced points\
-   **Delaunay Triangulation** converts points into stable triangles\
-   A **mesh** is built from vertices and indices\
-   **Perlin noise** displaces vertex height to form natural terrain\
-   Normals are recalculated and the result is rendered in real time

------------------------------------------------------------------------

## Development

This project was built using **Visual Studio** on Windows.

------------------------------------------------------------------------

## Authors

-   Ian Chua ([@peachismomo](https://github.com/peachismomo))\
-   Koh Yan Khang ([@yankhangSIT](https://github.com/yankhangSIT))\
-   Lin Zhao Zhi ([@B1ueeee](https://github.com/B1ueeee))
