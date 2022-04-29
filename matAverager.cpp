
// Made by Joshua Wilkinson for SIUE CS 286

#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <sstream>
#include <string>

using namespace std;
// a class to get more accurate time

class stopwatch
{

private:
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	bool timing;

public:
	stopwatch() : timing(false)
	{
		t1 = std::chrono::high_resolution_clock::time_point::min();
		t2 = std::chrono::high_resolution_clock::time_point::min();
	}

	void start()
	{
		if (!timing)
		{
			timing = true;
			t1 = std::chrono::high_resolution_clock::now();
		}
	}

	void stop()
	{
		if (timing)
		{
			t2 = std::chrono::high_resolution_clock::now();
			timing = false;
		}
	}

	void reset()
	{
		t1 = std::chrono::high_resolution_clock::time_point::min();
		t2 = std::chrono::high_resolution_clock::time_point::min();
		;
		timing = false;
	}

	// will return the elapsed time in seconds as a double
	double getTime()
	{
		std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		return elapsed.count();
	}
};

// function takes an array pointer, and the number of rows and cols in the array, and
// allocates and intializes the two dimensional array to a bunch of random numbers

void makeRandArray(unsigned int **&data, unsigned int rows, unsigned int cols, unsigned int seed)
{
	// allocate the array
	data = new unsigned int *[rows];
	for (unsigned int i = 0; i < rows; i++)
	{
		data[i] = new unsigned int[cols];
	}

	// seed the number generator
	// you should change the seed to get different values
	srand(seed);

	// populate the array

	for (unsigned int i = 0; i < rows; i++)
		for (unsigned int j = 0; j < cols; j++)
		{
			data[i][j] = rand() % 10000 + 1; // number between 1 and 10000
		}
}

void getDataFromFile(unsigned int **&data, char fileName[], unsigned int &rows, unsigned int &cols)
{
	ifstream in;
	in.open(fileName);
	if (!in)
	{
		cerr << "error opening file: " << fileName << endl;
		exit(-1);
	}

	in >> rows >> cols;
	data = new unsigned int *[rows];
	for (unsigned int i = 0; i < rows; i++)
	{
		data[i] = new unsigned int[cols];
	}

	// now read in the data

	for (unsigned int i = 0; i < rows; i++)
		for (unsigned int j = 0; j < cols; j++)
		{
			in >> data[i][j];
		}
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " [input data file] [num of threads to use] " << endl;

		cerr << "or" << endl
			 << "Usage: " << argv[0] << " rand [num of threads to use] [num rows] [num cols] [seed value]" << endl;
		exit(0);
	}

	// read in the file
	unsigned int rows, cols, seed;
	unsigned int numThreads;
	unsigned int **data;
	// convert numThreads to int
	{
		stringstream ss1;
		ss1 << argv[2];
		ss1 >> numThreads;
	}

	string fName(argv[1]);
	if (fName == "rand")
	{
		{
			stringstream ss1;
			ss1 << argv[3];
			ss1 >> rows;
		}
		{
			stringstream ss1;
			ss1 << argv[4];
			ss1 >> cols;
		}
		{
			stringstream ss1;
			ss1 << argv[5];
			ss1 >> seed;
		}
		makeRandArray(data, rows, cols, seed);
	}
	else
	{
		getDataFromFile(data, argv[1], rows, cols);
	}

	// UNCOMMENT if you want to print the data array
	// cerr << "data: " << endl;
	// for (unsigned int i = 0; i < rows; i++)
	// {
	// 	for (unsigned int j = 0; j < cols; j++)
	// 	{
	// 		cerr << "i,j,data " << i << ", " << j << ", ";
	// 		cerr << data[i][j] << " ";
	// 	}
	// 	cerr << endl;
	// }
	// cerr << endl;

	// tell omp how many threads to use
	omp_set_num_threads(numThreads);

	stopwatch S1;

	S1.start();

	/////////////////////////////////////////////////////////////////////
	///////////////////////  YOUR CODE HERE       ///////////////////////
	///////////////         Make it parallel!	 ////////////////////
	/////////////////////////////////////////////////////////////////////

	int maxrow = 0;
	int maxcol = 0;
	int maxavg = 0;
	int avg9 = 0;

