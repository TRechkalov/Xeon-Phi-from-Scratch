# Xeon Phi from Scratch

This project is created to make some examples of code for Intel Xeon Phi and some useful performance code too.

Project structure consist of src, include, test, build and extras. Extras contains additional files which may be useful as examples. I add there execution logs and vectorization reports.

# Histogram
First example is based on video from Colfax Research (http://research.colfaxinternational.com/). In this task we will generate number of people ages and make histogram about it.

Make target: histogram

Keywords: OpenMP, loop vectorization, MKL VSL, array alignment, offload mode.

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

Keywords: OpenMP, loop vectorization, array alignment, offload mode.

Files:

1. multiplicationTableMain.c - generates test data, controls offload calls

2. multiplicationTable.c - contains multiplication table function implementations 
