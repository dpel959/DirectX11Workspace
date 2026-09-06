#pragma once
#include <vector>
#include "Types.h"

// 기하학적 도형의 표현! 버텍스, 버텍스 버퍼, 인덱스 버퍼 등의 '순수한 3D 형태'를 '지오메트리'라고 칭한다.
// 템플릿 클래스로 만들어주는 이유는 버텍스 버퍼에서 Create를 템플릿으로 만든 이유와 같다. 버텍스가 정해진 한 구조로만 들어오지는 않기 때문이다.

template<typename T>
class Geometry
{
public:
	Geometry();
	~Geometry();
	
	uint32 GetVertexCount() const { return static_cast<uint32>(_vertices.size()); }
	const void* GetVertexData() const { return _vertices.data(); }
	// 왜 void*로 건네줄까? const T&로 건네줘도 속도는 같을텐데
	// -> DirectX API는 'C 스타일의 원시포인터만을 요구한다'. 다렉은 C/C++ 기반의 저수준 API로, std::vector라는 것을 모른다.
	// 실제로 SUBRESOURCE_DATA를 보면, pSysmem은 const void*이다. 다렉이 필요로 하는 것은 '어디서부터 시작해서, 몇 바이트를 그래픽 카드로 복사할까?'뿐이다.
	
	const std::vector<T>& GetVertices() const { return _vertices; }

	uint32 GetIndexCount() const { return static_cast<uint32>(_indices.size()); }
	const void* GetIndexData() const { return _indices.data(); }
	const std::vector<uint32>& GetIndices() const { return _indices; }

	void AddVertex(const T& vertex) { _vertices.push_back(vertex); }
	void AddVertices(const std::vector<T>& vertices) { _vertices.insert(_vertices.end(), vertices.begin(), vertices.end()); }
	void SetVertices(const std::vector<T>& vertices) { _vertices = vertices; }

	void AddIndex(const uint32 index) { _indices.push_back(index); }
	void AddIndices(const std::vector<uint32>& indices) { _indices.insert(_indices.end(), indices.begin(), indices.end()); }
	void SetIndices(const std::vector<uint32>& indices) { _indices = indices; }

private:
	std::vector<T> _vertices; // 이거 만드는 건 CPU의 영역이다. RAM에 저장됨.
	std::vector<uint32> _indices; // 인덱스 버퍼에 넣을 인덱스 순서
}; 

template<typename T>
inline Geometry<T>::Geometry()
{
}

template<typename T>
inline Geometry<T>::~Geometry()
{
}
