#include <iostream>



//선택 정렬 함수
void SelectionSort(int* array, int length)
{
	//예외처리
	if (length <= 1)
	{
		return;
	}

	for (int ix = 0; ix < length; ++ix)
	{
		//최소값
		int minIndex = ix;

		//현재 위치에서 끝까지 반복
		//회차가 거듭 될수록 오른쪽으로 위치를 옮겨가며 정렬
		for (int jx = ix + 1; jx < length; ++jx)
		{
			//비교
			if (array[minIndex] > array[jx])
			{
				minIndex = jx;
			}
		}

		//회차 전에 저장해뒀던 인덱스가 바뀌었는지 확인
		if (minIndex != ix)
		{
			std::swap<int>(array[ix], array[minIndex]);
		}
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
	// 자료 집합.
	int array[] = { 5, 2, 8, 4, 1, 7, 3, 6, 9, 10, 15, 13, 14, 12, 17, 16 };
	int length = sizeof(array) / sizeof(array[0]);

	//정렬 전 출력
	std::cout << "정렬 전 배열: ";
	PrintArrray(array, length);

	//정렬
	SelectionSort(array, length);
	
	//정렬 전 출력
	std::cout << "정렬 후 배열: ";
	PrintArrray(array, length);


}