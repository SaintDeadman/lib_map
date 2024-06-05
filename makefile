# Имя исполняемого файла
TARGET = main

# Компилятор и опции
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Папка для объектных файлов
OBJDIR = obj
SRCDIR = map_lib

# Поиск всех .c файлов в директории map_lib и их соответствующих .o файлов в obj
SRCFILES = $(wildcard $(SRCDIR)/*.c) main.c
OBJFILES = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRCFILES)))

# Цель по умолчанию: сборка всего проекта
all: $(TARGET)

# Создание папки для объектных файлов, если её нет
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Сборка исполняемого файла
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

# Компиляция исходных файлов в объектные файлы
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Цель для запуска приложения
run: $(TARGET)
	./$(TARGET)

# Очистка сгенерированных файлов
clean:
	rm -f $(TARGET) $(OBJFILES)
	rm -rf $(OBJDIR)

# Указание, что файлы не являются целями
.PHONY: all run clean
