#include "AStar.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <Windows.h>

AStar::AStar()
{
}

AStar::~AStar()
{
    //메모리 정리
    for (Node* node : openList)
    {
        SafeDelete(node);
    }
    openList.clear();

    for (Node* node : closedList)
    {
        SafeDelete(node);
    }
    closedList.clear();
}

std::vector<Node*> AStar::FindPath(Node* startNode, Node* goalNode, std::vector<std::vector<int>>& grid)
{
    return std::vector<Node*>();
}

void AStar::DisplayGridWithPath(std::vector<std::vector<int>>& grid, const std::vector<Node*>& path)
{
    static COORD position = { 0, 0 };
    static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, position);

    static int white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    static int green = FOREGROUND_GREEN;
    static int red = FOREGROUND_RED;

    //구분을 위해 설정했던 데이터 초기화
    for (int y = 0; y < static_cast<int>(grid.size()); ++y)
    {
        for (int x = 0; x < static_cast<int>(grid.size()); ++x)
        {
            int& value = grid[y][x];
            if (value == 5)
            {
                value = 0;
            }
        }
    }

    //경로를 제외한맵 출력
    for (int y = 0; y < static_cast<int>(grid.size()); ++y)
    {
        for (int x = 0; x < static_cast<int>(grid.size()); ++x)
        {
            int& value = grid[y][x];
            //시작위치 = 2
            if (value == 2)
            {
                SetConsoleTextAttribute(handle, red);
                std::cout << "S ";
                continue;
            }
            
            //목표 위치 = 3
            if (value == 3)
            {
                SetConsoleTextAttribute(handle, red);
                std::cout << "G ";
                continue;
            }

            //장애물 = 1
            if (value == 1)
            {
                SetConsoleTextAttribute(handle, white);
                std::cout << "1 ";
                continue;
            }

            //빈 공간 == 0
            if (value == 0)
            {
                SetConsoleTextAttribute(handle, white);
                std::cout << "0 ";
            }
        }

        //개행(다음 줄로 이동)
        std::cout << "\n";
    }

    //경로 출력
    for (Node* const node : path)
    {
        //경로는 '*' 표시
        position.X = static_cast<short>(node->position.x * 2);
        position.Y = static_cast<short>(node->position.y);

        SetConsoleCursorPosition(handle, position);
        SetConsoleTextAttribute(handle, green);

        std::cout << "* ";
        //잠시 대기(단위 : 밀리초)
        DWORD delay = static_cast<DWORD>(0.0f * 1000);
        Sleep(delay);
    }

    //위치 초기화
    position.X = 0;
    position.Y = 0;
}

std::vector<Node*> AStar::ConstructPath(Node* gaolPath)
{
    //경로를 저장할 배열 선언
    std::vector<Node*> path;

    //역추적하면서 path에 채우기
    Node* currentNode = goalNode;
    while (currentNode)
    {
        path.emplace_back(currentNode);
        currentNode = currentNode->parentNode;
    }

    //이렇게 얻은 결과는 순서가 거꾸로
    //그래서 다시 거꾸로 정렬이 필요함
    std::reverse(path.begin(), path.end());
    return path;
}

float AStar::CalculateHeuristic(Node* currentNode, Node* goalNode)
{
    //추정 비용 계산 함수
    //고민해볼 계산 방식
    //현재 노드에서 목표 노드까지의 비용 계산
    //단순 거리를 휴리스틱 비용으로 계산
    Position diff = *currentNode - *goalNode;
    return static_cast<float>(std::sqrt(diff.x * diff.x + diff.y * diff.y));
}

bool AStar::IsInRange(int x, int y, const std::vector<std::vector<int>>& grid)
{
    //예외처리
    if (grid.empty() || grid[0].empty())
    {
        return false;
    }

    //x,y 범위가 벗어났는지 확인
    if (x < 0 | x >= static_cast<int>(grid[0].size()) 
        || y < 0 || y >= static_cast<int>(grid.size()))
    {
        return false;
    }

    //벗어나지 않았다면 true 반환
    return true;
}

bool AStar::HasVisited(int x, int y, float gCost)
{
    //열린 리스트에 이미 같은 위치가 있고
    //기존  위치가 비용이 더 낮으면 방문했다고 판단(중복 방지)
    for (Node* const node : openList)
    {
        if (node->position.x == x && node->position.y == y
            && gCost >= node->gCost)
        {
            return true;
        }
    }

    //닫힌 리스트에 이미 같은 위치가 있고
    //비용이 더 낮으면 방문했다고 판단
    for (Node* const node : closedList)
    {
        if (node->position.x == x && node->position.y == y
            && gCost >= node->gCost)
        {
            return true;
        }
    }

    return false;
}

bool AStar::IsDestination(const Node* const node)
{
    //두 노드가 같은지 비교 node = goalNode 주소 비교도 가능 
    return *node == *goalNode;
}

void AStar::DisplayGrid(std::vector<std::vector<int>>& grid)
{
    //좌표 변수
    static COORD position = { 0, 0 };
    static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, position);

    //색상값
    static int white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    static int green = FOREGROUND_GREEN;
    static int red = FOREGROUND_RED;
    //맵 출력
    for (int y = 0; y < static_cast<int>(grid.size()); ++y)
    {
        for (int x = 0; x < static_cast<int>(grid[0].size()); ++x)
        {
            //시작 위치 = 2;
            if (grid[y][x] == 2)
            {
                SetConsoleTextAttribute(handle, red);
                std::cout << "S ";
                continue;
            }

            //목표 위치 = 3
            if (grid[y][x] == 3)
            {
                SetConsoleTextAttribute(handle, red);
                std::cout << "G ";
                continue;
            }

            //장애물 = 1
            if (grid[y][x] == 1)
            {
                SetConsoleTextAttribute(handle, white);
                std::cout << "1 ";
                continue;
            }

            //경로 == 5
            if (grid[y][x] == 5)
            {
                SetConsoleTextAttribute(handle, green);
                std::cout << "+ ";
                continue;
            }

            //빈 공간 == 0
            if (grid[y][x] == 0)
            {
                SetConsoleTextAttribute(handle, white);
                std::cout << "0 ";
            }
        }

        //개행 (다음줄로 이동)
        std::cout << "\n";
    }
    
    //위치 초기화
    position.X = 0;
    position.Y = 0;
}
