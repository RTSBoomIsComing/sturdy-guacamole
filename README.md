# sturdy-guacamole
simple game engine based on win32-directx11-imgui-glTF using PBR

I want to test if DirectX 11, a graphics library, and the 3D file format glTF work well together through this project.

## Coordinate system
We are using a right-handed coordinate system because glTF uses a right-handed coordinate system.

## external libraries and vcpkg
We are using vcpkg as our package manager program.

I considered including the vcpkg manifest file (vcpkg.json) in the project, but it seems to make things more complicated instead. 
Therefore, we will not include it.

The external libraries we will be using are as follows:
1. directxtk:x64-windows
1. directxmath:x64-windows

1. directxtex:x64-windows
1. directxtex[openexr]:x64-windows

1. imgui:x64-windows
1. imgui[docking-experimental]:x64-windows
1. imgui[dx11-binding]:x64-windows
1. imgui[win32-binding]:x64-windows

1. tinygltf:x64-windows

## gltf loader
We are currently using an open-source library, tinygltf as our glTF loader.

## result so far
![result 2023-04-30 231713](https://user-images.githubusercontent.com/61501369/235360957-55e77a47-7116-42f1-948e-abf57b0f920e.png)

Now we can load from glTF and draw 3d models.
The depth stencil buffer applied.
POSITION, NORMAL, TEXCOORD attributes are dealt.
But more features are needed to visualize and test those.

POSITION range of above image is [0, 1]. 
Vertices are so close each other that depth buffer seem to be working wrong. 
Therefore the feature scailing models is needed. (this feature is related to animation)

## TODO
1. implement direct lighting. e.g. PBR, Material, Light
1. implement cubemap and IBL(image based lighting)
1. implement shadowing(depth map, shadow map)
1. implement animation, skinning, morphing
1. implement object picking from screen using mouse(stencil buffer or ray casting)

