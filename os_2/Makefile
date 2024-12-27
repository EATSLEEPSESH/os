# Makefile

# Компилятор
CC = gcc

# Флаги компилятора
CFLAGS = -Wall -Wextra -Werror -pthread

# Имя исполняемого файла
TARGET = gauss

# Правило по умолчанию
all: $(TARGET)

# Правило для компиляции
$(TARGET): gauss.o
	$(CC) $(CFLAGS) -o $(TARGET) gauss.o

# Правило для создания объектных файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки
clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
