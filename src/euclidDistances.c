#include <math.h>

__declspec(target(mic))
double euclidDistance(const double* point1, const double* point2, const int pointWidth){
	double result = 0;
	
	for(int i=0;i<pointWidth;++i){
		result+=(point1[i]-point2[i])*(point1[i]-point2[i]);
	}

	return sqrt(result);
}

__declspec(target(mic))
void euclidDistancesReference(const double* data, double* const distances, const int n, const int pointWidth){
	double point[pointWidth] __attribute__((aligned(64)));
	point[:]=data[0:pointWidth];
	
	for(int i=0;i<n;++i){
		distances[i] = euclidDistance(point, data+i*pointWidth, pointWidth);
	}
}
/*
// first version - inline euclidDistance function
__declspec(target(mic))
void euclidDistances(const double* data, double* const distances, const int n, const int pointWidth){
	double point[pointWidth] __attribute__((aligned(64)));
	point[:]=data[0:pointWidth];
	
	
	for(int i=0;i<n;++i){
		double result = 0;
		const double* restrict point2 = data+i*pointWidth;
		
		for(int j=0;j<pointWidth;++j){
			result+=(point[j]-point2[j])*(point[j]-point2[j]);
		}

		distances[i] = sqrt(result);
	}
}*/

/*
// second version - add inner loop
__declspec(target(mic))
void euclidDistances(const double* data, double* const distances, const int n, const int pointWidth){
	double point[pointWidth] __attribute__((aligned(64)));
	point[:]=data[0:pointWidth];
	
	const int vecLen = 16;
	for(int ii = 0; ii < n; ii+=vecLen){
		for(int i=ii; i<ii+vecLen; i++){
			double result = 0;
			const double* restrict point2 = data+i*pointWidth;
			
			for(int j=0;j<pointWidth;++j){
				result+=(point[j]-point2[j])*(point[j]-point2[j]);
			}

			distances[i] = sqrt(result);
		}
	}
}*/

/*
// third version - change inner loop to vector operations
__declspec(target(mic))
void euclidDistances(const double* data, double* const distances, const int n, const int pointWidth){
	double point[pointWidth] __attribute__((aligned(64)));
	point[:]=data[0:pointWidth];
	
	const int vecLen = 64;
	double result[vecLen] __attribute__((aligned(64)));
	double tmp[vecLen] __attribute__((aligned(64)));
	for(int ii = 0; ii < n; ii+=vecLen){
		result[:]=0;
		
		for(int j=0;j<pointWidth;++j){
			const double* restrict point2 = data+ii*pointWidth;
			// vecLen = 16, n=1L<<21L; pointWidth=4; time=0.159
			//tmp[:]=point[j];tmp[:]-=point2[j:vecLen:pointWidth];tmp[:]=tmp[:]*tmp[:];result[:]+=tmp[:];
			
			// vecLen = 16, n=1L<<21L; pointWidth=4; time=0.169
			//tmp[:]=point[j]-point2[j:vecLen:pointWidth];result[:]+=tmp[:]*tmp[:];
			
			// vecLen = 16, n=1L<<21L; pointWidth=4; time=0.158
			result[:]+=(point[j]-point2[j:vecLen:pointWidth])*(point[j]-point2[j:vecLen:pointWidth]);
		}
		
		distances[ii:vecLen] = sqrt(result[:]);
	}
}*/

// fourth version - add openmp to outer loop
__declspec(target(mic))
void euclidDistances(const double* data, double* const distances, const int n, const int pointWidth){
	double point[pointWidth] __attribute__((aligned(64)));
	point[:]=data[0:pointWidth];
	
	const int vecLen = 64;
	#pragma omp parallel 
	{
		double result[vecLen] __attribute__((aligned(64)));
		double tmp[vecLen] __attribute__((aligned(64)));
		#pragma omp for
		for(int ii = 0; ii < n; ii+=vecLen){
			result[:]=0;
			
			for(int j=0;j<pointWidth;++j){
				const double* restrict point2 = data+ii*pointWidth;
				result[:]+=(point[j]-point2[j:vecLen:pointWidth])*(point[j]-point2[j:vecLen:pointWidth]);
			}
			
			distances[ii:vecLen] = sqrt(result[:]);
		}
	}
}
