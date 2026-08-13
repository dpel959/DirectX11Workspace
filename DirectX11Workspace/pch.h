#pragma once

#include "Types.h"
#include "Values.h"
#include "Struct.h"

//STL
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

//WIN
#include <windows.h>
#include <assert.h>

//DX
#include <d3d11.h>          // DirectX 11 핵심 기능 (ID3D11Device, Context 등)
#include <d3dcompiler.h>    // 셰이더(HLSL) 코드를 컴파일해 주는 도구
#include <wrl.h>            // Microsoft::WRL::ComPtr 스마트 포인터 사용 전용!
#include <DirectXMath.h>    // DirectX 전용 3D/2D 수학 라이브러리 (행렬, 벡터 연산)
#include <DirectXTex/DirectXTex.h> // PNG, JPG, DDS 같은 이미지 파일 불러오는 라이브러리
#include <DirectXTex/DirectXTex.inl> // C++ 소스 코드. 라이브러리는 아님. cpp인데 인라인이고, #include 안 하면 안 읽힘.
// 부록 cpp가 제일 맞는 말일듯.

using namespace DirectX;
using namespace Microsoft::WRL;

//프로젝트 설정이랑 별개로 이 책 꺼내 쓸 거야 임. lib는 연결하려면 이 난리 쳐야 함.
#pragma comment(lib, "d3d11.lib")        // DirectX 11 기본 라이브러리 연결. 
#pragma comment(lib, "d3dcompiler.lib")  // 셰이더 컴파일러 라이브러리 연결

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")  // 디버그 모드일 때는 디버깅 정보가 들어있는 DirectXTex_debug.lib 연결
#else 
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")     // 릴리스 모드일 때는 최적화된 DirectXTex.lib 연결
#endif

// OUT define
#define OUT