// 셰이더 = GPU에게 '어떤 일을 해라'임.
// 사용 방법은 2가지. 1. 컴파일 하면 결과물이 COS 파일로 나옴. 그걸 바로 로드해서 사용
// 2. 프로그램을 시작하는 순간, 코드를 읽고, 컴파일해서 동적으로 바로 적용하는 것.
// 물론, 런타임마다 그 많은 셰이더를 컴파일하는 건 부담이니 성능상 이미 만들어진 것을 이용하는 1번이 좋긴 하다.

// 그런데, 일단 1번 방식으로 컴파일은 해야한다. 왜냐면 2번처럼 동적으로만 굴리면 컴파일 타임에 에러를 잡을 수가 없다. 그냥 로드 실패했어요~ 하고 끝나버림.

// 처음부터 InputLayout과 연동이 필요하다.
struct VS_INPUT
{
    float4 position : POSITION; // 우리가 InputLayout에 쓰기로 했던 이름!
    float4 color : COLOR;
};

// 여기는 C++과의 약속이 아닌, GPU의 래스터라이저와의 약속이다.
// 그래서 SV_POSITION 태그는 우리가 정해준 게 아닌, 원래 그렇게 하는 '규약'이다.
// SV_ : GPU 하드웨어가 강제하는 규약. 절대 못 바꿈. - SV_POSITION
// SV_가 없음 : 내가 이름을 마음대로 바꿀 수 있음. - COLOR
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// 함수임. 원리는 C++과 같음
// 지금 IA - VS - RS - PS - OM 에서 [IA-VS] 단계임.
// 단, 파라미터는 뭉탱이로 받지만, 이 함수 자체는 버텍스 단위로 실행된다. 거의 위치 관련을 담당한다.
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    output.color = input.color;
    
    return output;
}

// 이후, RS 단계로 넘어감. RS 단계에서 하는 일을 대강만 설명하자면,
// 1. 투영 공간에서 받은 -1.0f~1.0f를 뷰포트 크기로 확장. 2. 컬링 -> 3. 삼각형 안의 픽셀 찾기 
//-> 4. 속성 값 보간. 정점의 색상/UV/노멀, 그리고 픽셀이 각 정점에서 얼마나 떨어져있는지 가중치를 계산해 결과를 반환한다. (PS_INPUT)

// 이후, PS 로 넘어가서 픽셀 단위로 함수를 실행해 줌. 거의 색상 관련을 담당한다.

// '내가 지금 반환하는 색상을 SV_Target에 칠해라!' 임. 엥? SV_Target 연결을 저희는 안 해줬는데요?
// SV_Target은 C++에서 OMSetRenderTartargets()로 연결해주었다. 사실 그게, SV_Target0으로 렌더 타겟 뷰를 설정한다.
// 그리고 SV_Target은 SV_Target0을 뜻한다.

// 이 VS_OUTPUT으로 넘어올때, RS로 인해 값들이 가중치에 따라 position, color 가 보정되어 넘어옴.

// 게임 엔진에서 Material이 이런 것을 이용해 동작한다.
float4 PS(VS_OUTPUT input) : SV_Target
{  
    return input.color;
}