// center block
#pragma omp parallel for
	for (int i = 1; i <= rows - 2; i++)
	{
		for (int j = 1; j <= cols - 2; j++)
		{
			if ((data[i - 1][j - 1]) + 80000 < avg9)
			{
				continue;
			}
			if ((data[i - 1][j]) + 80000 < avg9)
			{
				continue;
			}
			if ((data[i - 1][j + 1]) + 80000 < avg9)
			{
				continue;
			}

			if ((data[i][j - 1]) + 80000 < avg9)
			{
				continue;
			}
			if ((data[i][j]) + 80000 < avg9)
			{
				continue;
			}
			if ((data[i][j + 1]) + 80000 < avg9)
			{
				continue;
			}

			if ((data[i + 1][j - 1]) + 80000 < avg9)
			{
				continue;
			}
			if ((data[i + 1][j]) + 80000 < avg9)
			{
				continue;
			}
			if ((data[i + 1][j + 1]) + 80000 < avg9)
			{
				continue;
			}
			int sum = (data[i - 1][j - 1] + data[i - 1][j] + data[i - 1][j + 1] + data[i][j - 1] + data[i][j] + data[i][j + 1] + data[i + 1][j - 1] + data[i + 1][j] + data[i + 1][j + 1]);
			if (sum > avg9)
			{
#pragma omp critical
				if (sum > avg9)
				{
					maxrow = i;
					maxcol = j;
					maxavg = sum * 0.111;
					avg9 = sum;
				}
			}
		}
	}

// top edge
#pragma omp parallel for
	for (int j = 1; j <= cols - 2; j++)
	{
		int sum = (data[0][j - 1] + data[0][j] + data[0][j + 1] + data[1][j - 1] + data[1][j] + data[1][j + 1]);
		if (sum > maxavg * 6)
		{
#pragma omp critical
			if (sum > maxavg * 6)
			{
				maxrow = 0;
				maxcol = j;
				maxavg = sum * 0.166;
			}
		}
	}

// bottom edge
#pragma omp parallel for
	for (int j = 1; j <= cols - 2; j++)
	{
		int sum = (data[rows - 1][j - 1] + data[rows - 1][j] + data[rows - 1][j + 1] + data[rows - 2][j - 1] + data[rows - 2][j] + data[rows - 2][j + 1]);
		if (sum > maxavg * 6)
		{
#pragma omp critical
			if (sum > maxavg * 6)
			{
				maxrow = rows - 1;
				maxcol = j;
				maxavg = sum * 0.166;
			}
		}
	}

	// left edge
#pragma omp parallel for
	for (int i = 1; i <= rows - 2; i++)
	{
		int sum = (data[i - 1][0] + data[i][0] + data[i + 1][0] + data[i - 1][1] + data[i][1] + data[i + 1][1]);
		if (sum > maxavg * 6)
		{
#pragma omp critical
			if (sum > maxavg * 6)
			{
				maxrow = i;
				maxcol = 0;
				maxavg = sum * 0.166;
			}
		}
	}

	// right edge
#pragma omp parallel for
	for (int i = 1; i <= rows - 2; i++)
	{
		int sum = (data[i - 1][cols - 2] + data[i][cols - 2] + data[i + 1][cols - 2] + data[i - 1][cols - 1] + data[i][cols - 1] + data[i + 1][cols - 1]);
		if (sum > maxavg * 6)
		{
#pragma omp critical
			if (sum > maxavg * 6)
			{
				maxrow = i;
				maxcol = cols - 1;
				maxavg = sum * 0.166;
			}
		}
	}

	// top left corner
	int sum = (data[0][0] + data[0][1] + data[1][0] + data[1][1]);
	if (sum > maxavg * 4)
	{
		maxrow = 0;
		maxcol = 0;
		maxavg = sum * 0.25;
	}

	// top right corner
	sum = (data[0][cols - 1] + data[1][cols - 1] + data[0][cols - 2] + data[1][cols - 2]);
	if (sum > maxavg * 4)
	{
		maxrow = 0;
		maxcol = 0;
		maxavg = sum * 0.25;
	}

	// bottom left corner
	sum = (data[rows - 1][0] + data[rows - 1][1] + data[rows - 2][0] + data[rows - 2][1]);
	if (sum > maxavg * 4)
	{
		maxrow = 0;
		maxcol = 0;
		maxavg = sum * 0.25;
	}

	// bottom right corner
	sum = (data[rows - 1][cols - 1] + data[rows - 1][cols - 2] + data[rows - 2][cols - 1] + data[rows - 2][cols - 2]);
	if (sum > maxavg * 4)
	{
		maxrow = 0;
		maxcol = 0;
		maxavg = sum * 0.25;
	}

	S1.stop();

	// print out the max value here

	cout << "max average: " << maxavg << " at row: " << maxrow - 1 << ", col: " << maxcol - 1 << endl;

	cerr << "elapsed time: " << S1.getTime() << endl;
}
