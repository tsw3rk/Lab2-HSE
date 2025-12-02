#include <stdio.h>
#include <string.h>
#include "huffman.h"

void printUsage() {
    fprintf(stderr, "Использование:\n");
    fprintf(stderr, "  ./huffman encode <входной_файл> <выходной_файл>\n");
    fprintf(stderr, "  ./huffman decode <входной_файл> <выходной_файл>\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage();
        return 1;
    }

    char* mode = argv[1];
    char* inputFileName = argv[2];
    char* outputFileName = argv[3];

    FILE* inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        fprintf(stderr, "Ошибка: не удалось открыть входной файл '%s'\n", inputFileName);
        return 1;
    }

    FILE* outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fprintf(stderr, "Ошибка: не удалось создать выходной файл '%s'\n", outputFileName);
        fclose(inputFile);
        return 1;
    }

    if (strcmp(mode, "encode") == 0) {
        encodeFile(inputFile, outputFile);
    } else if (strcmp(mode, "decode") == 0) {
        decodeFile(inputFile, outputFile);
    } else {
        fprintf(stderr, "Неизвестный режим: '%s'. Используйте 'encode' или 'decode'.\n", mode);
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}