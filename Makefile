CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LIB_NAME = libmemory_manager.so

# Libraries
LIBS = -lm -pthread

# Source and Object Files
SRC = memory_manager.c
OBJ = $(SRC:.c=.o)

# Default target
all: mmanager list test_mmanager test_list

# Rule to create the dynamic library
$(LIB_NAME): $(OBJ)
	$(CC) -shared -o $@ $(OBJ)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the memory manager
mmanager: $(LIB_NAME)

# Build the linked list
list: linked_list.o

# Test target to run the memory manager test program
test_mmanager: test_memory_manager.o $(LIB_NAME)
	$(CC) -o test_memory_manager test_memory_manager.o -L. -lmemory_manager $(LIBS)

# Test target to run the linked list test program
test_list: test_linked_list.o linked_list.o $(LIB_NAME)
	$(CC) -o test_linked_list test_linked_list.o linked_list.o -L. -lmemory_manager $(LIBS)

# Compile test_memory_manager.o with CFLAGS if needed
test_memory_manager.o: test_memory_manager.c
	$(CC) $(CFLAGS) -c test_memory_manager.c -o test_memory_manager.o

# Compile test_linked_list.o with CFLAGS if needed
test_linked_list.o: test_linked_list.c
	$(CC) $(CFLAGS) -c test_linked_list.c -o test_linked_list.o

# Run tests
run_tests: run_test_mmanager run_test_list

# Run test cases for the memory manager
run_test_mmanager:
	./test_memory_manager

# Run test cases for the linked list
run_test_list:
	./test_linked_list

# Clean target to clean up build files
clean:
	rm -f *.o $(LIB_NAME) test_memory_manager test_linked_list linked_list.o

# Phony Targets
.PHONY: all mmanager list test_mmanager test_list run_tests run_test_mmanager run_test_list clean
