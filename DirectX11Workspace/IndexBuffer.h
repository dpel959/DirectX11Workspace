#pragma once
class IndexBuffer
{
public:
	IndexBuffer(ComPtr<ID3D11Device> device);
	~IndexBuffer();

	const ComPtr<ID3D11Buffer>& GetComPtr() const { return _indexBuffer; }
	uint32 GetStride() const { return _stride; }
	uint32 GetOffset() const { return _offset; }
	uint32 GetCount() const { return _count; }

	// 이건 템플릿일 필요 없다. 어차피 정수만 받을 거라..
	void Create(const std::vector<uint32>& indices);

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11Buffer> _indexBuffer;  // 인덱스 버퍼.얼마나 정점을 많이 이용하는 도형을 만들 것이냐에 따라 정해주면 된다.

	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;
};

