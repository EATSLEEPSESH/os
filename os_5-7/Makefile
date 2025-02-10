# Define variables
CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = -lczmq

# Source files
CONTROL_NODE_SRC = Control_node.c
COUNTING_NODE_SRC = Counting_node.c

# Output executables
CONTROL_NODE_EXE = control_node
COUNTING_NODE_EXE = counting_node

# Default target
all: $(CONTROL_NODE_EXE) $(COUNTING_NODE_EXE)

# Rule for compiling control_node
$(CONTROL_NODE_EXE): $(CONTROL_NODE_SRC)
	$(CC) $(CFLAGS) -o $(CONTROL_NODE_EXE) $(CONTROL_NODE_SRC) $(LDFLAGS)

# Rule for compiling counting_node
$(COUNTING_NODE_EXE): $(COUNTING_NODE_SRC)
	$(CC) $(CFLAGS) -o $(COUNTING_NODE_EXE) $(COUNTING_NODE_SRC) $(LDFLAGS)

# Rule for cleaning up
clean:
	rm -f $(CONTROL_NODE_EXE) $(COUNTING_NODE_EXE)

.PHONY: all clean
