#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bit.h"

// Создаёт новый побитовый писатель.
// Принимает уже открытый бинарный файл для записи.
BitWriter* createBitWriter(FILE* file) {
    BitWriter* writer = malloc(sizeof(BitWriter));
    writer->file = file;
    writer->buffer = 0;         // нет накопленных битов
    writer->bitsInBuffer = 0;   // счётчик текущих битов в буфере (0..7)
    return writer;
}

// Записывает один бит (0 или 1) в буфер.
// Когда набирается 8 бит - байт записывается в файл.
void writeBit(BitWriter* writer, int bit) {
    if (bit) {
        // Устанавливаем бит в позицию (7 - bitsInBuffer),
        // чтобы первый записанный бит стал старшим (MSB)
        writer->buffer |= (1 << (7 - writer->bitsInBuffer));
    }
    writer->bitsInBuffer++;

    // Если буфер заполнен (8 бит), сбрасываем его в файл
    if (writer->bitsInBuffer == 8) {
        fwrite(&(writer->buffer), 1, 1, writer->file);
        writer->buffer = 0;
        writer->bitsInBuffer = 0;
    }
}

// Дозаписывает оставшиеся биты (если буфер неполный).
void flushBits(BitWriter* writer) {
    if (writer->bitsInBuffer > 0) {
        fwrite(&(writer->buffer), 1, 1, writer->file);
    }
}

// Освобождает структуру BitWriter (сам файл не закрывает!).
void freeBitWriter(BitWriter* writer) {
    free(writer);
}

// Создаёт побитовый читатель.
BitReader* createBitReader(FILE* file, uint64_t totalBits) {
    BitReader* reader = malloc(sizeof(BitReader));
    reader->file = file;
    reader->buffer = 0;         // текущий прочитанный байт
    reader->bitsLeft = 0;       // сколько битов из buffer ещё не выдано
    reader->totalBits = totalBits;
    reader->bitsRead = 0;       // сколько битов уже прочитано всего
    return reader;
}

// Читает один бит (0 или 1). Возвращает -1, если достигнут конец потока.
int readBit(BitReader* reader) {
    // Прекращаем чтение, если прочитали все биты
    if (reader->bitsRead >= reader->totalBits) {
        return -1;
    }

    // Если в буфере закончились биты - читаем следующий байт
    if (reader->bitsLeft == 0) {
        if (fread(&(reader->buffer), 1, 1, reader->file) != 1) {
            return -1; // ошибка чтения
        }
        reader->bitsLeft = 8;
    }

    // Извлекаем старший ещё не прочитанный бит из buffer
    int bit = (reader->buffer >> (8 - reader->bitsLeft)) & 1;
    reader->bitsLeft--;
    reader->bitsRead++;
    return bit;
}

// Освобождает структуру BitReader.
void freeBitReader(BitReader* reader) {
    free(reader);
}