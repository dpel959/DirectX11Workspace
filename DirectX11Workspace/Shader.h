#pragma once
#include <string>

// HLSL에 쓸 수 있는 변수들을 넘겨주고 싶을때, (상수 버퍼, 텍스처, 샘플러 등)
// 이것이 '어디서 사용하는 것인지'를 지칭한다.

// 왜 또 값은 Bit Shift해서 써요? -> Bit Flag로 쓴다는 힌트를 주기 위해서이다.
// Bit Flag로 쓰는 이유는, 만약 VS, PS 두 용도 모두 쓰겠다고 하면, VS | PS 를 하면, int32 하나에 한번에 정보를 나타낼 수 있기 때문이다.
enum class ShaderScope
{
	None = 0,
	VertexShader = (1 << 0),
	PixelShader = (1 << 1),
};

// 강의와 차별점 : 이건 뭔가요? -> enum class는 int로 명시적으로 치환되지 않아, 연산자를 지원하지 않는다. 하지만 그러면 일일이 캐스팅을 해줘야하는 불편함이 남는다.
// 그렇다고, enum으로 만들 수도 없다. 이름이 전역으로 쓰여, 충돌하기 때문이다.
// DEFINE_ENUM_FLAG_OPERATORS는, 해당 이름의 enum class의 비트 연산 함수들을 만들어주는 매크로이다.
DEFINE_ENUM_FLAG_OPERATORS(ShaderScope);

class Shader
{
public:
	Shader(ComPtr<ID3D11Device> device);
	virtual ~Shader();

	virtual void Create(const std::wstring& path, const std::string& name, const std::string& version) = 0;

	const ComPtr<ID3DBlob>& GetBlob() const { return _blob; }

protected:
	void LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version);
	// 셰이더를 로드, blob에 저장해 넘겨줌

protected:
	std::wstring _path;
	std::string _name;
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3DBlob> _blob = nullptr; // 셰이더 로드, 저장
};

class VertexShader : public Shader
{
	using Super = Shader; // 꽤 많이 쓰는 기법이다. 중간에 부모를 하나 끼워넣어야 할 때, 이것만 바꾸면 되기 때문.
public:
	VertexShader(ComPtr<ID3D11Device> device);
	~VertexShader() override;

	const ComPtr<ID3D11VertexShader>& GetComPtr() const  { return _vertexShader; }

	void Create(const std::wstring& path, const std::string& name, const std::string& version) override;

protected:
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
};

class PixelShader : public Shader
{
	using Super = Shader;
public:
	PixelShader(ComPtr<ID3D11Device> device);
	~PixelShader() override;

	const ComPtr<ID3D11PixelShader>& GetComPtr() const { return _pixelShader; }

	void Create(const std::wstring& path, const std::string& name, const std::string& version) override;

protected:
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
};