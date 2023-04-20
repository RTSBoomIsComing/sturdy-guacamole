# sturdy-guacamole
simple game engine based on win32-directx11-imgui-gltf using PBR

이 프로젝트를 통하여 그래픽스 라이브러리인 directx11 과 3D 파일 포맷인 gltf 가
잘 어우러지는지 시험하고자 합니다.

## 좌표계
오른손 좌표계를 사용합니다. gltf 가 오른손 좌표계를 사용하기 때문입니다.
directx 는 전통적으로 왼손 좌표계를 사용하지만 이 프로젝트에서는 gltf 와 잘 어우러질 수 있도록 오른손 좌표계를 사용합니다.

물론 3D 파일을 불러올 때 왼손좌표계로 변환할 수도 있습니다.
하지만 이 경우에 gltf 의 bin 파일에 저장되어있는 각종 데이터를 후처리 없이 그대로 그래픽스 파이프라인에 전달할 수 있다는 특장점을 살릴 수 없습니다.

'쉐이더에서 좌표계를 변환할 수 있지 않을까?' 라는 고민도 해보았습니다.
하지만 예를 들어,
애니메이션 구현에 사용하는 inverse bind pose matrix 는 좌표계 변환이 결코 쉽지 않습니다.

## 외부 라이브러리와 vcpkg
패키지 관리자 프로그램으로 vcpkg 를 사용합니다.
그리고 vcpkg 에 등록되어있는 오픈소스 라이브러리만 사용합니다.

vcpkg manifest 파일 (vcpkg.json) 을 프로젝트에 포함할지 고민해보았습니다.
하지만 오히려 더 번잡해지는 것같아 포함하지 않습니다.

사용할 외부 라이브러리는 다음과 같습니다.
1. directxtk:x64-windows
1. directxmath:x64-windows

1. directxtex:x64-windows
1. directxtex[openexr]:x64-windows

1. imgui:x64-windows
1. imgui[docking-experimental]:x64-windows
1. imgui[dx11-binding]:x64-windows
1. imgui[win32-binding]:x64-windows

## gltf importer
gltf 파일을 불러오는데 사용할 라이브러리는 아직 정하지 않았습니다.
이것 저것 써 본 후에 결정하도록 하겠습니다.

