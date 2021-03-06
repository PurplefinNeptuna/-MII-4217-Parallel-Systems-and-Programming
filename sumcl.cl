kernel void simple_add(global const int* A, global const uint* B, global int* C){
	uint workid = get_global_id(0);
	if(workid >= (*B))
		return;
	int worksize = (*B) / get_global_size(0);
	int startpos = workid * worksize;
	int endpos = startpos + worksize;
	int localsum = 0;
	for(int i = startpos; i < endpos; i++){
		localsum += A[i];
	}
	C[workid] = localsum;
	printf("I got %d from work %d\n", localsum, workid);
}