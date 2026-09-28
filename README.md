# RayTracingStudy

<p align="center">
  <img src="Images/Phase1_Final.png" alt="Phase 1 최종 렌더" width="800">
</p>
<p align="center"><sub>Phase 1 최종 렌더 · 1200 × 675 · 픽셀당 500 샘플 · 최대 반사 깊이 50</sub></p>

[Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 C++로 한 챕터씩 따라 구현하며 정리한 레이 트레이싱 스터디 저장소입니다.
외부 라이브러리 없이 표준 라이브러리만 사용하고, 렌더 결과는 PPM 이미지로 출력합니다.

- 작성자: 이종진
- 환경: Windows 11 (x64) · Visual Studio 2022 · CUDA Toolkit 13.4

## 빌드 및 실행

**요구 사항**

- Windows (x64), Visual Studio 2022 (MSVC v143)
- [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads) 13.4 — 프로젝트가 CUDA 13.4 빌드 사용자 지정을 사용합니다.
  다른 버전을 설치했다면 `Phase1.vcxproj`의 `CUDA 13.4.props` / `CUDA 13.4.targets`를 설치한 버전에 맞게 바꿔야 합니다.

**실행 순서**

1. `Phase1/Phase1.sln`을 Visual Studio 2022로 엽니다.
2. 구성을 **Release | x64**로 두고 빌드합니다. 최종 장면은 픽셀당 500 샘플이라 Debug 빌드로는 오래 걸립니다.
3. 이미지는 표준 출력으로, 진행 상황(`Scanlines remaining`)은 표준 에러로 나오므로 결과를 파일로 리다이렉션합니다.

```bat
cd Phase1\x64\Release
Phase1.exe > final.ppm
```

> [!NOTE]
> Windows PowerShell 5.1의 `>`는 파일을 UTF-16으로 저장하기 때문에 PPM이 깨집니다.
> 명령 프롬프트(cmd)에서 실행하거나, PowerShell에서는 `cmd /c "Phase1.exe > final.ppm"`을 사용하세요.

## 트러블슈팅

CUDA 프로젝트를 처음 설정하면서 겪은 문제와 해결 방법은 [트러블슈팅.md](트러블슈팅.md)에 정리했습니다.

- `warning C4819` (CUDA 헤더의 코드 페이지 경고) → `-Xcompiler="/utf-8"` 옵션 추가
- `the provided PTX was compiled with an unsupported toolchain.` → GPU에 맞게 `Code Generation` 설정
  (현재 프로젝트는 `compute_120,sm_120` — RTX 50 시리즈 기준)

## 발표 자료

[RayTracingByOnWeekends_Redesign.pdf](발표자료/RayTracingByOnWeekends_Redesign.pdf) — 02장부터 14장까지의 내용을 정리한 스터디 발표 슬라이드

## 참고 자료

- Peter Shirley, Trevor David Black, Steve Hollasch, [*Ray Tracing in One Weekend*](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
