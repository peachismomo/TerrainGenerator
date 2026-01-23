# Procedural Terrain Generation System

![Terrain overview](docs/high-oct.png)

A real-time terrain generation system built with **C++20** and **OpenGL 4.5**, developed as part of the **CSD3183 – Artificial Intelligence for Games** curriculum.  
This project explores how classical algorithms—**Poisson Disk Sampling**, **Delaunay Triangulation**, and **Perlin Noise**—can be combined to generate expansive, natural-looking 3D landscapes in real time.

The focus is on controllable procedural content generation: producing terrains that are deterministic, repeatable, and infinitely variable, while remaining efficient enough for interactive applications such as games and simulations.

---

## Implementation Details

This terrain generator follows a simple pipeline:

1) **Sample points (Poisson Disk Sampling)**  
2) **Triangulate the points (Delaunay Triangulation)**  
3) **Build a renderable mesh (vertices + indices)**  
4) **Apply height (Perlin Noise)**  
5) **Recalculate normals + render (OpenGL)**

---

### 1) Domain Setup

The terrain is generated over a 2D rectangular domain (XZ plane), e.g. **200 × 200** units:

- `x ∈ [0, width]`
- `z ∈ [0, depth]`
- `y` is height (computed later)

Inputs typically include:
- `width`, `depth` (e.g. 200, 200)
- `pointCount` (target number of points)
- `seed` (for deterministic generation)
- noise parameters (frequency, amplitude, octaves, etc.)

---

### 2) Even Point Distribution (Poisson Disk Sampling)

To avoid clumping (common with purely random points), Poisson Disk Sampling is used to generate points that are:

- **Evenly spaced**
- **Natural-looking**
- **Stable for triangulation**

Given a target `pointCount`, the sampler fills the domain with points while enforcing a minimum distance between points (often derived from the desired density). This produces a set of 2D sample points:

- `P = { (x0, z0), (x1, z1), ... }`

These points represent the terrain’s “control vertices” before height is applied.

---

### 3) Mesh Topology (Delaunay Triangulation)

Once points are sampled, Delaunay triangulation is performed to produce a triangle mesh with good properties:

- avoids long skinny triangles where possible
- robust connectivity across the domain
- clean topology for large-scale terrain

The output is typically:
- a list of triangle indices referencing the sampled points  
  `T = { (i0, i1, i2), (i3, i4, i5), ... }`

---

### 4) Mesh Construction (Vertices + Indices)

The triangulation output is converted into a GPU-friendly mesh:

**Vertices**
- position: `(x, y, z)`
- normal: computed after height is applied
- (optional) UVs: derived from `(x/width, z/depth)` or a chosen tiling method

**Indices**
- triangle index buffer built directly from the triangulation triangles

At this stage, the mesh is still flat (all `y = 0`).

---

### 5) Height Variation (Perlin Noise)

To produce terrain features (mountains, valleys, slopes), Perlin noise is sampled per vertex:

For each vertex `(x, z)`:
- `height = Perlin(x * frequency, z * frequency) * amplitude`
- `y = height`

Optionally:
- multi-octave noise (fractal / FBM) for richer detail
- domain warping or ridge noise depending on desired style

Because the entire system is seed-driven, changing the seed produces an entirely new terrain while remaining deterministic.

---

### 6) Normals + Coloring + Rendering (OpenGL)

After height displacement:

- **Vertex normals are recomputed** from triangle faces for correct lighting.
  - Face normals are accumulated into each vertex normal, then normalized to produce smooth shading.

- **Vertex coloring is generated from height (Y)** to simulate landmass.
  - First, compute the terrain’s `minY` and `maxY`.
  - Each vertex height is normalized:  
    `t = clamp((y - minY) / (maxY - minY), 0, 1)`
  - Colors are assigned by height bands / interpolation:
    - **Below 0** → **Blue** (water)
    - **Near 0** → **Beige** (sand / shoreline)
    - **Mid-range** → **Green** (grassland)
    - **Peaks** → **White** (snow / mountain tops)
  - The final color is produced by interpolating between these key colors, creating smooth transitions across the terrain (e.g., beige → green → white for land elevations).

- **Mesh data is uploaded to the GPU** (VBO/IBO/VAO)
  - Vertex attributes typically include position, normal, and vertex color (and optional UVs).

- **Rendering**
  - The mesh is rendered using an OpenGL 4.5 pipeline with lighting/shading.
  - Vertex colors provide the base “biome” look, while lighting (using normals) provides depth and realism.

---

### Summary

Given a domain (e.g. **200×200**) and a target point count:

- **Poisson Disk Sampling** creates evenly spaced points  
- **Delaunay Triangulation** converts points into stable triangles  
- A **mesh** is built from vertices + indices  
- **Perlin noise** displaces vertex height to form natural terrain  
- Normals are recalculated and the result is rendered in real time

---

## Development

This project was build using visual studio.

Refer to the source code for current integration details.

---

## Authors

- Ian Chua ([@peachismomo](https://github.com/peachismomo))
- Koh Yan Khang ([@yankhangSIT](https://github.com/yankhangSIT))
- Lin Zhao Zhi ([@<github-username>](https://github.com/<github-username>))