#include <CL/cl.hpp>
#include <bits/stdc++.h>
using namespace std;

int main() {
	int processingcount = 4;
	ifstream ifile;
	stringstream sstr;
	string kernel_code;
	vector<int> numdata;
	int x;

	//read file for sum
	ifile.open("rand_data.txt");
	while (ifile >> x) {
		numdata.push_back(x);
	}
	ifile.close();
	uint32_t numsize = numdata.size();

	//read file for kernel code
	ifile.open("sumcl.cl");
	sstr << ifile.rdbuf();
	ifile.close();
	kernel_code = sstr.str();

	//let use default device for context and program
	cl::Context context(CL_DEVICE_TYPE_DEFAULT);
	cl::Program program(context, kernel_code, true);

	//c for holding each worker's sum
	vector<int> C(processingcount);

	// create buffers on the device
	cl::Buffer buffer_A(context, numdata.begin(), numdata.end(), true);
	cl::Buffer buffer_B(context, CL_MEM_READ_ONLY, sizeof(uint32_t));
	cl::Buffer buffer_C(context, C.begin(), C.end(), false);

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context);

	//write to buffer
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 100, &numdata[0]);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(uint32_t), &numsize);

	//start queue for (processingcount) work groups, each has 1 units
	cl::EnqueueArgs eargs(queue, cl::NDRange(processingcount));

	//run the kernel
	cl::make_kernel simple_add = cl::make_kernel<cl::Buffer&, cl::Buffer&, cl::Buffer&>(program, "simple_add");
	simple_add(eargs, buffer_A, buffer_B, buffer_C).wait();

	//read from buffer
	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * processingcount, &C[0]);

	//calculate final sum
	int result = 0;
	for (int i = 0; i < processingcount; i++) {
		result += C[i];
	}

	printf("Total sum: %d\n", result);
}