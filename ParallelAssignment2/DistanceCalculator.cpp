#include <iostream>
#include <cmath>
#include <vector>
#include <queue>

#define EPSILON 0.1

using namespace std;

double* DISTANCES;
int N;

struct Point {
	int index = 0;
	int x = 0;
	int y = 0;
	int successor = 0;
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

vector<Point*>* travel(queue<Point*>* points, const double GOAL)
{
	size_t POINTS_SIZE = points->size();
	Point* first = points->front();
	points->pop();
	first->successor = points->size();
	vector<Point*>* solution = new vector<Point*>{ first };
	vector<Point*>* explored = new vector<Point*>();
	double goal = 0;
	while (!points->empty()) {
		Point* current = points->front();
		points->pop();
		current->successor = POINTS_SIZE = solution->size() - 1;
		double distance = getPointsDistance(current->index, solution->back()->index);
		if (isDifferent(distance + goal, GOAL)) {
			solution->back()->successor--;
			while (solution->back()->successor == 0) {
				current = pop(solution);
				goal -= getPointsDistance(current->index, solution->back()->index);
				solution->back()->successor--;
				points->push(current);
			}
		}
		else {
			goal += distance;
			solution->push_back(current);
		}
		if (isAlmostTheSame(goal, GOAL)) {
			return solution;
		}
	}
	return NULL;
}


