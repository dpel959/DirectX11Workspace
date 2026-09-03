#pragma once

// ConstantBuffer은 VertexShader에 쓰일때가 많지만, 꼭 그런 것은 아니다. CB도 PS에서 쓸 때가 있고, 텍스처도 VS에서 쓸 때가 있다.

// 왜 template? -> CB의 Data로 무엇이 다루어질지 모르기 때문이다. TransformData를 다룰 수도 있지만, 다른 것을 다룰 수도 있다.
// CB의 본질은 '상수 버퍼'이다. TarsnformData 만을 담는 것이 아니다.

template<typename T>
class ConstantBuffer
{
public:
	ConstantBuffer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
		: _device(device)
		, _deviceContext(deviceContext)
	{

	}
	
	~ConstantBuffer() {};

	const ComPtr<ID3D11Buffer>& GetComPtr() const { return _constantBuffer; }

	void Create()
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC; // 사실 이건 CPU의 수정을 허락하는 것은 아니다. GPU와 CPU가 둘 다 효과적으로 쓸 수 있는 곳(PCIe Write-Combined)에 메모리를 배치시키라는 것이다.
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Constant Buffer로 사용할 것이다!
		desc.ByteWidth = sizeof(T);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU는 이 값을 고칠 수 있어야한다! (CPU에서 여러 값을 상수로서 보내주는 것이기에.)

		HRESULT hr = _device->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	// Map -> UnMap을 모든 타입을 다룰 수 있도록 다룬다.
	void CopyData(const T& data)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		ZeroMemory(&subResource, sizeof(subResource));

		// CPU에서 GPU로 데이터를 복사할 때 SUBRESOURCE와 Map, Unmap을 쓴다.
		// CPU가 Map을 통해, subResource에 'GPU의 빈 메모리 주소'를 받는다.
		// 그 뒤, GPU가 _constantBuffer(GPU에게는 b 레지스터)을 모두 사용했다면, CPU가 쓴 값을 자신의 b 레지스터에 쓴다.

		// Resource가 버퍼, 텍스처 타입 등을 뜻한다면 서브리소스는 그 하위에 들어가는 값들을 뜻한다.
		// 근데 버퍼는 서브리소스가 단 하나임. 자신의 저장공간. 그것이 두번째 파라미터, 0번 인덱스라고 표기된 것.

		_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		::memcpy(subResource.pData, &data, sizeof(data));
		_deviceContext->Unmap(_constantBuffer.Get(), 0);

		// 참고로 Unmap을 제대로 안 하면, 나중에 GPU에 접근하려할때 GPU가 아직 수정 중이어서 아무도 다가가지 못해서
		// Draw()를 하려고 해도 에러가 난다.
	}

private:

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext; //  SubResource를 다루어주기 위해 필요하다.
	ComPtr<ID3D11Buffer> _constantBuffer;
};