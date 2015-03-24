
// age - array of people ages, age is from {0 ,... , 100}
// n - size of age, n ~ 10 mln
//// m - number of groups in histogram
// groupWidth - 20 years

// 256 bit - vector register in Xeon
// 512 bit - vector register in Xeon Phi
// 4 Byte per float => 8 floats in vector register of Xeon
// -------||------- => 16 floats in vector register of Xeon Phi

// first version
__declspec(target(mic))
void histogramReference(const float* age, int* const hist, const int n, const float groupWidth){
	for(int i=0; i<n; i++){
		const int j = (int)(age[i]/groupWidth); // not data dependency, so can be vectorized
		hist[j]++; // has data dependency from prev step, so it is not vectorizable
	}
}

/*
// first step - replace division by multiplication
__declspec(target(mic))
void histogram(const float* age, int* const hist, const int n, const float groupWidth){
	const float invGroupWidth = 1.0f/groupWidth; // replacing division by multiplication
	for(int i=0; i<n; i++){
		const int j = (int)(age[i]*invGroupWidth); // not data dependency, so can be vectorized
		hist[j]++; // has data dependency from prev step, so it is not vectorizable
	}
}*/

/*
// second step - extract inner loop for vectorization
__declspec(target(mic))
void histogram(const float* age, int* const hist, const int n, const float groupWidth){
	const float invGroupWidth = 1.0f/groupWidth; // replacing division by multiplication
	const int vecLen = 16;
	for(int ii = 0; ii < n; ii+=vecLen){
		for(int i=ii; i<ii+vecLen; i++){
			const int j = (int)(age[i]*invGroupWidth); // not data dependency, so can be vectorized
			hist[j]++; // has data dependency from prev step, so it is not vectorizable
		}
	}
}*/

/*
// final version
// third step - split inner loop into two loops. First loop will be vectorizable 
__declspec(target(mic))
void histogram(const float* age, int* const hist, const int n, const float groupWidth, const int m){
	const int vecLen = 16; // number of floats in vector register, may be greater and divisible by 16. Tests showed 16 is optimal (from authors)
	const float invGroupWidth = 1.0f/groupWidth; // replacing division by multiplication
	
	for(int ii = 0; ii < n; ii+=vecLen){ // external loop by data segments
		int histIdx[vecLen] __attribute__((aligned(64)));
		#pragma vector aligned
		for(int i=ii; i<ii+vecLen; i++){ // internal loop process segments, this loop is vectorized
			histIdx[i-ii] = (int)(age[i] * invGroupWidth);
		}
		for(int c = 0;c<vecLen;c++){ // internal loop process segments, this loop is not vectorized
			hist[histIdx[c]]++; // has data dependency from prev step, so it is not vectorizable
		}
	}
}*/

// fourth step - try add openmp
__declspec(target(mic))
void histogram(const float* age, int* const hist, const int n, const float groupWidth, const int m){
	const int vecLen = 16; // number of floats in vector register, may be greater and divisible by 16. Tests showed 16 is optimal (from authors)
	const float invGroupWidth = 1.0f/groupWidth; // replacing division by multiplication
	
	#pragma omp parallel 
	{
		int histLocal[m] __attribute__((aligned(64)));
		histLocal[:]=0;
		
		#pragma omp for
		for(int ii = 0; ii < n; ii+=vecLen){ // external loop by data segments
			int histIdx[vecLen] __attribute__((aligned(64)));
			
			#pragma vector aligned
			for(int i=ii; i<ii+vecLen; i++){ // internal loop process segments, this loop is vectorized
				histIdx[i-ii] = (int)(age[i] * invGroupWidth);
			}
			for(int c = 0;c<vecLen;c++){ // internal loop process segments, this loop is not vectorized
				histLocal[histIdx[c]]++; // has data dependency from prev step, so it is not vectorizable
			}
		}
		
		#pragma omp critical
		{
			for(int i=0;i<m;++i){
				hist[i]+=histLocal[i];
			}
		}
	}
}