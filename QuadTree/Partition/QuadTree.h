#pragma once

#include "Node.h"

class QuadTree
{
public:
	QuadTree(const Bounds bounds);
	~QuadTree();

	//트리에 노드 추가 함수
	void Insert(Node* node);

	//질의 함수
	std::vector<Node*> Query(Node* QueryNode);
	//void Query(Node* queryNode, std::vector<Node*>& outNodeList);

public:
	//트리의 최대 허용깊이
	static const int maxDepth = 5;


private:
	//루트 노드
	Node* root = nullptr;

};

