# Makefile for CP431-A1 - Group 10
# Prime Gaps Finder

# Compiler and flags
CC = mpicc
CFLAGS = -O3 -Wall
LIBS = -lgmp

TARGET = A1_grp10 # target exe
SOURCE = A1_grp10.c # source

all: $(TARGET)
	@echo "Compilation successful"
	@echo "Run benchmarks with `make test` or `make help` for more options"

$(TARGET): $(SOURCE)  # Compile
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

test: $(TARGET) #  test
	@echo "Test with 4 processors (1 billion)..."
	mpirun -np 4 ./$(TARGET) 1000000000


clean-results: #Clean results
	rm -f prime_gaps_results.txt

# Run full benchmark: 1 billion with 2-8 procs, then 1 trillion with 8 procs
benchmark: $(TARGET) clean-results
	@echo ""
	@echo "Part 1: Testing 1 Billion with 2-8 processors"
	@echo "--------------------------------------"
	@mpirun -np 2 ./$(TARGET) 1000000000
	@mpirun -np 3 ./$(TARGET) 1000000000
	@mpirun -np 4 ./$(TARGET) 1000000000
	@mpirun -np 5 ./$(TARGET) 1000000000
	@mpirun -np 6 ./$(TARGET) 1000000000
	@mpirun -np 7 ./$(TARGET) 1000000000
	@mpirun -np 8 ./$(TARGET) 1000000000
	@echo ""
	@echo "Part 2: Testing 1 Trillion with 8 processors"
	@echo "--------------------------------------"
	@mpirun -np 8 ./$(TARGET) 1000000000000
	@echo ""
	@echo "========================================"
	@echo "Benchmark Complete!"
	@echo "Results saved to: prime_gaps_results.txt"
	@echo "========================================"

# Clean up
clean:
	rm -f $(TARGET) *.o prime_gaps_results.txt

# Help msg
help:
	@echo "Available targets:"
	@echo ""
	@echo "  make              - Compile the program"
	@echo "  make test         - Test with 4 processors (1 billion)"
	@echo ""
	@echo "  make benchmark    - Run FULL benchmark:"
	@echo "                      • 1 billion with 2-8 processors"
	@echo "                      • 1 trillion with 8 processors"
	@echo "                      (DONT RUN ON TEACH TERMINAL)"
	@echo ""
	@echo "  make clean        - Remove compiled files and results"
	@echo "  make help         - Show this help message"
	@echo ""