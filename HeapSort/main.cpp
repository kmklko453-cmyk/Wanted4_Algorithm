#include <iostream>

// 배열을 힙으로 만드는 함수.
// array: 정렬 중인 배열.
// length: 배열 항목 수.
// index: 현재 처리 중인 인덱스.
void Heapify(int* array, int length, int index)
{
	// 힙 구성을 완료할 때까지 반복.
	while (true)
	{
		// 최대힙(MaxHeap).
		int largest = index;

		// 자손 인덱스 구하기
		// 왼쪽 자손: 2N + 1.
		int leftChild = 2 * index + 1;
		// 오른쪽 자손: 2N + 2.
		int rightChild = 2 * index + 2;
		
		// 더 큰값 크기 (+자손 인덱스 존재 여부 확인).
		if (leftChild < length
			&& array[leftChild] > array[largest])
		{
			largest = leftChild;
		}

		// 더 큰값 크기 (+자손 인덱스 존재 여부 확인).
		if (rightChild < length
			&& array[rightChild] > array[largest])
		{
			largest = rightChild;
		}

		// 현재 노드가 가장 크면 종료.
		// 힙 구조가 유지된 것이기 때문.
		if (largest == index)
		{
			break;
		}

		// 현재 노드와 자손 노드(가장 큰 값을 가지는) 교환.
		std::swap<int>(array[index], array[largest]);

		// largest를 index로 업데이트.
		index = largest;
	}
}

// 힙 정렬 함수.
void HeapSort(int* array, int length)
{
	// 힙 구조로 변환.
	for (int ix = length / 2 - 1; ix >= 0; --ix)
	{
		Heapify(array, length, ix);
	}
	
	// 힙 정렬.
	for (int ix = length - 1; ix > 0; --ix)
	{
		// 루트 노드와 마지막 노드(현재 노드)를 교환.
		std::swap<int>(array[0], array[ix]);

		// 힙 구조를 유지하기 위해 루트 노드부터 다시 힙으로 만들기.
		Heapify(array, ix, 0);
	}
}

//배열 출력하는 함수
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
	//int array[] = { 5, 2, 8, 4, 1, 7, 3, 6, 9, 10, 15, 13, 14, 12, 17, 16 };
	int array[] =
	{
		1, 58, 10, 20, 56, 63, 73, 5, 28, 37,
		80, 61, 82, 45, 11, 66, 83, 59, 22, 64,
		52, 89, 94, 76, 44, 40, 75, 2, 23, 57,
		92, 8, 41, 96, 15, 84, 35, 69, 54, 47,
		90, 24, 43, 74, 34, 85, 72, 95, 18, 17,
		98, 9, 29, 53, 27, 79, 39, 51, 31, 16,
		6, 97, 26, 100, 21, 48, 33, 60, 91, 19,
		30, 13, 71, 78, 87, 25, 81, 4, 42, 93,
		49, 12, 14, 7, 62, 77, 38, 99, 88, 50,
		32, 46, 70, 3, 86, 68, 36, 67, 55, 65
	};

	int length = _countof(array);

	//출력
	std::cout << "정렬 전 배열: ";
	PrintArrray(array, length);

	//정렬
	HeapSort(array, length);

	std::cout << "정렬 후 배열: ";
	PrintArrray(array, length);


}