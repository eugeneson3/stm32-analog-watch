# STM32 아날로그 시계·스톱워치

RTC 시각과 스톱워치 값을 LCD·FND에 표시하고, 스텝모터 3개로 시침·분침·초침 위치를 제어하였습니다.

- 수행 시기 2026.10
- 참여 형태 개인
- 담당 범위 시각 데이터 연동 및 표시·스텝모터 Application 구성

[포트폴리오 상세 페이지](https://eugeneson3.github.io/projects/stm32-analog-watch/)

## 프로젝트 목표

디지털 시각 데이터와 물리적인 시계 바늘을 연결하여 현재 시각 및 스톱워치 시간을 표시하는 것을 목표로 하였습니다.

## 설계 및 구현

- DS3231 레지스터를 I2C로 읽고 BCD·십진수 변환을 적용하였습니다.
- FND의 자리 선택을 주기적으로 변경하고 LCD에 시각 정보를 표시하였습니다.
- 시·분·초 값을 모터 회전 스텝으로 변환하고 각 바늘의 현재 위치와 목표 위치를 관리하였습니다.
- Application을 ClockHands·Display·StopWatch·TimeClock 모듈로 구성하였습니다.

## Troubleshooting

### 모터 구동 주기 및 초기 위치 검토

모터가 목표 위치로 이동하는 주기와 전원 인가 시 기준 위치를 정의해야 합니다.

현재 소스는 전원 인가 시 세 바늘이 12시 방향에 있다고 가정하며, 구동 주기가 너무 짧으면 탈조 가능성이 있습니다.

ClockHands 모듈에서 2ms 스텝 주기와 현재·목표 위치를 관리하도록 구현하였습니다.

초기 바늘 정렬은 실행 전 확인 항목으로 기록하였습니다. 발표 자료에는 별도의 정량 오차 측정 결과가 없습니다.

## 검증 및 결과

- LCD·FND 시각 출력과 아날로그 바늘 동작을 시연 영상으로 기록하였습니다.

## 소스코드 구조

소스 스냅샷과 발표 자료를 기준으로 작성하였습니다. 파일별 원본 경로와 SHA-256은 SOURCE_MANIFEST.json에 기록하였습니다.

| 경로 | 내용 |
| --- | --- |
| [Core/Src/ap/ClockHands/](Core/Src/ap/ClockHands/) | 모터 3개 목표 위치 및 이동 제어 |
| [Core/Src/ap/TimeClock/](Core/Src/ap/TimeClock/) | 현재 시각 관리 |
| [Core/Src/ap/StopWatch/](Core/Src/ap/StopWatch/) | 스톱워치 관리 |
| [Core/Src/ap/Display/](Core/Src/ap/Display/) | LCD·FND 출력 |
| [Core/Src/driver/](Core/Src/driver/) | RTC·표시 장치·모터 드라이버 |

## 실행 방법

1. STM32CubeIDE에서 저장소 루트를 Import하고 .ioc의 핀 및 타이머 구성을 확인합니다.
2. 보드 다운로드 전 바늘의 초기 위치와 모터 배선을 확인합니다.

## 동작 동영상

- [아날로그 시계·스톱워치 동작](https://www.youtube.com/watch?v=htJ-XKtOes8)

## 참고 자료

- [발표 자료](https://docs.google.com/presentation/d/1VLAkIOdrnwaiuhC7mJxGrKz0auzhg2lnHmrIdwkciWk/edit)

## 소스 이력

2026년 11월 18일 로컬 프로젝트에서 구성한 공개 스냅샷입니다. 원본 파일의 상대 경로와 SHA-256은 [SOURCE_MANIFEST.json](SOURCE_MANIFEST.json)에 기록하였습니다. 빌드 산출물과 로컬 개발 환경은 제외하였습니다. 파일별 저작권 및 라이선스 고지를 유지하였습니다.
