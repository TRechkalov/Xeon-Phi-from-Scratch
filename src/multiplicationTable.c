
void multiplicationTableReference(const int* data, int* const table, const int n){
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			table[i*n+j]=i*j;
		}
	}
}
/*
// first version
__declspec(target(mic))
void multiplicationTable(const int* data, int* const table, const int n){
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			table[i*n+j]=i*j;
		}
	}
}*/
/*
// second version - split outer loop
__declspec(target(mic))
void multiplicationTable(const int* data, int* const table, const int n){
	const int vecLen = 16;
	for(int ii = 0; ii < n; ii+=vecLen){
		for(int i=ii; i<ii+vecLen; i++){
			for(int j=0; j<n; j++){
				table[i*n+j]=i*j;
			}
		}
	}
}*/

/*
// third version - split inner loop
__declspec(target(mic))
void multiplicationTable(const int* data, int* const table, const int n){
	const int vecLen = 16;
	for(int ii = 0; ii < n; ii+=vecLen){
		for(int i=ii; i<ii+vecLen; i++){
			for(int jj = 0; jj < n; jj+=vecLen){
				for(int j=jj; j<jj+vecLen; j++){
					table[i*n+j]=i*j;
				}
			}
		}
	}
}*/
/*
// fourth version - separate external and internal loops
__declspec(target(mic))
void multiplicationTable(const int* data, int* const table, const int n){
	const int vecLen = 128;
	for(int ii = 0; ii < n; ii+=vecLen){
		for(int jj = 0; jj < n; jj+=vecLen){
			for(int i=ii; i<ii+vecLen; i++){
				for(int j=jj; j<jj+vecLen; j++){
					table[i*n+j]=i*j;
				}
			}
		}
	}
}*/

/*
// fifth version - add openmp
// n=2^13, MIC time= 0.011 , CPU time=0.015 
// n=2^15, MIC time= 0.120 , CPU time=0.256 
__declspec(target(mic))
void multiplicationTable(const int* data, int* const table, const int n){
	const int vecLen = 512;
	#pragma omp parallel for
	for(int ii = 0; ii < n; ii+=vecLen){
		for(int jj = 0; jj < n; jj+=vecLen){
			for(int i=ii; i<ii+vecLen; i++){
				for(int j=jj; j<jj+vecLen; j++){
					table[i*n+j]=i*j;
				}
			}
		}
	}
}*/


// alternative - add openmp to first version
// n=2^13, MIC time=0.004, CPU time=0.04
// n=2^15, MIC time=0.060, CPU time=0.244
__declspec(target(mic))
void multiplicationTable(const int* data, int* const table, const int n){
	#pragma omp parallel for
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			table[i*n+j]=i*j;
		}
	}
}
