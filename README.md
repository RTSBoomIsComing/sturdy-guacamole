# sturdy-guacamole
simple game engine based on win32-directx11-imgui-glTF using PBR

I want to test if DirectX 11, a graphics library, and the 3D file format glTF work well together through this project.

## Coordinate system
We are using a right-handed coordinate system because glTF uses a right-handed coordinate system.

## 외부 라이브러리와 vcpkg
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
![first demo simple triangle](https://user-images.githubusercontent.com/61501369/233636451-45f75f5b-5d4b-4bd8-8313-616f457c7f6e.png)
