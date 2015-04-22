# Xeon Phi from Scratch

This project is created to make some examples of code for Intel Xeon Phi and some useful performance code too.

Project structure consist of src, include, test, build and extras. Extras contains additional files which may be useful as examples. I add there execution logs and vectorization reports.

# Useful links

1. Site of Colfax Research (http://research.colfaxinternational.com/) (ENG)
2. Nemniugin S. Meeting Xeon Phi - part 1,2 / Roboschool 2013 (http://roboschool.org/materials/roboschool2013) (RUS)
3. Nasonov A. Programming Xeon Phi in practice / Roboschool 2014 (http://roboschool.org/materials/roboschool2014) (RUS)
4. Arch D. Robison. SIMD Parallelism using Array Notation (https://software.intel.com/en-us/blogs/2010/09/03/simd-parallelism-using-array-notation) (ENG)

# Histogram
First example is based on video from Colfax Research (http://research.colfaxinternational.com/). In this task we will generate number of people ages and make histogram about it.

Make target: histogram

Keywords: OpenMP, loop vectorization, MKL VSL, array alignment, offload mode, float.

Minor info about MKL VSL is available here http://habrahabr.ru/post/215775/ (RUS).

Files:

1. histogramMain.c - generates test data, controls offload calls

2. histogram.c - contains histogram function implementations 

Information source: 

1. http://habrahabr.ru/post/246055/ (RUS)

2. http://www.youtube.com/watch?v=w3bSng5bmBc#t=477 (RUS)

3. http://www.youtube.com/watch?v=L_MxaSsbiDg (ENG)

This example is adopted for using on SUSU cluster.
Major differences from Colfax version:

1. Used offload mode instead native mode.

2. Used old style vec-report because of old compiler version. More information is here http://habrahabr.ru/company/intel/blog/222769/ (RUS).

3. In histogram.c presented all steps of histogram function modifications.

4. There are no final code with using parallel execution in videos. So OpenMP version is made by my own. It works not so good as in Colfax slides but it is finally faster then CPU :)

# Multiplication table
It is necessary to make multiplication table for specified numbers list. 

Make target: multi

Keywords: OpenMP, loop vectorization, array alignment, offload mode, int.

Files:

1. multiplicationTableMain.c - generates test data, controls offload calls

2. multiplicationTable.c - contains multiplication table function implementations 

# Euclid distances example
In this example I generate big array of points. Then I take first point and count distances from each point to first point. 

Make target: euclid

Keywords: OpenMP, loop vectorization, MKL VSL, array alignment, offload mode, double.

Files:

1. euclidDistancesMain.c - generates test data, controls offload calls

2. euclidDistances.c - contains Euclid distance function implementations 

# Minor examples

Added a couple of minor examples.

1. Example of using intrinics adopted from http://habrahabr.ru/company/intel/blog/205552/ (RUS)

2. Available memory test. This program allocates memory in infinite loop. Each memory chunk size increased for 32 megabytes.
