#ifndef LAB2_HUFFMAN_H
#define LAB2_HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Node {
    unsigned char symbol;  // символ (только в листьях)
    unsigned long freq;    // частота
    struct Node* left;
    struct Node* right;
} Node;

typedef struct MinHeap {
    int size;       // текущее количество узлов
    int capacity;   // максимальное количество (обычно 256)
    Node** array;   // массив указателей на Node
} MinHeap;

// Создаёт новый узел
Node* createNode(unsigned char symbol, unsigned long freq);

// Освобождает память дерева
void freeTree(Node* root);

// Считает частоты всех байтов из файла
// freqs — массив из 256 элементов, инициализированный нулями
void buildFrequencyTable(FILE* inputFile, unsigned long freqs[256]);

// Строит дерево Хаффмана по таблице частот
Node* buildHuffmanTree(unsigned long freqs[256]);

// Генерирует коды Хаффмана (сохраняет в массив строк, индекс = символ)
void generateCodes(Node* root, char* code, int depth, char* codes[256]);

// Режим encode
// Кодирует файл и записывает результат в бинарный файл формата:
void encodeFile(FILE* inputFile, FILE* outputFile);

// Режим decode
// Читает частоты из файла, строит дерево и декодирует биты
void decodeFile(FILE* inputFile, FILE* outputFile);

#endif //LAB2_HUFFMAN_H