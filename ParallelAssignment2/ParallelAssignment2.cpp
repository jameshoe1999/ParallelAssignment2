// ParallelAssignment2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <omp.h>
#include <thread>
#include <Windows.h>

// determine the output accuracy
// a smaller value usually leads to performance downgrade
#define EPSILON 0.1

using namespace std;

// the output should match this with smallest difference
double CHALLENGE = 0.0;
double* DISTANCES;
int N = 0;
int NUM_THREADS = thread::hardware_concurrency();

/*
 * Simple struct to store each point info
 * Important to preserve the index for
 * later referencing
 */
struct Point {
	int index = 0;
	int x = 0;
	int y = 0;
	int* successors;
};

void setNumThreads(int points_size) 
{
	NUM_THREADS = min(thread::hardware_concurrency(), max(2, (int)(points_size / 15)));
}

/*
* To compare both floating point values
* If the absolute difference is less than EPSILON
* We will consider both values are the same.
*/
int isCloseToChallenge(double num)
{
	return abs(num - CHALLENGE) <= EPSILON;
}

/*
* Base case to halt the searching
*/
int isOverChallenge(double num) {
	return (num - CHALLENGE) > EPSILON;
}

double calculateDistance(Point* point1, Point* point2)
{
	return sqrt(pow((point2->x - point1->x), 2) + pow((point2->y - point1->y), 2));
}

void calculateAllDistances(deque<Point*>* points)
{
	const int SIZE = N * (N - 1) / 2;
	const int ROWNUM = SIZE / N;
	DISTANCES = new double[SIZE];
	// As we only need the triangle numbers
	#pragma omp parallel for num_threads(NUM_THREADS)
	for (int i = 0; i < N; i++)
	{
		Point* x = points->at(i);
		int offset = (i * (i + 1) / 2) + 1;
		for (int n = i + 1; n < N; n++)
		{
			Point* y = points->at(n);
			int pos = i * (N - 1) + n - offset;
			if (pos < SIZE) {
				DISTANCES[pos] = calculateDistance(x, y);
			}
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
* Extract point info from text line
* parse it to a Point object
* and return it back.
*/
Point* parsePoint(string line)
{
	Point* point = new Point;
	size_t len = line.length();
	size_t space = line.find(' ', 0);
	size_t space2 = line.find(' ', space + 1);
	point->index = stoi(line.substr(0, space));
	point->x = stoi(line.substr(space + 1, len - space2 + 1));
	point->y = stoi(line.substr(space2 + 1));
	point->successors = new int[NUM_THREADS];
	return point;
}

/*
* Read file, store points in Point objects, store challenge
*/
deque<Point*>* readFile(ifstream& file, string filename)
{
	file.open(filename);
	if (file.fail()) {
		cout << "Failed to open the file." << endl;
		exit(-1);
	}
	string line;
	deque<Point*>* points = new deque<Point*>();
	int counter = 0;
	while (getline(file, line))
	{
		if (line.size() == 0) {
			continue;
		}
		else if (line.find(' ') == -1) {
			CHALLENGE = stod(line);
			break;
		}
		points->push_back(parsePoint(line));
	}
	N = points->size();
	file.close();
	return points;
}

/*
* print the visited points
* call {cout} to print the result in console
*/
void printSolution(ostream& output, vector<Point*>* solution)
{
	for (Point*& p : *solution)
	{
		output << p->index << endl;
	}
}


Point* popBack(vector<Point*>* list)
{
	Point* item = list->back();
	list->pop_back();
	return item;
}

/*
* This method is essential to mark
* down the number of remaining items
* in the list as the successor of the
* popped item.
*/

Point* popFront(deque<Point*>* list)
{
	Point* item = list->front();
	list->pop_front();
	item->successors[omp_get_thread_num()] = list->size();
	return item;
}

vector<Point*>* travel(deque<Point*>* points, volatile int &stopFlag)
{
	vector<Point*>* solution = new vector<Point*>{ popFront(points) };
	Point* item = NULL;
	double goal = 0, distance = 0;
	while (!points->empty() && !stopFlag) {
		item = popFront(points);
		distance = getPointsDistance(item->index, solution->back()->index);
		if (isOverChallenge(distance + goal)) {
			points->push_back(item);
			solution->back()->successors[omp_get_thread_num()]--;
			while (solution->size() > 0) {
				if (solution->back()->successors[omp_get_thread_num()] <= 0) {
					Point* point = popBack(solution);
					points->push_back(point);
					if (solution->empty()) {
						solution->push_back(popFront(points));
						goal = 0;
						break;
					}
					goal -= getPointsDistance(point->index, solution->back()->index);
					solution->back()->successors[omp_get_thread_num()]--;
				}
				else {
					break;
				}
			}
		}
		else {
			goal += distance;
			solution->push_back(item);
		}

		if (isCloseToChallenge(goal)) {
			return solution;
		}
	}
	delete(solution);
	return NULL;
}

/*
* 
*/
vector<Point*>* findSolution(deque<Point*>* points) 
{
	vector<Point*>* solution = NULL;
	int* solutionSizes = new int[N];
	volatile int max = 0, shouldStop = 0;
	#pragma omp parallel for num_threads(NUM_THREADS)
	for (int i = 0; i < N; i++)
	{
		deque<Point*>* rotated = new deque<Point*>(*points);
		rotate(rotated->begin(), rotated->begin() + i, rotated->end());
		vector<Point*>* temp = travel(rotated, shouldStop);
		if (temp != NULL) {
			if (temp->size() > max) {
				#pragma omp critical
				solution = temp;
				max = temp->size();
				if (temp->size() == N) {
					shouldStop = 1;
					break;
				}
			}
		}
	}
	return solution;
}

int main()
{
	ifstream stream;
	vector<Point*>* solution;
	deque<Point*>* points = readFile(stream, "Challenge.txt");
	setNumThreads(points->size());
	calculateAllDistances(points);
	solution = findSolution(points);
	if (solution != NULL) {
		ofstream output;
		output.open("Solution.txt");
		printSolution(output, solution);
	}
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
