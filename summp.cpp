#include <cstdio>
#include <fstream>
#include <omp.h>
#include <vector>

using namespace std;

int main() {
	int x, maxthread, threadid, localsum, sum = 0, sizeperthread, start, finish;
	vector<int> numdata;

	// read number from rand_data.txt
	// work only for C++
	ifstream ifile;
	ifile.open("rand_data.txt");
	while (ifile >> x) {
		numdata.push_back(x);
	}

	//run with maximum number of threads
	maxthread = omp_get_max_threads();
	sizeperthread = numdata.size() / maxthread;
	printf("Max thread is %d\n", maxthread);

#pragma omp parallel private(threadid, localsum, start, finish) shared(sum)
	//all thread run this block
	{
		threadid = omp_get_thread_num();
		localsum = 0;
		start = threadid * sizeperthread;
		finish = start + sizeperthread;
		// sum
		for (int i = start; i < finish; i++) {
			localsum += numdata[i];
		}
		printf("I got %d from thread %d\n", localsum, threadid);

#pragma omp critical
		// this block can only run one at a time
		{
			sum += localsum;
		}
	}

	// print global sum
	printf("The sum is %d.\n", sum);
}
