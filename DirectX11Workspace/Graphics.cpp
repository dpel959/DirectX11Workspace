#include "pch.h"
#include "Graphics.h"

Graphics::Graphics(HWND hwnd)
{
	_hwnd = hwnd;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewPort();
}

Graphics::~Graphics()
{

}

// 이제 리소스를 이용하는 것이니, 이용 및 연동은 _deviceContext 를 이용하면 된다! 
// Device는 리소스 생성, 스왑 체인은 후면 버퍼와 관련이 있을 뿐이다.
void Graphics::RenderBegin()
{
	// OM은 마지막 단계, OutputMerge Stage를 뜻한다.
	// GPU가 그림을 모두 그리는 데 성공했으면, 이 도화지에 그림을 그려줘. 라고 하는 것.
	// 그리고 우리는 후면 버퍼인 렌더 타겟 뷰를 만들어 놨었다. 후면 버퍼에 그림을 그려달라고 요청한다.
	// 맨 앞 파라미터는 View의 수. 그 뒤에 타겟의 주소를 적어준다.
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);

	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor); // 이건 일정 색상으로 렌더 타겟 뷰를 밀어달라는 것. 지금은 초기화용도.

	_deviceContext->RSSetViewports(1, &_viewport); // viewport 화면 정보를 기입!
	// 아니, 래스터라이저가 왜 뷰포트가 필요해요? -> 래스터라이저는 투영 공간의 좌표를 -1.0f ~ 1.0f로 받는다.
	// 그래서 실제로 어느 픽셀에 어떻게 적용해야할지 좌표를 모름. 그래서 '실제 크기는 이래'라는 정보를 RS(래스터라이저)에게 주는 것.
}

void Graphics::RenderEnd()
{
	// 스왑 체인은 전면, 후면 버퍼 2개를 두고 있고, (우리가 후면 버퍼 하나라고 설정했으니)
	// 그리고 백 버퍼에서 다 그렸으면 프론트로 고속 복사로 그려주는 (=출력) 것이다.

	HRESULT hr = _swapChain->Present(1, 0); // 백 버퍼에 그림 다 그렸으니, 프론트 버퍼에 제출할게요.
	// 파라미터 1은 수직 동기화 여부, 2는 그냥 테스트나 티어링 허용 여부

	assert(SUCCEEDED(hr));
}

