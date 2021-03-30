#include <iostream>
#include <cmath>
#include <vector>

#define EPSILON 0.1

using namespace std;

double* DISTANCES;
int N;

struct Point {
	int index = 0;
	int x = 0;
	int y = 0;
};

double calculateDistance(Point* point1, Point* point2)
{
	return sqrt(pow((point2->x - point1->x), 2) + pow((point2->y - point1->y), 2));
}

void calculateAllDistances(vector<Point*>* points)
{
	N = points->size();
	size_t counter = 0;
	// As we only need the triangle numbers
	const size_t SIZE = N * (N - 1) / 2;
	DISTANCES = new double[SIZE];
	vector<Point*>::iterator outerPtr = points->begin();
	while (outerPtr != points->end())
	{
		Point* x = *outerPtr;
		outerPtr++;
		vector<Point*>::iterator innerPtr = outerPtr;
		while (innerPtr != points->end())
		{
			Point* y = *innerPtr;
			if (counter < SIZE) {
				DISTANCES[counter] = calculateDistance(x, y);
			}
			counter++;
			innerPtr++;
		}
	}
}

double getPointsDistance(int x, int y)
{
	if (x == y) return 0.0;
	int Y = min(x, y), X = max(x, y);
	int offset = (Y * (Y + 1) / 2) + 1;
	return DISTANCES[Y * (N - 1) + X - offset];
}

/*
* To compare both floating point values
* If the absolute difference is less than EPSILON
* We will consider both values are the same.
*/
int isAlmostTheSame(double num1, double num2)
{
	return abs(num1 - num2) <= EPSILON;
}

int isDifferent(double num1, double num2) {
	return (num2 - num1) > EPSILON;
}

template <typename T>
T pop(vector<T>* list)
{
	T item = list->back();
	list->pop_back();
	return item;
}
