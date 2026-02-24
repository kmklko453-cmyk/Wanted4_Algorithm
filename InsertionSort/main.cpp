#include <iostream>
#include <Windows.h>

using Comparer = bool(*)(int, int);

bool IsGreater(int x, int y)
{
	return x > y;
}
bool IsLess(int x, int y)
{
	return x < y;
}

//삽입 정렬 함수
int InsertionSory(int* array, int length, Comparer comparer)
{
	//예외 처리
	if (length <= 1)
	{
		return -1;
	}

	//배열 순회
	for (int ix = 1; ix < length; ++ix)
	{
		//현재 삽입할 값
		int key = array[ix];
		int jx = ix - 1;

		// key보다 큰 값은 오른쪽으로 배치
		//while (jx >= 0 && array[jx] > key)
		while (jx >= 0 && comparer(array[jx],key))
		{
			array[jx + 1] = array[jx];
			--jx;
		}

		//적절한 위치에 Key 삽입
		array[jx + 1] = key;
	}
}

//배열 항목 출력 함수
void PrintArrray(int* array, int length)
{
	for (int ix = 0; ix < length; ++ix)
	{
		std::cout << array[ix];
		if (ix < length - 1)
		{
			std::cout << ",";
		}

	}
	std::cout << "\n";
}

int main()
{
	// 자료 집합.
	int array[] = { 5, 2, 8, 4, 1, 7, 3, 6, 9, 10, 15, 13, 14, 12, 17, 16 };

	//배열 항목 개수 구하기
	//int length = ARRAYSIZE(array);
	int length = _countof(array);

	//정렬 전 출력
	std::cout << "정렬 전 배열 : ";
	PrintArrray(array, length);

	//정렬
	InsertionSory(array, length, IsGreater);

	//정렬 후 출력
	std::cout << "정렬 후 배열 : ";
	PrintArrray(array, length);


}