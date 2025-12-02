#ifndef LAB2_BIT_H
#define LAB2_BIT_H

#include <stdio.h>
#include <stdint.h>

// Побитовая запись
typedef struct BitWriter {
    FILE* file;
    uint8_t buffer;      // накопленные биты (младшие bitsInBuffer битов значимы)
    int bitsInBuffer;    // 0..7
} BitWriter;

// Побитовое чтение
typedef struct BitReader {
    FILE* file;
    uint8_t buffer;
    int bitsLeft;         // сколько битов осталось в buffer
    uint64_t totalBits;   // общее число бит в потоке
    uint64_t bitsRead;    // сколько прочитано
} BitReader;

BitWriter* createBitWriter(FILE* file);
void writeBit(BitWriter* writer, int bit);
void flushBits(BitWriter* writer); // дозаписывает остаток
void freeBitWriter(BitWriter* writer);

BitReader* createBitReader(FILE* file, uint64_t totalBits);
int readBit(BitReader* reader);
void freeBitReader(BitReader* reader);

#endif //LAB2_BIT_H