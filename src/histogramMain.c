#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <mkl_vsl.h>

__declspec(target(mic))
void histogramReference(const float* age, int* const hist, const int n, const float groupWidth);

__declspec(target(mic))
void histogram(const float* age, int* const hist, const int n, const float groupWidth, const int m);

__declspec(target(mic))
void test(const float* age, int* const refGroup, const int n, const float groupWidth, const int m, const int nTrials);

int main(int argv, char* argc[]){
	#pragma offload_transfer target(mic)

	const size_t n=1L<<30L;
	//const size_t n=1L<<20L;
	const float maxAge = 99.999f;
	const float groupWidth = 20.0f;
	const size_t m = (size_t)floorf(maxAge/groupWidth + 0.1f);
	const int nTrials = 10;
	//const int nTrials = 4;
	
	float* age = (float*) _mm_malloc(sizeof(int)*n, 64);
	
	int refGroup[m];
	
	printf("Initialization...");fflush(0); // stdout
	
	VSLStreamStatePtr rnStream;
	vslNewStream(&rnStream, VSL_BRNG_MT19937,1);
	vsRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rnStream, n, age, 0.0f, maxAge);
	for(int i=0;i<n;++i){
		age[i] = age[i]*age[i]/maxAge;
	}
	
	refGroup[:]=0;
	histogramReference(age, refGroup, n, groupWidth);
	printf("complete.\n");fflush(0); // stdout
	
	test(age, refGroup, n, groupWidth, m ,nTrials);
	#pragma offload target(mic) \
					in(age: length(n)) \
					in(refGroup: length(m)) \
					in(n) \
					in(groupWidth) \
					in(m) \
					in(nTrials) 
	{
		test(age, refGroup, n, groupWidth, m ,nTrials);
	}
	
	_mm_free(age);
}

void test(const float* age, int* const refGroup, const int n, const float groupWidth, const int m, const int nTrials){
	int group[m];
	for(int t=0;t<nTrials;++t){
		group[:]=0;
		printf("Iteration %d...", t);
		const double t0 = omp_get_wtime();
		histogram(age, group, n, groupWidth, m);
		const double t1 = omp_get_wtime();
		
		for(int i=0;i<m;++i){
			if(fabs((double)(refGroup[i] - group[i])) > 1e-4*fabs((double)refGroup[i] + group[i])){
				printf("Result is incorrect!\n");
				for(int i=0;i<m;++i){
					printf(" (%d vs %d)", group[i], refGroup[i]);
				}
			}
		}
		printf(" time: %.3f sec\n", t1-t0);
		printf(" Performance: %.3f billion numbers / sec \n", n/(t1-t0)*1.0e-9);
		
		printf("Result: ");
		for(int i=0;i<m;++i){
			printf("\t%d", group[i]);
		}
		printf("\n");
		fflush(0);
	}
}
