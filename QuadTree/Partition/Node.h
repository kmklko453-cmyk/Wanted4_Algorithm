#pragma once

#include "Bounds.h"
#include <vector>

//메모리 삭제 헬퍼 함수
template <typename T>
void SaffeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}


//영역 구분을 위한 열거형
enum class NodeIndex
{
	//완전히 포함되는 4가지
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	//경계선 걸침
	Straddling,
	//영역을 벗어난 경우
	OutOfArea
};

//쿼드 트리를 구성하는 노드 클래스
// - 사실 속성 먼저 작성하는 건 좋은 방법이 아니라고 한다. (어려움)
// - 이 객체가 담당하는 책임을 먼저 파악
// - 책임을 수행하는데 필요한 기능 (메소드)를 정리하고
// - 메소드를 구현하는데 필요한 속성을 정의
class Node
{
public:
	Node(const Bounds& bounds, int depth = 0);
	~Node();

	//삽입(Insert) 
	void Insert(Node* node);

	//질의( Query-물어보기)
	//bounds: 영역 테스트 요청 값
	//possible: 전달된 영역을 관리할 가능성이 있는 노드 목록
	void Query(const Bounds& bounds, std::vector<Node*>& possibleNode);

	//정리
	void Clear();

	//Getter
	inline const Bounds& GetBounds() const { return bounds; }
	inline const std::vector<Node*>& Points() const { return points; }
	

	//자식노드
	inline Node* TopLeft() const{ return topLeft; }
	inline Node* TopRight() const{ return topRight; }
	inline Node* BottomLeft() const{ return bottomLeft; }
	inline Node* BottomRight() const{ return bottomRight; }

private:
	//4분할 함수
	bool SubDivide();
	
	//영역이 분할됐는지 여부 확인 함수
	bool IsDivided();

	//전달된 영역과 겹치는 영역을 반환하는 함수
	NodeIndex TestRegion(const Bounds& bounds);

	//전달된 영역이 포함되거나 거치는 영역을 모두 구할 때 사용하는 함수
	std::vector<NodeIndex> GetQuad(const Bounds& bounds);

	//자식 노드 정리 함수
	void ClearChildren();

private:
	// 현재 노드의 깊이
	int depth = 0;

	//노드의 영역(Bounds)
	Bounds bounds;

	//현재 영역에 포함된 노드(목록)
	std::vector<Node*> points;

	//자식 노드 (4개의 노드)
	Node* topLeft = nullptr;
	Node* topRight = nullptr;
	Node* bottomLeft = nullptr;
	Node* bottomRight = nullptr;

};

