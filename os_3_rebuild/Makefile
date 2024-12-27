# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
LDFLAGS = -lm

# Source files
CHILD_SRC = child.c
PARENT_SRC = parent.c

# Output binaries
CHILD_BIN = child
PARENT_BIN = parent

# Default target
all: $(CHILD_BIN) $(PARENT_BIN)

# Rule for compiling the child program
$(CHILD_BIN): $(CHILD_SRC)
	$(CC) $(CFLAGS) -o $(CHILD_BIN) $(CHILD_SRC) $(LDFLAGS)

# Rule for compiling the parent program
$(PARENT_BIN): $(PARENT_SRC)
	$(CC) $(CFLAGS) -o $(PARENT_BIN) $(PARENT_SRC)

# Rule for cleaning up
clean:
	rm -f $(CHILD_BIN) $(PARENT_BIN)

.PHONY: all clean
