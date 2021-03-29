// ParallelAssignment2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>

// determine the output accuracy
// a smaller value usually leads to performance downgrade
#define EPSILON 0.1

using namespace std;

// the output should match this with smallest difference
double CHALLENGE = 0.0;
double* DISTANCES;
int N;

/*
 * Simple struct to store each point info
 * Important to preserve the index for
 * later referencing
 */
struct Point {
	int index = 0;
	int x = 0;
	int y = 0;
};

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

// distance between two points on a plane
double calculateDistance(Point point1, Point point2)
{
	return sqrt(pow((point2.x - point1.x), 2) + pow((point2.y - point1.y), 2));
}

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

double recalculateListDist(vector<Point*> list)
{
	double distance = 0.0;
	Point* currentP = list.back(); 
	list.pop_back();
	for (Point*& p : list)
	{
		distance += calculateDistance(*currentP, *p);
		currentP = p;
	}
	return distance;
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
	return point;
}

/*
* Read file, store points in Point objects, store challenge
*/
void readFile(ifstream& file, string filename, vector<Point*>* points, double& goal)
{
	file.open(filename);
	if (file.fail()) {
		cout << "Failed to open the file." << endl;
		return;
	}
	string line;
	while (getline(file, line))
	{
		if (line.size() == 0) {
			continue;
		}
		else if (line.find(' ') == -1) {
			goal = stod(line);
			return;
		}
		Point* point = parsePoint(line);
		points->push_back(point);
	}
	file.close();
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

vector<Point*> findCombination(vector<Point*> list)
{
	vector<Point*> visited;
	double travelled = 0.0;
	int counter = 0;
	int index = 0;
	while (isCloseToChallenge(travelled))
	{
		Point* curP = list.front();
		visited.push_back(curP);
		list.erase(list.begin());
		index++;
		if (visited.size() < 1) continue;

		counter++;
		double distance = calculateDistance(*curP, *(*visited.end() - 2));
		travelled += distance;  
		
		if (isOverChallenge(travelled)) {
			if (visited.size() == list.size()) {
				visited.erase(visited.end() - counter, visited.end());
				travelled = recalculateListDist(visited);
				counter = 0;
			}
			else {
				visited.pop_back();
				travelled -= distance;
			}
		}
		else {
			index++;
		}
	}
	return visited;
}

int main()
{
	ifstream stream;
	vector<Point*>* points = new vector<Point*>();
	
	readFile(stream, "Challenge.txt", points, CHALLENGE);
	calculateAllDistances(points);

	size_t N = points->size();
	size_t P = (size_t) pow(N, 2);
	for (size_t i = 0; i < N - 1; i++)
	{
		for (size_t n = i + 1; n < N; n++)
		{
			printf("(%d, %d) --> %.5f\n", i, n, getPointsDistance(i, n));
		}
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
