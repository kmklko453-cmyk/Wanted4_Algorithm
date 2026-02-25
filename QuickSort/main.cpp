#include <iostream>
#include <Windows.h>
//퀵 정렬 재귀 함수

//파티션 함수
//기준점 - Pivot 선택 및 분할 함수
int Partition(int* array, int left, int right)
{
	//분할을 위한 변수
	//전달된 배열의 첫 항목을 피벗으로 선택(다른 방법도 가능/ 인덱스 3개 비교후 중간값)
	int pivot = array[left];
	int low = left + 1;
	int high = right;

	//왼쪽애서 오른쪽으로,오른쪽에서 왼쪽으로 이동하면서 분할처리
	while (low <= right)
	{
		// 왼쪽에서 오른쪽으로 이동처리
		// 기준 값보다 큰 값  찾기 (low 증가 처리)
		while (low <= right && array[low] <= pivot)
		{
			++low;

		}
		// 오른쪽에서 왼쪽으로 이동처리
		// 기준 값보다 큰 값  찾기 (high 감소 처리)
		while (high > left && array[high] >= pivot)
		{
			--high;
		}

		//두 인덱스가 교차했는지 확인
		if (low > high)
		{
			break;
		}

		//low와 high 교환
		std::swap<int>(array[low], array[high]);
	}

	//피벗 값과 왼쪽 데이터 집합의 마지막 요소(high)를 교환
	//노션 노트의 분할과정 이미지 참고
	std::swap<int>(array[left], array[high]);

	//피벗의 최종 위치 반환
	return high;
}

//퀵 정렬 재귀 함수
void QuickSort(int* array, int left, int right)
{
	//left와 right가 만나면 종료
	if (left >= right)
	{
		return;
	}
	//배열 분할
	int pivot = Partition(array, left, right);

	//왼쪽 배열 정렬 (재귀)
	QuickSort(array, left, pivot - 1);

	//오른쪽 배열 정렬 (재귀)
	QuickSort(array, pivot + 1, right);
}

//배열 항목 출력 함수
void PrintArrray(int* array, int length)
{
	for (int ix = 0; ix < length; ++ix)
	{
		std::cout << array[ix];
		if (ix < length - 1)
		{
			std::cout << ", ";
		}

	}
	std::cout << "\n";
}

int main()
{
	// 자료 집합.
	//int array[] = { 5, 2, 8, 4, 1, 7, 3, 6, 9, 10, 15, 13, 14, 12, 17, 16 };

	// 자료 집합 2.
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

	//배열 항목 개수 구하기
	int length = _countof(array);

	//정렬 전
	std::cout << "정렬 전 배열: ";
	PrintArrray(array, length);

	//정렬
	QuickSort(array, 0, length - 1);

	//정렬 후
	std::cout << "정렬 후 배열: ";
	PrintArrray(array, length);

}