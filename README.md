# OpenGL Portal Demo

A real-time demo of stencil-based framebuffer portal rendering using recursive portals, implemented in modern OpenGL with support for volumetric and flat portals, recursive rendering, and view-dependent clipping.

---

##  Running the Demo

Clone this repo and make sure the required dependencies are available.

### Dependencies

You'll need the following submodules:

```bash
git clone https://github.com/ocornut/imgui.git external/imgui
git clone https://github.com/nothings/stb.git external/stb
git clone https://github.com/assimp/assimp.git external/assimp
```

Then configure and build using CMake (tested on Linux with CMake ≥ 3.10 and a C++17-compatible compiler):

```bash
cmake .
make
bin/GL_Portal
```

---

## Features Implemented

* ✅ Recursive portal rendering (up to N levels)
* ✅ Volumetric portals with full 3D geometry
* ✅ Oblique near-plane clipping for correct view occlusion
* ✅ Dynamic camera with WASD + mouse controls
* ✅ Skyboxes, textured environments, and moving objects
* ✅ Falling animated objects for stress-testing scene traversal
* ✅ Bidirectional and asymmetric portal links

---

## Attribution & References

* **Camera and movement**: Based on [learnopengl.com](https://learnopengl.com/Getting-started/Camera) camera code, with modifications for portal-relative transforms.
* **App / Window / Shader / GLShape and Renderable / Main structure**: Inspired by class template code from programming assignments (HW1–HW2).
* **Oblique clipping plane algorithm**: Inspired by Eric Lengyel’s article *"Oblique View Frustum Depth Projection and Clipping"*, also found in *Game Programming Gems 5*, Section 2.6.
* **3D models**: `teapot.obj` and `suzanne.obj` from standard public domain 3D resources.

