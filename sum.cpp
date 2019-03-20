/*
MPI Routine list:
https://www.mpich.org/static/docs/latest/
*/

#include "mpi.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define MAXSIZE 100

using namespace std;

int main(int argc, char** argv) {
	int myid, numprocs;
	int data[MAXSIZE], i, x, low, high, myresult = 0, result, mymax = 0, maxall;
	char fn[255];
	FILE* fp;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (0 == myid) {
		/* open input file and intialize data */
		strcpy(fn, getenv("PWD"));
		strcat(fn, "/rand_data.txt");
		if (NULL == (fp = fopen(fn, "r"))) {
			printf("Can't open the input file: %s\n\n", fn);
			exit(1);
		}
		for (i = 0; i < MAXSIZE; i++) {
			fscanf(fp, "%d", &data[i]);
		}
	}

	/* broadcast data */
	MPI_Bcast(data, MAXSIZE, MPI_INT, 0, MPI_COMM_WORLD);

	/* add portion of data */
	x = MAXSIZE / numprocs; /* must be an integer */
	low = myid * x;
	high = low + x;
	for (i = low; i < high; i++) {
		myresult += data[i];
		mymax = max(mymax, data[i]);
	}
	printf("I got (sum: %4d, max: %2d) from process %3d\n", myresult, mymax, myid);

	/* compute global sum */
	MPI_Reduce(&myresult, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	// compute max
	MPI_Reduce(&mymax, &maxall, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	if (0 == myid) {
		printf("The sum is %d with %d as highest number found.\n", result, maxall);
	}

	MPI_Finalize();
}
