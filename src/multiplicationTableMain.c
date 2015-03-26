#include <stdio.h>
#include <omp.h>
#include <math.h>

__declspec(target(mic))
void multiplicationTableReference(const int* data, int* const refTable, const int n);

__declspec(target(mic))
void multiplicationTable(const int* data, int* const refTable, const int n);

__declspec(target(mic))
void test(const int* data, int* const refTable, const int n, const int nTrials);


int main(int argv, char* argc[]){
	#pragma offload_transfer target(mic)

	//const size_t n=1L<<30L;
	const size_t n=1L<<15L; // size of data
	const size_t m=n*n; // size of table
	const int nTrials = 8;
	printf("%d\n",n);
	
	int* data = (int*) _mm_malloc(sizeof(int)*n, 64);
	int* refTable = (int*) _mm_malloc(sizeof(int)*m, 64);
	
	
	printf("Initialization...");fflush(0); // stdout
	
	for(int i=0;i<n;++i){
		data[i] = i;
	}
	
	multiplicationTableReference(data, refTable, n);
	/*for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			printf("%d ", refTable[i*n+j]);
		}
		printf("\n");
	}*/
	printf("complete.\n");fflush(0); // stdout
	
	test(data, refTable, n, nTrials);
	#pragma offload target(mic) \
					in(data: length(n)) \
					in(n) \
					in(nTrials) \
					in(refTable: length(0)) 
	{
		test(data, refTable, n, nTrials);
	}
	
	_mm_free(data);
	_mm_free(refTable);
}

void test(const int* data, int* const refTable, const int n, const int nTrials){
	const int m = n*n;
	int* table = (int*) _mm_malloc(sizeof(int)*m, 64);
	for(int t=0;t<nTrials;++t){
		printf("Iteration %d...", t);
		const double t0 = omp_get_wtime();
		multiplicationTable(data, table, n);
		const double t1 = omp_get_wtime();
		
		/*
		for(int i=0;i<m;++i){
			if(refTable[i] != table[i]){
				printf("Result is incorrect!\n");
				for(int i=0;i<m;++i){
					printf(" (%d vs %d)", table[i], refTable[i]);
				}
				break;
			}
		}*/
		printf(" time: %.3f sec\n", t1-t0);
		printf(" Performance: %.3f billion numbers / sec \n", m/(t1-t0)*1.0e-9);
		
		fflush(0);
	}
	_mm_free(table);
}
