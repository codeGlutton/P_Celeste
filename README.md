# :computer: P_Celeste
<div align="center">
<img src="https://github.com/user-attachments/assets/36694f7e-8df7-483f-b58c-d58171c7d235"/>
</div>

This project is a 2D platformer developed using DirectX 11, created as a portfolio project that recreates Celeste.
It focuses on recreating the core mechanics and gameplay UX of the original game.

이 프로젝트는 DirectX 11을 사용하여 개발한 2D 플랫포머 게임으로, Celeste를 모작한 포트폴리오 목적의 프로젝트입니다.
게임의 핵심 메커니즘과 조작감을 동일하게 구현하는 데 중점을 두고 제작되었습니다.

## :pushpin: Main Architecture (핵심 아키텍처)
<div align="center">
<img src="https://github.com/user-attachments/assets/eeb30675-8f96-4910-b4d7-c1dff6bde829"/>
</div>
<div align="center">
<img src="https://github.com/user-attachments/assets/14500275-11b9-49a4-8990-fb4fb2d5cd21"/>
</div>

In a structure where multiple stages exist within a single world, keeping all stage data loaded in memory at all times led to performance degradation and unnecessary memory usage.
To address this, a streaming system was designed and implemented to dynamically load and unload only the 9 stages surrounding the player.

하나의 World 내에 다수의 Stage가 존재하는 구조에서, 모든 Stage 데이터를 실시간으로 메모리에 상주시킬 경우 성능 저하와 메모리 낭비가 발생하는 문제가 있었습니다.
이를 해결하기 위해 플레이어를 중심으로 한 인접 9개 Stage만을 동적으로 로드/언로드하는 Streaming 시스템을 설계 및 적용하였습니다.

* 플레이어 위치를 기준으로 현재 Stage와 주변 8개 Stage를 포함한 3×3 영역만 활성화
* 해당 영역을 벗어난 Stage는 즉시 메모리에서 해제하여 리소스 사용 최소화
* 상시 유지가 필요한 Stage 런타임 데이터는 별도 객체로 유지
* 결과적으로 메모리 사용량 감소 및 안정적인 프레임 유지 달성

## :wrench: Tools & Technologies (사용한 기술)
- **Client** : DirectX 11
- **Lib** : DirectXTex, FMOD
