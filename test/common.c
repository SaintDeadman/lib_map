#include <stdlib.h>
#include <time.h>
#include "common.h"

void generate_random_bytes(unsigned char *buffer, size_t length) {
    srand((unsigned int)clock());
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (unsigned char)(rand() % 256); // Генерация случайного байта
    }
}