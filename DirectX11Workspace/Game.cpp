#include "pch.h"
#include "Game.h"
#include <climits>

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_width = GWinSizeX;
	_height = GWinSizeY;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewPort();

	CreateGeometry();

	CreateVS();
	CreateInputLayOut(); // 실행 단계와 다르게, 생성에서는 InputLayout은 VS보다 뒤에 만들어야한다. vsBlob이 필요해서...
	CreateConstantBuffer();

	CreateRasterizerState();
	CreateSamplerState();
	CreateBlendState();

	CreatePS();

	CreateSRV();
}

void Game::Update()
{
	_transformData.offset.x += 0.0003f;
	_transformData.offset.y += 0.0003f;

	D3D11_MAPPED_SUBRESOURCE subResource; 
	ZeroMemory(&subResource, sizeof(subResource));

	// CPU에서 GPU로 데이터를 복사할 때 SUBRESOURCE와 Map, Unmap을 쓴다.
	// CPU가 Map을 통해, subResource에 'GPU의 빈 메모리 주소'를 받는다.
	// 그 뒤, GPU가 _constantBuffer(GPU에게는 b 레지스터)을 모두 사용했다면, CPU가 쓴 값을 자신의 b 레지스터에 쓴다.
	
	// Resource가 버퍼, 텍스처 타입 등을 뜻한다면 서브리소스는 그 하위에 들어가는 값들을 뜻한다.
	// 근데 버퍼는 서브리소스가 단 하나임. 자신의 저장공간. 그것이 두번째 파라미터, 0번 인덱스라고 표기된 것.

	_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_deviceContext->Unmap(_constantBuffer.Get(), 0);

	// 참고로 Unmap을 제대로 안 하면, 나중에 GPU에 접근하려할때 GPU가 아직 수정 중이어서 아무도 다가가지 못해서
	// Draw()를 하려고 해도 에러가 난다.
}

void Game::Render()
{
	RenderBegin(); // 준비 작업. 도화지 초기화 등.

	// 실제 그리는 작업.
	// IA - VS - RS - PS - OM

	{
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		// IA

		// 어떤 데이터를 넣어줄거야?
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		// 그 버텍스들 어떤 순서로 읽을 거야?
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		// '정점 하나'에 들어간 데이터는 어떤 식으로 해석해?
		_deviceContext->IASetInputLayout(_inputLayout.Get());
		// 자 읽어주신 데이터는 삼각형으로 인식해주세요.
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS

		// GPU 네가 이 셰이더로 일했으면 좋겠어
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);

		// b0에 _constantBuffer의 Buffer 내용 등록!
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());

		// RS
		_deviceContext->RSSetState(_rasterizerState.Get());

		// PS

		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView[0].GetAddressOf());
		_deviceContext->PSSetShaderResources(1, 1, _shaderResourceView[1].GetAddressOf()); // 맨 앞은 셰이더 인덱스. t0, t1 붙인 거

		_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());

		// OM
		_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);

		//_deviceContext->Draw(_vertices.size(), 0); // 정점 몇 개인지 입력하고, 그려주세요
		_deviceContext->DrawIndexed(_indices.size(), 0, 0); // 인덱스를 참고해서 그린다!
	}

	RenderEnd(); // 다 그렸으니 제출. 이런 흐름
}

// 이제 리소스를 이용하는 것이니, 이용 및 연동은 _deviceContext 를 이용하면 된다! 
// Device는 리소스 생성, 스왑 체인은 후면 버퍼와 관련이 있을 뿐이다.
void Game::RenderBegin()
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

void Game::RenderEnd()
{
	// 스왑 체인은 전면, 후면 버퍼 2개를 두고 있고, (우리가 후면 버퍼 하나라고 설정했으니)
	// 그리고 백 버퍼에서 다 그렸으면 프론트로 고속 복사로 그려주는 (=출력) 것이다.

	HRESULT hr = _swapChain->Present(1, 0); // 백 버퍼에 그림 다 그렸으니, 프론트 버퍼에 제출할게요.
	// 파라미터 1은 수직 동기화 여부, 2는 그냥 테스트나 티어링 허용 여부

	assert(SUCCEEDED(hr));
}

void Game::CreateDeviceAndSwapChain()
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

	// HRESULT는 bool이다. 성공 실패 여부를 받아냄
	// SUCCEEDED는 매크로. hr이 true면 SUCCEEDED도 true. 아니면 false
	assert(SUCCEEDED(hr));

	// 이제 만들어진 백 버퍼에 그림을 그려달라고 해야한다. -> RTV 생성!
}

void Game::CreateRenderTargetView()
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

void Game::SetViewPort()
{
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}

