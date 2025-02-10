# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC
LDFLAGS = -ldl -lm
LIB_DIR = .
LIB_INTEGRAL = libintegral.so
LIB_DERIVATIVE = libderivative.so
INTEGRAL_SRC = integral.c
DERIVATIVE_SRC = derivative.c
TEST_PROGRAM_1 = test_program_1
TEST_PROGRAM_1_SRC = test_program_1.c
TEST_PROGRAM_2 = test_program_2
TEST_PROGRAM_2_SRC = test_program_2.c

# Default target
all: $(LIB_INTEGRAL) $(LIB_DERIVATIVE) $(TEST_PROGRAM_1) $(TEST_PROGRAM_2)

# Rule for compiling the integral library
$(LIB_INTEGRAL): $(INTEGRAL_SRC)
	$(CC) $(CFLAGS) -shared -o $(LIB_INTEGRAL) $(INTEGRAL_SRC) $(LDFLAGS)

# Rule for compiling the derivative library
$(LIB_DERIVATIVE): $(DERIVATIVE_SRC)
	$(CC) $(CFLAGS) -shared -o $(LIB_DERIVATIVE) $(DERIVATIVE_SRC) $(LDFLAGS)

# Rule for compiling test program 1
$(TEST_PROGRAM_1): $(TEST_PROGRAM_1_SRC) $(LIB_INTEGRAL) $(LIB_DERIVATIVE)
	$(CC) $(CFLAGS) -o $(TEST_PROGRAM_1) $(TEST_PROGRAM_1_SRC) -L$(LIB_DIR) -lintegral -lderivative $(LDFLAGS)

# Rule for compiling test program 2
$(TEST_PROGRAM_2): $(TEST_PROGRAM_2_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TEST_PROGRAM_2) $(TEST_PROGRAM_2_SRC)

# Rule for running test program 1
run_test_program_1: $(TEST_PROGRAM_1)
	LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH ./$(TEST_PROGRAM_1)

# Rule for running test program 2
run_test_program_2: $(TEST_PROGRAM_2)
	LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH ./$(TEST_PROGRAM_2)

# Rule for cleaning up
clean:
	rm -f $(LIB_INTEGRAL) $(LIB_DERIVATIVE) $(TEST_PROGRAM_1) $(TEST_PROGRAM_2)

.PHONY: all clean run_test_program_1 run_test_program_2
