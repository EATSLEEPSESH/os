# Makefile

# Компилятор
CC = gcc

# Флаги компилятора
CFLAGS = -Wall -Wextra -Werror

# Имена исходных файлов
SRCS = parent.c child.c

# Имена объектных файлов
OBJS = $(SRCS:.c=.o)

# Имена исполняемых файлов
TARGETS = parent child

# Правило по умолчанию
all: $(TARGETS)

# Правило для компиляции родительского процесса
parent: parent.o
	$(CC) $(CFLAGS) -o parent parent.o

# Правило для компиляции дочернего процесса
child: child.o
	$(CC) $(CFLAGS) -o child child.o

# Правило для создания объектных файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки
clean:
	rm -f $(OBJS) $(TARGETS)

.PHONY: all clean
