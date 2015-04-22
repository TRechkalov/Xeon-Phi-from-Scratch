# compile options
CC = icc
CFLAGS = -mkl -O3 -openmp -sox -std=c99 -Wall 
#-vec-report6 - add this to get vectorization report. vec6 shows information about success. vec5 gives information only about vectorization fails.
# In original video used ICC 15 so this options is for newest version of Intel Compiler.
# -opt-report-phase:vec -opt-report-stdout  

# Base paths
BUILD_PATH=build
INCLUDE_PATH=include
SRC_PATH=src
TEST_PATH=test
EXTRAS_PATH=extras

HISTOGRAM = $(BUILD_PATH)/histogramMain.o $(BUILD_PATH)/histogram.o 
MULTIPLICATION_TABLE = $(BUILD_PATH)/multiplicationTableMain.o $(BUILD_PATH)/multiplicationTable.o
EUCLID_DISTANCES = $(BUILD_PATH)/euclidDistancesMain.o $(BUILD_PATH)/euclidDistances.o 
INTRINSICS = $(BUILD_PATH)/intrinsics.o
MEMORY_SIZE = $(BUILD_PATH)/memorySize.o

all: histogram 

$(BUILD_PATH)/histogramMain.o: $(SRC_PATH)/histogramMain.c
$(BUILD_PATH)/histogram.o: $(SRC_PATH)/histogram.c
$(BUILD_PATH)/multiplicationTableMain.o: $(SRC_PATH)/multiplicationTableMain.c
$(BUILD_PATH)/multiplicationTable.o: $(SRC_PATH)/multiplicationTable.c
$(BUILD_PATH)/euclidDistancesMain.o: $(SRC_PATH)/euclidDistancesMain.c
$(BUILD_PATH)/euclidDistances.o: $(SRC_PATH)/euclidDistances.c
$(BUILD_PATH)/intrinsics.o: $(SRC_PATH)/intrinsics.c
$(BUILD_PATH)/memorySize.o: $(SRC_PATH)/memorySize.c

# add this if you need vectorization reports in files
#2> $(EXTRAS_PATH)/histogram/$<.txt
$(HISTOGRAM) $(MULTIPLICATION_TABLE) $(EUCLID_DISTANCES) $(INTRINSICS) $(MEMORY_SIZE):
	$(CC) $(CFLAGS) -c $< -o $@ 
	
memorySize: clean $(MEMORY_SIZE)
	$(CC) $(CFLAGS) -o $(BUILD_PATH)/memorySizeExecutive $(MEMORY_SIZE)
	./$(BUILD_PATH)/memorySizeExecutive
	
intrinsics: clean $(INTRINSICS)
	$(CC) $(CFLAGS) -o $(BUILD_PATH)/intrinsicsExecutive $(INTRINSICS)
	./$(BUILD_PATH)/intrinsicsExecutive
	
euclid: clean $(EUCLID_DISTANCES)
	$(CC) $(CFLAGS) -o $(BUILD_PATH)/euclidDistancesExecutive $(EUCLID_DISTANCES)
	./$(BUILD_PATH)/euclidDistancesExecutive
	
multi: clean $(MULTIPLICATION_TABLE)
	$(CC) $(CFLAGS) -o $(BUILD_PATH)/multiplicationTableExecutive $(MULTIPLICATION_TABLE)
	./$(BUILD_PATH)/multiplicationTableExecutive
	
histogram: clean $(HISTOGRAM)
	$(CC) $(CFLAGS) -o $(BUILD_PATH)/histogramExecutive $(HISTOGRAM)
	./$(BUILD_PATH)/histogramExecutive
	
clean:
	rm -f $(BUILD_PATH)/*.o histogramExecutive 
	

	#.SUFFIXES: .o .c .oMIC
	
# MIC native directives, I can't build MIC executive file without errors so I made tests in offload mode.
#MICOBJECTS = histogramMain.oMIC histogram.oMIC

#.c.oMIC:
#	$(CC) -c -mmic $(CFLAGS) -o "$@" "$<"
	
#runmeMIC: $(MICOBJECTS)
#	$(CC) $(CFLAGS) -o runmeMIC $(MICOBJECTS)