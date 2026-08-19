#pragma once
#include <string>

class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	void RenderBegin();
	void RenderEnd();

	//--- 디바이스 만들고, 화면 띄우기
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewPort();

	//--- 삼각형 만들기
	void CreateGeometry();
	void CreateInputLayOut();

	void LoadShaderFromFile(const std::wstring& path, const std::string& name
	,const std::string&version, OUT ComPtr<ID3DBlob>& blob);

	void CreateVS();
	void CreatePS();

	void CreateSRV(); 

	void CreateConstantBuffer();
	
private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:
	// Device & SwapChain

	// 스마트 포인터 같이 동작하도록하는, Wrapper 클래스 ComPtr.
	// (이거 안 하면 수동으로 ref 카운트 늘려주고 줄여주고 해야함..)

	// 스마트 포인터여서 nullptr을 하지 않아도 되긴 된다
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;
	//엥? 스왑 체인은 D3D11이 아니라 DXGI네요?
	//DXGI는 '그래픽의 일부'가 '다른 부분보다 더 느리게 진화한다' 는 것에 착안해,
	//DX 런타임과 독립해서 하위 수준 작업을 관리하는 놈. 인데 그냥 더블 버퍼링 관리하는 놈이다

	// 이걸 굳이 왜 나눠요? 하면 '그래픽'을 담당하는 렌더링 부분은 정말 빨리 발전하고 바뀌는데
	// 모니터에 출력하는 건 그에 비해 많이 느리게, 느릿하게 발전하기 때문이다. 그래서 천천히 변하는 모니터 출력 관련을 따로 나눠놓은 것.

	// RTV

	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	// 뭔가 그려달라고 할때 '뷰포트'가 필요하다. 화면을 묘사하는 구조체. 그냥 화면 껍데기임
	D3D11_VIEWPORT _viewport = { 0 };
	float _clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

private:
	// Geometry. 도형 = Mesh 만들기
	std::vector<Vertex> _vertices; 	// 이거 만드는 건 CPU의 영역이다. RAM에 저장됨.
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr; // 이제 GPU의 VRAM으로 넘겨주는 거임.
	std::vector<uint32> _indices; // 인덱스 버퍼에 넣을 인덱스 순서
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr; // 인덱스 버퍼.얼마나 정점을 많이 이용하는 도형을 만들 것이냐에 따라 정해주면 된다.
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// VS 
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr; // 셰이더 로드, 저장

	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView[2] = { nullptr, nullptr };

	// constant buffer
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;
};

