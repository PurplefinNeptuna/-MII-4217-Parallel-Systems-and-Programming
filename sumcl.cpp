#include <CL/cl.hpp>
#include <bits/stdc++.h>
using namespace std;

int main() {
	ifstream ifile;
	stringstream sstr;
	string kernel_code;
	vector<int> numdata;
	int x;

	ifile.open("rand_data.txt");
	while (ifile >> x) {
		numdata.push_back(x);
	}
	ifile.close();
	int numsize = numdata.size();

	ifile.open("sumcl.cl");
	sstr << ifile.rdbuf();
	ifile.close();
	kernel_code = sstr.str();

	cl::Context context(CL_DEVICE_TYPE_DEFAULT);
	cl::Program program(context, kernel_code, true);

	vector<int> B(1);
	B[0] = numsize;
	vector<int> C(4);

	// create buffers on the device
	cl::Buffer buffer_A(context, numdata.begin(), numdata.end(), true);
	cl::Buffer buffer_B(context, B.begin(), B.end(), true);
	cl::Buffer buffer_C(context, C.begin(), C.end(), false);

	//create queue to which we will push commands for the device.
	cl::CommandQueue queue(context);
	//write to buffer
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 100, &numdata[0]);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int), &B[0]);
	cl::EnqueueArgs eargs(queue, cl::NDRange(4));

	//run the kernel
	cl::make_kernel simple_add = cl::make_kernel<cl::Buffer&, cl::Buffer&, cl::Buffer&>(program, "simple_add");
	simple_add(eargs, buffer_A, buffer_B, buffer_C).wait();

	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 4, &C[0]);

	//calculate final sum
	int result = 0;
	for (int i = 0; i < 4; i++) {
		result += C[i];
	}

	printf("Total sum: %d\n", result);
}