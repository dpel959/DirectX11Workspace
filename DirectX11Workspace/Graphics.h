#pragma once
class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics();

	void RenderBegin();
	void RenderEnd();

	const ComPtr<ID3D11Device>& GetDevice() const { return _device; }
	const ComPtr<ID3D11DeviceContext>& GetDeviceContext() const { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewPort();

private:
	HWND _hwnd;
	uint32 _width = GWinSizeX;
	uint32 _height = GWinSizeY;

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
};

