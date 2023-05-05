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
![image](https://user-images.githubusercontent.com/61501369/236203651-9c8a622e-bf42-4ece-848a-16e45983ae1e.png)

## Done
- Load from glTF and draw 3d models.
- Draw normal vectors of model. (On/Off)
- Show loaded images.
- Apply texturing, normal mapping, occlusion mapping, emissive mapping.
- Implement physically based rendering

## TODO
1. implement cubemap and IBL(image based lighting)
1. implement shadowing(depth map, shadow map)
1. implement animation, skinning, morphing
1. implement object picking from screen using mouse(stencil buffer or ray casting)

