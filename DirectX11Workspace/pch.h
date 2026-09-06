#pragma once

// 여러 번 사용되고 변경이 적은 표준 라이브러리만 넣자. 불필요한 헤더를 모든 파일에 넣으면 낭비이다.
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Windows
#define WIN32_LEAN_AND_MEAN // 무거운 윈도우 기능들 제외
#define NOMINMAX // std::min, std;:max 충돌 없애기
#include <windows.h>

// 여러 번 사용되고 변경이 적은 DirectX 기반 헤더
#include <d3d11.h>          // DirectX 11 핵심 기능 (ID3D11Device, Context 등)
#include <wrl/client.h>     // Microsoft::WRL::ComPtr 스마트 포인터
#include <DirectXMath.h>    // DirectX 전용 3D/2D 수학 라이브러리 (행렬, 벡터 연산)
#include "SimpleMath.h"    // 여러 Transform 관련 파일에서 공통으로 사용하는 수학 타입

// PCH는 컴파일 비용을 줄이는 캐시일 뿐, 각 헤더의 의존성을 대신하지 않는다.
// 프로젝트 클래스 헤더와 특정 기능 전용 헤더는 사용하는 파일에서 직접 include한다.