void Game::CreateGeometry()
{
	// 버텍스들의 데이터! 이건 CPU에 저장하므로, 아직 CPU의 영역 = RAM에 저장됨.
	{
		_vertices.resize(4); // 인덱스 버퍼를 이용해 사각형으로 만들어보자!

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f);
		_vertices[0].uv = Vec2(0.f, 5.0f);
		//_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
		_vertices[1].uv = Vec2(0.f, 0.f);
		//_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].uv = Vec2(5.0f, 5.0f);
		//_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);

		_vertices[3].position = Vec3(0.5f, 0.5f, 0.f);
		_vertices[3].uv = Vec2(5.0f, 0.f);
		//_vertices[3].color = Color(0.5f, 0.5f, 0.5f, 1.f);
	}

	// 버텍스 버퍼
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc)); 
		{
			// 이거 중요!!! 'GPU만 읽을 수 있는' 데이터. 하지만 GPU도 CPU도 쓰지 못한다.
			// GPU에 버퍼가 만들어 질건데, 얘를 어떻게 만들고, 작업을 할 거임?
			// DEFAULT = GPU만 읽고 쓸 수 있음
			// DYNAMIC = GPU는 읽고, CPU는 쓸 수만 있다.
			// STAGING = GPU에서 CPU 로 데이터를 전송할때 사용된다.

			// 근데 정점 데이터를 굳이 바꿀 일이 없잖아? 그래서 IMMUTABLE
			desc.Usage = D3D11_USAGE_IMMUTABLE; 
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 어떻게 쓸 거에요? = 버텍스 버퍼로요 (내가 어떻게 읽을 지 알려줘.)
			// 텍스처의 해석기가 렌더 타겟 뷰라면, 버퍼의 해석기는 BindFlags임.
			desc.ByteWidth = static_cast<uint32>(sizeof(Vertex) * _vertices.size()); // 사이즈는?
		}

		// 그래서 실제로 CPU의 어떤 데이터를 넘겨줄건데? 

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data(); // == &_vertices[0]

		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf()); // CPU 데이터 data를 desc에 따라 _vertexBuffer로 옮겨줘
	
		assert(SUCCEEDED(hr));
	}

	// 인덱스
	{
		// 벡터를 채우는 것인데, 중요한 것이 있다. 이것은 '그리는(정점을 활용하는) 순서'이다.
		// 시계 방향, 반시계 방향 둘 중 하나를 일관적으로 사용해야한다.
		
		// 인덱스 버퍼는 사실 버텍스를 버텍스 버퍼에 넣은대로 순서대로 조립하지 말고, 제 순서대로 조립해주세요이다.
		// 그런데 왜 이게 좋냐고 하면, GPU의 캐시때문이다. 셰이더 연산을 하려고 했는데, 이미 이전에 한 것이면 캐시에서 결과값을 가져온다.
		// 그리는 순서 제시도 가능 + 캐시로 인해 자연스레 셰이더 연산을 줄일 수 있는 것이다.

		// 이 0,1,2 번호는 버텍스 버퍼에 넣은 정점의 순서를 의미하는 것이다.
		_indices = { 0,1,2,2,1,3 }; 
	}

	// 인덱스 버퍼
	{
		D3D11_BUFFER_DESC desc;
		::ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = static_cast<uint32>(sizeof(uint32) * _indices.size());

		D3D11_SUBRESOURCE_DATA data;
		::ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data();

		HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());

		assert(SUCCEEDED(hr));
	}
}

// 아직까지도 GPU 입장에서 버텍스 버퍼는 그냥 데이터 쪼가리임. 이걸로 뭘 하라고?를 알려줌
void Game::CreateInputLayOut()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// 밑에 써두는 POSITION, COLOR 같은 이름은 규약은 아니고, HLSL과 연동하기 위해 저장하는 이름
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 지금 넘길 버퍼에 float,float,float는 POSITION 데이터에요
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // !!중요!! 앞의 12바이트를 POSITION이 먹었으니, 저희 12바이트부터 시작해요
	};

	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// InputLayout = "GPU가 데이터를 어떻게 읽어야할지 알려주는 번역기"
	// _vsBlob을 왜 필요로 하냐면, ELEMENT_DESC와 struct VS_INPUT이 서로 일치하나 사전 검증 하는 것이다.
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
}

// 셰이더를 로드, blob에 저장해 넘겨줌
void Game::LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version, OUT ComPtr<ID3DBlob>& blob)
{
	// 디버그 모드고, 최적화 건너뛰겠다.
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	//d3dcompiler의 함수임. d3d11 기본 제공이 아님
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr
	);

	assert(SUCCEEDED(hr));
}

// 파일로 존재하던 셰이더를, 메모리에 들고 동작을 시킨다
// 딱 흐름을 정리하자면, InputLayout에서 ELEMENT_DESC 생성(C++) -> HLSL 짜기 -> C++로 돌아와 D3DCompileFromFile로 HLSL 로드
// -> Blob으로 셰이더 파일 로드, Blob을 통해 셰이더를 C++에서 생성 -> 다시 InputLayout으로 와 CreateInputLayout
void Game::CreateVS()
{
	// 파일에서 셰이더 정보를 blob으로 끌어온다!
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob); // 셰이더 이름, 셰이더 종류, 셰이더 버전, 받을 블롭

	// blob을 통해 버텍스 셰이더를 만든다! 굳이 blob을 거쳐서 만드는 이유는, 나중에 InputLayout을 만들때도 blob이 필요하기 때문. (검증용)
	HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());

	assert(SUCCEEDED(hr));
}

