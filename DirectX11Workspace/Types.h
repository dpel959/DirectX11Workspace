#pragma once
#include <windows.h>
#include "DirectXMath.h"
#include "SimpleMath.h"

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;

using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

//using Vec2 = DirectX::XMFLOAT2;
//using Vec3 = DirectX::XMFLOAT3;
//using Vec4 = DirectX::XMFLOAT4;
using Color = DirectX::XMFLOAT4;

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;

using Quaternion = DirectX::SimpleMath::Quaternion;

///
/// 이건 뭔데 XMFLOAT를 버리고 쓰나요?
/// -> SimpleMath는 XMFLOAT를 그대로 상속받은 타입이다. 뭐가 다르냐 하면 연산자를 지원한다. (+ 내적, 외적 등의 기능 등)
/// 이동, 복사 연산자 정도는 기본 지원하지만, 사칙연산같은 연산자는 지원하지 않는다. 그래서 그걸 지원하는 것이 SImpleMath의 Vector.
/// 
/// XMFLOAT를 쓴다고 하면, 꽤 복잡한 과정을 거쳐야한다.
/// 

#if 0 
 XMFLOAT3 a(1, 2, 3);
 XMFLOAT3 b(4, 5, 6);
 
 XMFLOAT3 c = a + b; // 불가능.
 
 // 따로 XMFLOAT를 XMVECTOR로 옮기는 과정이 필요함.
 // 이걸 왜 나누었나 하면..XMVECTOR은 사실 CPU의 SIMD 레지스터임. 물론, CPU의 SIMD는 내부에서 1,2,4,8 등 크기를 나누어 다룰 수는 있음. 
 // 하지만, 데이터의 주소 자체는 16/32/64바이트 단위로 받아와야함. SIMD 레지스터의 크기가 그 크기이기 때문임, (16/32/64바이트 자체가 레지스터 크기임. 분할되어있는 것이 아님.)
 // 참고로, 64바이트 SIMD는 캐시라인만큼의 데이터를 한번에 받아오기 용이하게 하기 위함도 있음.
 // 
 // 보통 CPU - RAM 간의 데이터 버스는 8바이트 단위임. 거리가 멀어서, 구리선의 문제도 있어서임. 그래서 '여러 번' 연속으로 쏴주는 것이 DDR 버스트이다.
 // 8번의 DDR 버스트 = 64바이트 전송 = 캐시라인 = CPU의 ZMM SIMD 레지스터. 이렇게 대역폭이 최적화될 수 있다.
 // 
 // 참고로, GPU는 각 코어에 VRAM이 거의 붙어있는 수준이라 데이터 버스가 256비트 이상도 전송할 수 있는 것. 
 // 
 // 아무튼, 그래서 12바이트 + 4바이트 더미 / 12바이트 + 4바이트 더미 이런 식은 가능함.
 // 12 + 12 이런 식으로도 가능은 함. 하지만 CPU의 SIMD는 내부적으로 4바이트 계산기의 연속임. 그러므로 12바이트+12바이트+12바이트 이런 식으로 데이터가 이어진다면
 // 만약 float3라고 치면, x0,y0,z0,x1..이렇게 되고, 4바이트에 x1까지 담기게 되고, 이건 다른 벡터의 정보이다. 그러므로 이러한 것을 '미는' 작업이 필요하게 된다.
 // 
 // 그래서, 그 밀고 마스킹하고 하는 작업을 안 하기 위해서, 애초에 '패딩을 해서 넣어주는 게' 밑 작업임.
 // 결국 XMFLOAT가 몇 개의 float를 담느냐에 따라 더미를 씌워서 SIMD에 잘 올리는게 Load라고 볼 수 있겠음.

 // GPU의 D3D11_BUFFER와는 다른 얘기임. 그건 GPU SIMD라서 진짜로 데이터 자체를 4 * 4 행렬 크기로밖에 못 다룸..
 // 하지만 이전 GPU와 다르게, 요즘 GPU는 4바이트 레지스터들로 이루어져 있음. 그런데 16바이트 데이터들을 고집하는 이유는,
 // 4 * 4 행렬이 데이터의 기본이 되는 것(1 * 4 벡터(동차 좌표, 색상) = 16바이트, 4 * 4 행렬 = 64바이트 = 캐시라인), 이전 GPU 레지스터의 하위 호환성을 위해서라는 이유가 있음.
 // 사실 GPU는 하드웨어적 이유로는 굳이 16바이트를 고집할 필요는 없음. 어차피 코어들은 4바이트 레지스터들을 가지므로(보통 float를 다루므로). 
 // 16바이트로 데이터를 다루는 이유는 그저 하위 호환성 때문이다.

 // CPU : 진짜 하드웨어적으로 벡터, 행렬 연산을 하려면 16바이트로 받는 게 이득이다. 캐시라인 관련 이득도 있다.
 // 한 스레드에 4바이트 연산기로 이루어진 SIMD 레지스터를 다룬다. 결국 한 스레드이므로 각 나눈 부분에 따라 다른 if-else를 적용하는 것을 불가능.
 // Q. 아니, 가능한데요? A. 그렇게 보여지게 가능은 하다. COND/Blend를 사용하면 됨. 그런데 '진짜로' if-else를 적용하는 것은 안 된다는 것이다.
 // GPU : 사실 하드웨어적으로 16바이트 단위로 받을 필요는 없다. 하위 호환성 때문에 그렇다. 스케줄러 하나가 Warp(32개 스레드/코어 묶음)에 일을 시킨다.
 // Warp에서는 다른 코어에, 다른 흐름을 가질 수 있으므로 각 다른 if, else 를 적용할 수 있다.

 XMVECTOR va = XMLoadFloat3(&a); // 함수로 SIMD에 올려주고...
 XMVECTOR vb = XMLoadFloat3(&b);
 XMVECTOR vc = XMVectorAdd(va, vb); // 함수로 연산을 해주고...
 XMFLOAT3 c;
 XMStoreFloat3(&c, vc); // 다시 메모리에 올려야 함.

