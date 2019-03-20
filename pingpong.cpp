/*
MPI Routine list:
https://www.mpich.org/static/docs/latest/
*/

#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <algorithm>

using namespace std;

double timeSlave[2];
double timeM1, timeM2, timeS1, timeS2;
double totalMasterTime, totalSlaveTime;
char *data, *rData;

void randomizeData(int l){
	data = new char[l];
	for(int i = 0; i < l - 1; i++){
		data[i] = ('a' + (rand()%26));
	}
	data[l - 1] = '\0';
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	int procId, procNum, sendLength, sendTry;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procId);
	MPI_Barrier(MPI_COMM_WORLD);

	if(procNum!=2){
		if(procId == 0){
			printf("Program only for 2 process only!\n");
		}
		exit(1);
	}

	if(argc != 3){
		if(procId == 0){
			printf("usage: pingpong [sendByteLength] [sendTry]\n");
		}
		exit(1);
	}

	sendLength = atoi(argv[1]);
	sendTry = atoi(argv[2]);

	if(procId == 0){
		printf("Send data with length of %d byte(char) %d times\n", sendLength, sendTry);
	}

	data = new char[sendLength];
	rData = new char[sendLength];	

	randomizeData(sendLength);

	int tries = 0;
	
	while(tries < sendTry){
		if(procId == 0){
			randomizeData(sendLength);
			timeM1 = MPI_Wtime();
			MPI_Send(&data, sendLength, MPI_CHAR, 1, tries, MPI_COMM_WORLD);
			timeM2 = MPI_Wtime();
			MPI_Recv(&timeSlave, 1, MPI_DOUBLE, 1, tries, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			totalMasterTime += (timeM2 - timeM1);
			totalSlaveTime += timeSlave[0];
		}
		else{
			timeS1 = MPI_Wtime();
			MPI_Recv(&rData, sendLength, MPI_CHAR, 0, tries, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			timeS2 = MPI_Wtime();
			timeSlave[0] = timeS2 - timeS1;
			MPI_Send(&timeSlave, 1, MPI_DOUBLE, 0, tries, MPI_COMM_WORLD);
		}
		tries++;
		MPI_Barrier(MPI_COMM_WORLD);
	}

	totalMasterTime /= sendTry;
	totalSlaveTime /= sendTry;

	if(procId == 0){
		printf("Avg Master Time: %lf microseconds\nAvg Slave Time: %lf microseconds\n", totalMasterTime * 1000000, totalSlaveTime * 1000000);
	}

	MPI_Finalize();
}