#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "huffman.h"
#include "bit.h"
#include "utils.h"

Node* createNode(unsigned char symbol, unsigned long freq) {
    Node* node = malloc(sizeof(Node));
    node->symbol = symbol;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void buildFrequencyTable(FILE* inputFile, unsigned long freqs[256]) {
    int ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        freqs[(unsigned char)ch]++;
    }
    rewind(inputFile); // вернуться в начало для повторного чтения
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = malloc(sizeof(MinHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = malloc(capacity * sizeof(Node*));
    return heap;
}

void minHeapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->array[left]->freq < heap->array[smallest]->freq)
        smallest = left;
    if (right < heap->size && heap->array[right]->freq < heap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        Node* temp = heap->array[idx];
        heap->array[idx] = heap->array[smallest];
        heap->array[smallest] = temp;
        minHeapify(heap, smallest);
    }
}

void insertHeap(MinHeap* heap, Node* node) {
    if (heap->size == heap->capacity) return;

    int i = heap->size;
    heap->array[i] = node;
    heap->size++;

    while (i != 0 && heap->array[(i - 1) / 2]->freq > heap->array[i]->freq) {
        Node* temp = heap->array[i];
        heap->array[i] = heap->array[(i - 1) / 2];
        heap->array[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

Node* extractMin(MinHeap* heap) {
    if (heap->size == 0) return NULL;
    Node* root = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    minHeapify(heap, 0);
    return root;
}

Node* buildHuffmanTree(unsigned long freqs[256]) {
    MinHeap* heap = createMinHeap(256);

    for (int i = 0; i < 256; i++) {
        if (freqs[i] > 0) {
            insertHeap(heap, createNode((unsigned char)i, freqs[i]));
        }
    }

    while (heap->size > 1) {
        Node* left = extractMin(heap);
        Node* right = extractMin(heap);

        Node* parent = createNode(0, left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        insertHeap(heap, parent);
    }

    Node* root = (heap->size == 1) ? extractMin(heap) : createNode(0, 0);
    free(heap->array);
    free(heap);
    return root;
}

void generateCodes(Node* root, char* code, int depth, char* codes[256]) {
    if (!root) return;

    if (!root->left && !root->right) {
        // Лист: сохраняем код
        code[depth] = '\0';
        codes[root->symbol] = strdup(code);
        return;
    }

    code[depth] = '0';
    generateCodes(root->left, code, depth + 1, codes);

    code[depth] = '1';
    generateCodes(root->right, code, depth + 1, codes);
}

void encodeFile(FILE* inputFile, FILE* outputFile) {
    unsigned long freqs[256] = {0};
    buildFrequencyTable(inputFile, freqs);

    // Записываем частоты (256 × uint32_t)
    for (int i = 0; i < 256; i++) {
        uint32_t f = (uint32_t)freqs[i];
        fwrite(&f, sizeof(uint32_t), 1, outputFile);
    }

    Node* root = buildHuffmanTree(freqs);
    char* codes[256] = {0};
    char codeBuffer[256];
    generateCodes(root, codeBuffer, 0, codes);

    // Подсчитываем длину битового потока
    uint64_t totalBits = 0;
    for (int i = 0; i < 256; i++) {
        if (freqs[i] > 0) {
            totalBits += freqs[i] * strlen(codes[i]);
        }
    }

    // Записываем длину в битах
    fwrite(&totalBits, sizeof(uint64_t), 1, outputFile);

    // Побитовая запись
    BitWriter* writer = createBitWriter(outputFile);
    int ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        char* code = codes[(unsigned char)ch];
        for (int i = 0; code[i]; i++) {
            writeBit(writer, code[i] == '1');
        }
    }
    flushBits(writer);
    freeBitWriter(writer);

    // Статистика
    fseek(inputFile, 0, SEEK_END);
    uint64_t originalSize = ftell(inputFile);
    uint64_t compressedSize = 1024 + 8 + (totalBits + 7) / 8;
    printStats(freqs, codes, originalSize, compressedSize);

    // Освобождение
    for (int i = 0; i < 256; i++) {
        free(codes[i]);
    }
    freeTree(root);
}

void decodeFile(FILE* inputFile, FILE* outputFile) {
    // Читаем частоты
    unsigned long freqs[256] = {0};
    for (int i = 0; i < 256; i++) {
        uint32_t f;
        if (fread(&f, sizeof(uint32_t), 1, inputFile) != 1) return;
        freqs[i] = f;
    }

    // Читаем длину битового потока
    uint64_t totalBits;
    if (fread(&totalBits, sizeof(uint64_t), 1, inputFile) != 1) return;

    // Строим дерево
    Node* root = buildHuffmanTree(freqs);
    if (!root) return;

    // Декодируем биты
    BitReader* reader = createBitReader(inputFile, totalBits);
    Node* current = root;
    while (reader->bitsRead < reader->totalBits) {
        int bit = readBit(reader);
        if (bit < 0) break;

        current = (bit == 0) ? current->left : current->right;

        if (!current->left && !current->right) {
            fputc(current->symbol, outputFile);
            current = root;
        }
    }
    freeBitReader(reader);
    freeTree(root);
}