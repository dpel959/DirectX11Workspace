#pragma once
#include <cassert>
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>
#include "Types.h"
class VertexBuffer
{
public:
	VertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> device);
	~VertexBuffer();

	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetComPtr() const { return _vertexBuffer; }
	uint32 GetStride() const { return _stride; }
	uint32 GetOffset() const { return _offset; }
	uint32 GetCount() const { return _count; }

	// 왜 템플릿으로 해요? -> 버텍스는 여러 형태가 있을 수 있음. uv, color, 포지션 등.
	template<typename T>
	void Create(const std::vector<T>& vertices)
	{
		_stride = sizeof(T);
		_count = static_cast<uint32>(vertices.size());

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
			desc.ByteWidth = static_cast<uint32>(_stride * _count); // 사이즈는?
		}

		// 그래서 실제로 CPU의 어떤 데이터를 넘겨줄건데? 

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data(); // == &_vertices[0]

		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf()); // CPU 데이터 data를 desc에 따라 _vertexBuffer로 옮겨줘

		assert(SUCCEEDED(hr));
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device; // 참조용! 물론 싱글톤으로 만들어놔도 됨.
	Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer; // 이제 GPU의 VRAM으로 넘겨주는 거임.

	// 이건 나중에도 공통적으로 사용해서, 저장해주자
	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;
};

