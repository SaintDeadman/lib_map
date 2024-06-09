# Имя исполняемых файлов
MAIN_TARGET = main
MAP_TARGET = 00_simple_map

# Компилятор и опции
CC = gcc
CFLAGS = -Wall -Wextra -O2 -m64

# Папка для объектных файлов
OBJDIR = obj
SRCDIR = map_lib
TESTDIR = test

# Поиск всех .c файлов в директории map_lib и их соответствующих .o файлов в obj
SRCFILES = $(wildcard $(SRCDIR)/*.c)
MAIN_SRCFILES = main.c
MAP_SRCFILES = test/00_simple_map.c
OBJFILES = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRCFILES)))

# Поиск всех .c файлов в директории test и их соответствующих .o файлов в obj
TESTSRCFILES = $(wildcard $(TESTDIR)/*.c)
TESTOBJFILES = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(TESTSRCFILES)))

# Цель по умолчанию: сборка всего проекта
all: $(MAIN_TARGET) $(MAP_TARGET) $(TEST_TARGET)

# Создание папки для объектных файлов, если её нет
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Сборка исполняемого файла main
$(MAIN_TARGET): $(OBJFILES) $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $(MAIN_TARGET) $(OBJFILES) $(OBJDIR)/main.o -lpthread -march=x86-64

# Сборка исполняемого файла 00_simple_map
$(MAP_TARGET): $(OBJFILES) $(OBJDIR)/00_simple_map.o $(OBJDIR)/common.o
	$(CC) $(CFLAGS) -o $(MAP_TARGET) $(OBJFILES) $(OBJDIR)/00_simple_map.o $(OBJDIR)/common.o -lcunit -lpthread -march=x86-64

# Сборка исполняемого файла для тестов
TEST_TARGET = test_runner
$(TEST_TARGET): $(OBJFILES) $(TESTOBJFILES) $(OBJDIR)/common.o
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(OBJFILES) $(TESTOBJFILES) -lcunit -lpthread -march=x86-64

# Компиляция исходных файлов в объектные файлы
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/00_simple_map.o: $(TESTDIR)/00_simple_map.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Компиляция common.c
$(OBJDIR)/common.o: $(TESTDIR)/common.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Цель для запуска приложения main
run_main: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# Цель для запуска приложения 00_simple_map
run_map: $(MAP_TARGET)
	./$(MAP_TARGET)

# Цель для запуска тестов
run_tests: $(TEST_TARGET)
	./$(TEST_TARGET)

# Очистка сгенерированных файлов
clean:
	rm -f $(MAIN_TARGET) $(MAP_TARGET) $(TEST_TARGET) $(OBJFILES) $(OBJDIR)/main.o $(OBJDIR)/00_simple_map.o $(OBJDIR)/common.o $(TESTOBJFILES)
	rm -rf $(OBJDIR)

# Указание, что файлы не являются целями
.PHONY: all run_main run_map run_tests clean