#endif

// Matrix는 XMFLOAT4X4를 상속받았다. SimpleMath의 벡터와 XMFLOAT3과 같은 관계이다. 그냥 연산자만 잘 추가된 것.
// XMFLOAT4x4의 내부는 struct와 float[][]의 union으로 이루어져있다.
// union은 '모든 멤버가 완전히 똑같은 시작 주소를 공유'하는 것이다.
// 즉, 메모리를 64바이트로 낭비하지 않고도, _11, _12...(구조체 방식)으로도, m[4][4] 방식으로도 접근할 수 있다는 것.
// 왜냐? 같은 메모리 주소를 공유하고, struct도 float 16개로 이루어진 구조체고, m[4][4]도 float 16개 배열이니까.

// Matrix는 static 함수로 Translation, Scale, Rotation 행렬을 만들어주는 기능도 제공한다. (행렬 셈, 뺌, 곱은 물론이고.)
// 그런데 Rotation은 sin, cos 값을 넣어주기는 한데.. 사실 이걸 언제나 생으로 구하지는 않는다.
// 쓸 것 같은 sin, cos 값들을 구해놓은 다음에 '캐싱'해서 재활용하는 경우가 많다.

// 그런데 주의할 것은, SimpleMath는 '오른손 좌표계'이다. Rotation 식이 달라지는 건 열벡터, 행벡터의 차이라서 그건 상관 없지만,
 // Z축의 방향이 다르다. 왼손 좌표계는 +Z가 앞, 오른손은 -Z가 앞이다. 즉, 'Look 벡터가 다르다!'
 // X축을 잡고 90도 회전하면, 왼손은 '화면 안', 오른손은 '화면 밖'으로 이동한다. 회전 자체가 물리적으로 반대다!
 // 또한, 삼각형 앞뒷면 판정도 다르다. 왼손은 '시계 방향'으로 돌아야 앞면이지만, 오른손은 '반시계 방향'으로 돌아야 앞면이다.
 // 및, 당연히 x, y, z의 방향이 다르므로 외적의 결과도 다르다. x, y 축의 법선 벡터인 z축이 어디로 나올것이냐? 가 달라진다.
 // 또, 모델링은 기본적으로 오른손 좌표계라서, 변환해서 받아줘야한다.

 // Q. 근데 어떻게 회전 행렬이 같을 수가 있지? A. 회전해서 바뀌는 값 자체는 '수치적으로는' 같다. 하지만 그 회전해서 '실제로 어떻게 그려질지, 어디로 구를 것인지' 가 다를 뿐이지.