#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// Выводит подробную статистику по результатам кодирования:
void printStats(unsigned long freqs[256], char* codes[256], uint64_t originalSize, uint64_t compressedSize) {
    printf("\n=== Статистика ===\n");
    printf("Символ | Частота | Код\n");
    printf("------------------------\n");

    for (int i = 0; i < 256; i++) {
        if (freqs[i] > 0) {
            char disp = (i >= 32 && i <= 126) ? (char)i : '?';

            // Особые случаи: заменяем управляющие символы на буквы
            if (i == '\n') disp = 'n';  
            if (i == '\t') disp = 't';  
            if (i == ' ')  disp = ' ';  

            // Выводим информацию о символе
            printf("  %3d (%c) | %7lu | %s\n", i, disp, freqs[i], codes[i]);
        }
    }

    // Рассчитываем и выводим коэффициент сжатия
    double ratio = (double)compressedSize / (double)originalSize;
    printf("\nИсходный размер: %llu байт\n", (unsigned long long)originalSize);
    printf("Сжатый размер:   %llu байт\n", (unsigned long long)compressedSize);
    printf("Коэффициент сжатия: %.3f\n", ratio);
}

// Сравнивает два файла побайтово и возвращает:
//   1 - если файлы идентичны,
//   0 - если различаются (и выводит позицию первого различия),
//  -1 - если не удалось открыть хотя бы один из файлов.
int compareFiles(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "rb");
    FILE* f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        return -1; // ошибка открытия
    }

    int ch1, ch2;
    int pos = 0; // текущая позиция в файле (для отладки)

    while (1) {
        ch1 = fgetc(f1);
        ch2 = fgetc(f2);

        if (ch1 == EOF && ch2 == EOF) {
            fclose(f1);
            fclose(f2);
            return 1;
        }

        if (ch1 != ch2) {
            printf("Различие на позиции %d: %d vs %d\n", pos, ch1, ch2);
            fclose(f1);
            fclose(f2);
            return 0;
        }

        pos++;
    }
}
