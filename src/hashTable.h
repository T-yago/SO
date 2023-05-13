#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include "execute.h"

#define MAX_SIZE 100

// Define the HashNode struct
typedef struct hashNode {
    int key;
    Mensagem* value;
} HashNode;

// Define the HashTable struct
typedef struct hashtable {
    HashNode* nodes[MAX_SIZE];
    int size;
} HashTable;

// Funções

HashTable* newHashTable();
void insert(HashTable* table, int key, Mensagem value);
Mensagem* get(HashTable* table, int key);
void delete(HashTable* table, int key);
int lookup(HashTable* table, int key);
void freeHashTable(HashTable* table);

#endif