void Game::CreatePS()
{

	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob); // 셰이더 이름, 셰이더 종류, 셰이더 버전, 받을 블롭

	HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());

	assert(SUCCEEDED(hr));
}

void Game::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID; // SOLID : 그냥 있는 그대로 , WIREFRAME : 삼각형 (폴리곤) 단위로만 보여주겠다
	desc.CullMode = D3D11_CULL_BACK; // CULL_NONE, CULL_FRONT(앞에 있는 걸 자름)도 가능. Q. 근데 앞을 왜 잘라?
	desc.FrontCounterClockwise = false; // 근데 앞, 뒤를 어떻게 판별하는데요? -> '버텍스가 시계 방향으로 구성되면 앞이다' 라고 알려주는 것
	// 우리가 인덱스를 0, 1, 2, 2, 1, 3 으로 시계 방향으로 구성했던 것은 이것 때문. 만약 카메라가 봤을때 버텍스 순서가 반시계 방향이면 이 속성이 false면 잘리게 된다.
	// Q, 근데 이걸 z-depth로 안 하네...? 2D 때문인가?
	HRESULT hr = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());

	assert(SUCCEEDED(hr));
}

// 그래서, 이 Sampler가 하는 것이 무엇이냐? 텍스처의 UV좌표는 0.0f~1.0f이다.
// 그런데 넣어진 값이 '범위를 벗어나면, 그 값을 어떻게 반영할 것인가?'를 담당한다.
// 그것을 어떻게 담당할지를 'ADDRESS MODE'라고 한다.
void Game::CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));

	// BORDER: 밑에서 설정한 BorderColor로 채운다. 스나이퍼 조준경 밖 암전 처리
	// MIRROR: UV 좌표 값을 거울에 대칭시키듯 채운다. 
	// WRAP: UV 좌표 값을 반복 적용한다. 타일 등에 이용.
	// CLAMP: 가장자리 픽셀을 쭉 늘린다. UI 아이콘, 스카이박스 등에 이용
	// MIRROR_ONCE: 0,0을 기준으로 딱 한번만 거울 반사하고, 나머지는 CLAMP한다. 나비 같이 완벽한 좌우 대칭 등에 이용
	desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

	// BorderColor는 순서대로 RGBA이다.
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;

	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = (std::numeric_limits<float>::max)();
	desc.MinLOD = (std::numeric_limits<float>::min)();
	desc.MipLODBias = 0.0f;

	_device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
}

// 텍스처의 Alpha 값에 따라, 어떻게 섞여야 할 것인지
void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Src = 새로 만드는 픽셀에 Src의 알파값 만큼을 투명도를 곱하겠다
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // Dest = 원래 있었던 픽셀에 (1 - Src의 알파값) 만큼을 투명도에 곱하곘다.
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 그걸 더하겠다. 일반적인 알파 블렌딩. 
	//보통 이펙트에 사용하는 가산 블렌딩은 SrcBlend와 DestBlend를 둘 다 D3D11_BLEND_ONE = 모두 그대로 적용.으로 설정한다. 빛 유지 + 새로운 빛 발광 = 즉 이펙트 등에 많이 사용한다.
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
	assert(SUCCEEDED(hr));
}

// ShaderResourceView. 텍스처 사용에 필요함. 이미지를 받아오는 방법, 라이브러리는 여러가지 있지만, DirectXTex를 사용한다.
// RTV는 여기다 쓰세요 - OM 단계 쓰기 전용이고, SRV는 텍스처 가져왔으니 쓰세요 - PS 단계 읽기 전용 (자원)이다.
// 유명한 Render-to-texture라는 기술이 있는데, 처음 파이프라인에 RTV로 그리고, 다음 파이프라인에 SRV로 덮어씌워 본 렌더링 위에 덮는 미니맵/후처리 등을 쓰기도 한다.
void Game::CreateSRV()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;

	HRESULT hr = ::LoadFromWICFile(L"Skeleton.png", WIC_FLAGS_NONE, &md, img);

	assert(SUCCEEDED(hr));

	hr = CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView[0].GetAddressOf());

	assert(SUCCEEDED(hr));

	hr = ::LoadFromWICFile(L"Golem.jpg", WIC_FLAGS_NONE, &md, img);

	assert(SUCCEEDED(hr));

	hr = CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView[1].GetAddressOf());

	assert(SUCCEEDED(hr));
}

void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC; // 사실 이건 CPU의 수정을 허락하는 것은 아니다. GPU와 CPU가 둘 다 효과적으로 쓸 수 있는 곳(PCIe Write-Combined)에 메모리를 배치시키라는 것이다.
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU는 이 값을 고칠 수 있어야한다! (CPU에서 여러 값을 상수로서 보내주는 것이기에.)

	HRESULT hr = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}