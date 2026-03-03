#include "Partition/QuadTree.h"
#include <iostream>

// 단순한 레벨 클래스
class Level
{
public:
	Level() = default;
	~Level()
	{
		//레벨에 배치된  모든 Node 제거
		for (Node*& node : nodeList)
		{
			SaffeDelete(node);
		}
		nodeList.clear();
	}

	//레벨에 새로운 노드 추가
	void AddNewNode(Node* newNode)
	{
		nodeList.emplace_back(newNode);
	}


	//Getter
	inline const std::vector<Node*>& GetAllNode() const { return nodeList; }

private:
	//레벨에 배치된 노드(Actor로 생각해볼 수 있음)
	std::vector<Node*> nodeList;
};



int main()
{
	//레벨 구성
	Level level;
	level.AddNewNode(new Node(Bounds(60, 80)));
	level.AddNewNode(new Node(Bounds(50, 50)));
	level.AddNewNode(new Node(Bounds(90, 120)));
	level.AddNewNode(new Node(Bounds(150, 150)));
	level.AddNewNode(new Node(Bounds(10, 10)));

	//트리 구성
	//(0,0) 좌표를 왼쪽 상단 지점으로
	QuadTree tree(Bounds(0, 0, 200, 200));

	//삽입
	for (Node* node : level.GetAllNode())
	{
		tree.Insert(node);
	}

	std::cout << "노드 삽입 완료\n";

	//질의 (Query)
	//Node testNode(Bounds(10, 10));
	Node testNode(Bounds(45, 45, 20, 40));
	std::vector<Node*> intersects = tree.Query(&testNode);

	//검색 성공
	if (!intersects.empty())
	{
		std::cout << "겹치는 노드들 " << intersects.size() << "개 찾았습니다.";

	}

	//검색 실패
	else
	{
		std::cout << "겹치는 노드를 찾지 못했습니다.\n";
	}
}