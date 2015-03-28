#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <mkl_vsl.h>

double euclidDistance(const double* point1, const double* point2, const int pointWidth){
	double result = 0;
	/*for(int i=0;i<pointWidth;++i){
		printf("%f ", point1[i]);
	}
	
	printf("|");
	
	for(int i=0;i<pointWidth;++i){
		printf("%f ", point2[i]);
	}*/
	
	for(int i=0;i<pointWidth;++i){
		result+=(point1[i]-point2[i])*(point1[i]-point2[i]);
	}
	
	//result = sqrt(result);
	//printf("(%f)\n", result);
	//return result;
	return sqrt(result);
	//printf("(%f)\n", sqrtf(result));
	
	/*for(int i=0;i<pointWidth;++i){
		result+=(point1[i]-point2[i])*(point1[i]-point2[i]);
	}*/
	
	//return sqrtf(result);
}

void euclidDistancesReference(const double* data, double* const distances, const int n, const int pointWidth){
	double point[pointWidth] __attribute__((aligned(64)));
	point[:]=data[0:pointWidth];
	
	printf("\n");
	
	for(int i=0;i<n;++i){
		distances[i] = euclidDistance(point, data+i*pointWidth, pointWidth);
		/*for(int i=0;i<n;++i){
			printf("%f ", distances[i]);
		}
		printf("\n");*/
	}

	for(int i=0;i<n;++i){
		printf("%f ", distances[i]);
	}
}
/*
__declspec(target(mic))
void histogramReference(const double* age, int* const hist, const int n, const double groupWidth);

__declspec(target(mic))
void histogram(const double* age, int* const hist, const int n, const double groupWidth, const int m);

__declspec(target(mic))
void test(const double* age, int* const refGroup, const int n, const double groupWidth, const int m, const int nTrials);*/

int main(int argv, char* argc[]){
	#pragma offload_transfer target(mic)

	//const size_t n=1L<<30L;
	const size_t n=1L<<3L;
	const size_t pointWidth=2;
	const double maxValue = 99.999;
	//const double groupWidth = 20.0f;
	//const size_t m = (size_t)floorf(maxAge/groupWidth + 0.1f);
	//const int nTrials = 10;
	const int nTrials = 4;
	
	double* data = (double*) _mm_malloc(sizeof(double)*n*pointWidth, 64);
	
	double refGroup[n]; // reference distances
	
	printf("Initialization...");fflush(0); // stdout
	
	VSLStreamStatePtr rnStream;
	vslNewStream(&rnStream, VSL_BRNG_MT19937,1);
	vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rnStream, n*pointWidth, data, 0.0, maxValue);
	
	/*for(int i=0;i<n*pointWidth;++i){
		printf("%f ", data[i]);
	}*/

	
	refGroup[:]=0;
	euclidDistancesReference(data, refGroup, n, pointWidth);
	
	for(int i=0;i<n;++i){
		printf("%f ", refGroup[i]);
	}printf("\n");
	
	printf("complete.\n");fflush(0); // stdout
	/*
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
	}*/
	
	_mm_free(data);
}

/*
void test(const double* age, int* const refGroup, const int n, const double groupWidth, const int m, const int nTrials){
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
}*/
