#ifndef LAB2_UTILS_H
#define LAB2_UTILS_H

#include <stdio.h>
#include <stdint.h>

// Выводит статистику: частоты, коды, коэффициент сжатия
void printStats(unsigned long freqs[256], char* codes[256], uint64_t originalSize, uint64_t compressedSize);

// Сравнивает два файла побайтово
int compareFiles(const char* file1, const char* file2);


#endif //LAB2_UTILS_H