void Graphics::CreateDeviceAndSwapChain()
{
	// swap chain을 정의하는 struct
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc)); // ::memset으로 0으로 밀어주는 거임. C++은 스택의 지역변수는 쓰레기 값으로 차 있으니까.

	{
		// 당연히 버퍼 크기도 우리 화면 크기랑 같아야한다. 같은 규격의 정보를 담아야하니까
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.RefreshRate.Numerator = 60; // 화면 주사율
		desc.BufferDesc.RefreshRate.Denominator = 1; // 화면 주사율 분모 소수점 주사율을 지원하기 위해 있음.
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA 8 8 8 8 비트 칼라에, UNORM(Unsigned Normalized) = 0~255 정수를 셰이더(GPU)에서 알아서 0.f~1.f로 읽으라는 뜻
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1; // 삼각형을 그리다보면, 픽셀 단위가 아니라서 자글자글하게 그려지는, 계단 현상이 일어날 수 있다.
		// 그래서, 그것을 어떻게 처리할 것인가? 인데. 계단현상을 줄이는 Anti-aliasing이다. 한 픽셀을 4개로 쪼개, 그 4 픽셀을 사용한 보간 값을 이용하면 이런 계단 현상을 줄일 수 있다.
		// 이 Count는 그것에 관한 수치임. 그 샘플을 몇 개로 쪼개 보간할 것인가. 1은 안 쪼개고 보간 안 하겠다는 거임.
		// 당연히 보간이므로, RS가 담당한다.
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 이 버퍼 어떻게 쓸 거에요?? 라는 거
		// 최종 결과물을 그려주는 역할로 사용하겠다. 라고 말한 것
		desc.BufferCount = 1; // 후면 버퍼는 하나. 더 늘리면 더 다음 프레임도 미리 준비할 수 있다.
		desc.OutputWindow = _hwnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 이게 프론트 버퍼와 백 버퍼 플립하면서 안 하겠다는 거임. 그래서 백 버퍼 내용을 프론트 버퍼로 고속 복사하고 버리는 것.
		// 원래는 Flip 방식이 권장된다. DXGI_SWAP_EFFECT_FLIP_DISCARD하고, buffer count도 2개 이상으로 해주어야한다.
		// 아니, Flip은 그냥 프론트 버퍼, 백 버퍼 바꾸는 건데 왜 버퍼 개수가 바뀌어야 함? -> 그게 아니라, v 의미가 달라짐.
		// DXGI_SWAP_EFFECT_FLIP_DISCARD면 BufferCount는 전면 + 후면 버퍼의 총 갯수를 지칭하기 때문임.
	}
	// 자, 위 정의로 인해 우리는 백버퍼를 포함해 버퍼를 2개 가진 것이다. (더블 버퍼링 = 스왑 체인. 전면 버퍼 + 후면 버퍼들)


	// 이제 진짜 생성하는 함수
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE, // 드라이버 타입을 하드웨어로 하겠다. = 그래픽카드를 사용하겠다!
		// 만약 컴퓨터에 GPU가 없다면, CPU가 GPU 역할을 하게 하는 TYPE_WARP등, 여러가지가 있다.
		// GPU가 없다면, 이 TYPE을 바꿔 다시 해주기도 한다. if문으로 HARDWARE 타입 생성에 실패하면 else로 WARP로 시도한다.
		nullptr,
		0, //D3D_FEATURE_LEVEL 이게 배열인데, 우리가 '이런 DX 버전들을 지원해야해!를 명시 가능
		// 입력 안 하면 이 녀석이 지원할 수 있는 가장 최신 버전을 고름
		nullptr,
		0,
		D3D11_SDK_VERSION, // 매크로임. 세부 버전 뭔지 들어가 있음.
		// --- 여기까지가 Device 설정이고,
		//SwapChain Descripton
		&desc,
		_swapChain.GetAddressOf(),//_swapChain의 더블 포인터. 왜 굳이 더블 포인터냐면, 내용물인 '주소'를 새로 할당해서 넘겨주기 때문이다.
		//ComPtr에는 여러 지원 함수가 있는데, Get은 T*를 반환, GetAddressOf는 T**를 반환한다. 
		_device.GetAddressOf(),
		nullptr, // 그래픽카드 검사 후,최종 선택한 스펙 버전 1개를 받아올 변수의 주소 (feature level)
		_deviceContext.GetAddressOf()
	);

	// HRESULT는 성공/실패 정보가 담긴 정수형 상태 코드다. bool과 달리, 어떻게 성공했는지, 어떻게 실패했는지를 나눠서 정보를 가짐.
	// SUCCEEDED는 HRESULT가 성공 범위인지 검사하는 매크로. 왜 매크로를 쓰냐면, HRESULT는 성공이나 실패의 경우가 여러가지라서 그것을 구분하기 위해서이다. 
	// 최상위 비트가 0이면 성공, 1이라면 실패를 의미해서 HRESULT >= 0인지 아닌지 여부를 보는 것으로 한번에 판별이 가능하다.
	assert(SUCCEEDED(hr));

	// 이제 만들어진 백 버퍼에 그림을 그려달라고 해야한다. -> RTV 생성!
}
void Graphics::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	// ComPtr은 -> 가 오버로딩 되어있음. 스마트 포인터처럼 사용하면 됨
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	// _swapChain, 네 안에 만들어져 있는 백 버퍼를 나에게 반환해줘. 라는 함수

	assert(SUCCEEDED(hr));

	// RenderTargetView = 어떠한 버퍼에 렌더링을 할 수 있도록 도와주는 '안경'='해석기'이다.
	// View가 붙으면 어떠한 태그를 달아 이걸 나중에 GPU에게 친절하게 설명하기 위핸 부연설명.
	// Texture2D는 GPU의 VRAM에 올라가 있는 순수한 생 바이트 데이터이다. 텍스처는 그저 리소스일 뿐이니까. 
	// 하지만, 그 버퍼 주소를 들고 있는 렌더 타켓 뷰를 만들면, 렌더 타겟 뷰는 말 그대로 그 리소스를 GPU에게 '화면으로 그리는 도화지로 써라' 라고 알려주는 역할을 한다.
	// 이걸 GPU에게 '어떤 식으로 해석할 것인가?' 라고 알려주는 게 View이다.

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	// 이제 그러면 그 백 버퍼를 묘사하는 렌더 타겟 뷰를 만들어줘. 라고 device에게 요청. (새로운 리소스!)
	// GPU와 소통하는 용도로 필요함. 렌더 타겟 뷰는 GPU에게 명령하기 위해 필요한 특수한 포인터. 라고 봐도 된다

	assert(SUCCEEDED(hr));
}

void Graphics::SetViewPort()
{
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}
