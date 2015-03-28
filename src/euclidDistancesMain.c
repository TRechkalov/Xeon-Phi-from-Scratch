#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <mkl_vsl.h>

__declspec(target(mic))
void euclidDistancesReference(const double* data, double* const distances, const int n, const int pointWidth);

__declspec(target(mic))
void euclidDistances(const double* data, double* const distances, const int n, const int pointWidth);

__declspec(target(mic))
void test(const double* data, double* const refDistances, const int n, const int pointWidth, const int nTrials);

int main(int argv, char* argc[]){
	#pragma offload_transfer target(mic)

	const size_t n=1L<<26L;
	//const size_t n=1L<<24L;
	const size_t pointWidth=3;
	const double maxValue = 99.999;
	const int nTrials = 10;
	//const int nTrials = 4;
	
	double* data = (double*) _mm_malloc(sizeof(double)*n*pointWidth, 64);
	
	//double refDistances[n] __attribute__((aligned(64))); // reference distances
	double* refDistances = (double*) _mm_malloc(sizeof(double)*n, 64); // reference distances
	
	printf("Initialization...");fflush(0); // stdout
	
	VSLStreamStatePtr rnStream;
	vslNewStream(&rnStream, VSL_BRNG_MT19937,1);
	vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rnStream, n*pointWidth, data, 0.0, maxValue);
	
	/*for(int i=0;i<n*pointWidth;++i){
		printf("%f ", data[i]);
	}*/
	euclidDistancesReference(data, refDistances, n, pointWidth);
	/*
	for(int i=0;i<n;++i){
		printf("%f ", refDistances[i]);
	}printf("\n");*/
	
	printf("complete.\n");fflush(0); // stdout
	
	test(data, refDistances, n, pointWidth ,nTrials);
	#pragma offload target(mic) \
					in(data: length(n*pointWidth)) \
					in(refDistances: length(n)) \
					in(n) \
					in(pointWidth) \
					in(nTrials) 
	{
		test(data, refDistances, n, pointWidth ,nTrials);
	}
	
	_mm_free(data);
	_mm_free(refDistances);
}


void test(const double* data, double* const refDistances, const int n, const int pointWidth, const int nTrials){
	double* distances = (double*) _mm_malloc(sizeof(double)*n, 64);
	for(int t=0;t<nTrials;++t){
		for(int i=0;i<n;++i){
			distances[i]=0;
		}
		printf("Iteration %d...", t);
		const double t0 = omp_get_wtime();
		euclidDistances(data, distances, n, pointWidth);
		const double t1 = omp_get_wtime();
		
		for(int i=0;i<n;++i){
			if(fabs(refDistances[i] - distances[i]) > 1e-4*fabs(refDistances[i] + distances[i])){
				printf("Result is incorrect!\n");
				/*for(int j=0;j<n;++j){
					printf(" (%d vs %d)", distances[j], refDistances[j]);
				}*/
				break;
			}
		}
		printf(" time: %.3f sec\n", t1-t0);
		printf(" Performance: %.3f billion numbers / sec \n", n*(pointWidth+1)/(t1-t0)*1.0e-9);
		
		/*for(int i=n-8;i<n;++i){
			printf("%f ", distances[i]);
		}
		printf("\n");*/
		/*
		printf("Result: ");
		for(int i=0;i<n;++i){
			printf("\t%d", distances[i]);
		}
		printf("\n");*/
		fflush(0);
	}
	_mm_free(distances);
}
