
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <mkl_vsl.h>
#include <immintrin.h>

__declspec(target(mic))
void memoryTest(){
	
	double *data;
	int byteChunk = 1L<<25L;
	for(int counter = 1;;counter++){
		data = (double*)_mm_malloc(byteChunk*counter, 64);
		data[byteChunk*counter/sizeof(double)-1]=0;
		printf("%dMb \n", counter*32);fflush(0);
		_mm_free(data);
	}
}

int main(int argv, char* argc[]){
	#pragma offload_transfer target(mic)
	
	printf("Hello, world!\n");
	
	//memoryTest();
	
	#pragma offload target(mic)
	{
		memoryTest();
	}
}


