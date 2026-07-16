## Large Thin Wrapper
A thin OpenGL core-to-OpenGL ES wrapper, primarily intended for running Minecraft.

# Building
`./gradlew :ltw:assembleRelease`

After completion, an AAR with native libraries will be available in `ltw/build/outputs/aar/ltw-release.aar`

# OpenGL requirements

## Minecraft: Java Edition

The latest release of Minecraft: Java Edition (1.21.x) is built on the Blaze3D renderer
on top of LWJGL 3.3.x. Mojang's officially posted minimum remains **OpenGL 4.0**
(in effect since 2017); however, since release 1.21.6 the game relies on **OpenGL 4.4**-class
features in practice and an OpenGL 4.4-capable GPU is required for stable operation. A GPU
exposing **OpenGL 4.5** is recommended.

Rather than depending on a single version number, Blaze3D queries for individual extensions
and adapts its code paths at runtime. The desktop features it depends on are:

| Feature | Core in | Used for | Exposed by LTW as |
|---|---|---|---|
| Persistent mapped buffers (`glBufferStorage`) | OpenGL 4.4 | Upload/streaming vertex & index buffers | `GL_ARB_buffer_storage` |
| GPU timer queries (`glQueryCounter`) | OpenGL 3.3 | GPU usage counter (Blaze3D `TimerQuery`) | `GL_ARB_timer_query` |
| Base-vertex drawing (`glDrawElementsBaseVertex`) | OpenGL 3.2 | Indexed draws with a vertex offset | `GL_ARB_draw_elements_base_vertex` |
| Per-draw-buffer blending | OpenGL 4.0 | Independent blend per render target (required by Iris) | `GL_ARB_draw_buffers_blend` |
| Texture buffers (`glTexBuffer`) | OpenGL 3.1 | Bind a buffer object to a sampler | `GL_ARB_texture_buffer_object` |

## What LTW exposes

LTW translates these desktop OpenGL calls down to OpenGL ES. To the host application it
reports an **OpenGL 3.3** core profile (`GL_VERSION`) together with a **GLSL 4.60** shading
language version, layered with the ARB extensions listed above. This 3.3 + curated-ARB set
is what satisfies the feature checks Minecraft performs at startup.

Reported strings:

- `GL_VERSION` → `3.3 OpenLTW (Built on: <date>/<time>)`
- `GL_SHADING_LANGUAGE_VERSION` → `4.60 LTW`
- `GL_VENDOR` → `artDev, SerpentSpirale, CADIndie`

Exposed ARB extensions (see `build_extension_string()` in [egl.c](ltw/src/main/tinywrapper/egl.c)):

- `GL_ARB_buffer_storage` — persistent mapped buffers; backed by `GL_EXT_buffer_storage`.
- `GL_ARB_texture_buffer_object` — texture buffers; backed by `GL_EXT_texture_buffer` or OpenGL ES 3.2.
- `GL_ARB_draw_elements_base_vertex` — base-vertex indexed drawing.
- `GL_ARB_draw_buffers_blend` — per-target blend state; required by Iris. Needs OpenGL ES 3.2 or `GL_OES`/`GL_EXT_draw_buffers_indexed`.
- `GL_ARB_timer_query` — GPU timer queries used by Minecraft's GPU usage counter.

## OpenGL ES device requirements

LTW requires a device exposing at least **OpenGL ES 3.0** with **ESSL 3.00**. OpenGL ES 3.1
and 3.2 enable additional fast paths. If the device reports ES < 3.0 or ESSL < 3.00, LTW
prints a warning, as this configuration is unsupported and will cause problems.

The following device-side ES extensions are detected and used when present:

| OpenGL ES extension | Backs | Notes |
|---|---|---|
| `GL_EXT_buffer_storage` | `GL_ARB_buffer_storage` | Persistent mapped buffers |
| `GL_EXT_texture_buffer` | `GL_ARB_texture_buffer_object` | Texture buffers on ES 3.0/3.1 |
| `GL_EXT_multi_draw_indirect` | multi-draw-indirect dispatch | Indirect drawing |
| `GL_EXT_disjoint_timer_query` | `GL_ARB_timer_query` | Accurate GPU timestamps |
| `GL_OES_draw_elements_base_vertex` / `GL_EXT_draw_elements_base_vertex` | `GL_ARB_draw_elements_base_vertex` | Base-vertex on ES 3.0/3.1 |
| `GL_OES_draw_buffers_indexed` / `GL_EXT_draw_buffers_indexed` | `GL_ARB_draw_buffers_blend` | Per-target blending on ES 3.0/3.1 |

The GLSL shader converter additionally injects `GL_EXT_texture_cube_map_array`,
`GL_EXT_texture_buffer`, `GL_OES_texture_storage_multisample_2d_array` and
`GL_EXT_shader_non_constant_global_initializers` when translating desktop GLSL to ESSL.

## Environment variables

| Variable | Default | Effect |
|---|---|---|
| `LTW_HIDE_BUFFER_STORAGE` | unset | When set to `1`, hides `GL_ARB_buffer_storage` from the application. |
| `LTW_ENABLE_TIMER_QUERY` | unset | When set to `1`, force-enables timer queries even without `GL_EXT_disjoint_timer_query` (faked queries, see `query.c`). |
