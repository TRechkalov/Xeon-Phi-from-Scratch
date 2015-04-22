/* source from http://habrahabr.ru/company/intel/blog/205552/*/

#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <mkl_vsl.h>
#include <immintrin.h>

__declspec(target(mic))
void intrinsicTest(){
	const int size = 64;
	double A[size], B[size], C[size];
	A[:]=3;
	B[:]=2;
	C[:]=0;
	
	#ifdef __MIC__
		for (int i = 0; i < size; i += 8) {
			__m512d a = _mm512_load_pd(&A[i]);
			__m512d b = _mm512_load_pd(&B[i]);
			__m512d c = _mm512_add_pd(a, b);
			_mm512_store_pd(&C[i], c);
		}
	#else
		for (int i = 0; i < size; i += 4) {
			__m256d a = _mm256_load_pd(&A[i]);
			__m256d b = _mm256_load_pd(&B[i]);
			__m256d c = _mm256_add_pd(a, b);
			//_mm256_store_pd(C+i, c); // get here error with unknown reason
		}
	#endif
	
	for(int i=0;i<size;++i){
		printf("%.3f ", C[i]);
	}
	
	printf("\n");fflush(0);
}

int main(int argv, char* argc[]){
	#pragma offload_transfer target(mic)
	
	printf("Hello, world!\n");
	
	intrinsicTest();
	
	#pragma offload target(mic)
	{
		intrinsicTest();
	}

}


