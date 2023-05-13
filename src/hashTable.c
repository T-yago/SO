#include "execute.h"
#include "hashTable.h"
#include <string.h>


// Initialize a new HashTable
HashTable* newHashTable() {
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    table->size = 0;
    for (int i = 0; i < MAX_SIZE; i++) {
        table->nodes[i] = NULL;
    }
    return table;
}

// Hash function to map a key to an index
int hash(int key) {
    return key % MAX_SIZE;
}

// Insert a new key-value pair into the HashTable using open addressing
void insert(HashTable* table, int key, Mensagem msg) {
    // Hash the key to get an index
    int index = hash(key);

    Mensagem* value = malloc (sizeof(Mensagem));
    strncpy(value->name_program, msg.name_program, sizeof(value->name_program));
    value->pid = msg.pid;
    value->tempo = msg.tempo;
    value->type = msg.type;

    // Check if the index is already occupied
    while (table->nodes[index] != NULL) {
        // If the key already exists, update the value and return
        if (table->nodes[index]->key == key) {
            table->nodes[index]->value = value;
            return;
        }
        // Otherwise, probe to the next index using linear probing
        index = (index + 1) % MAX_SIZE;
    }

    // Create a new HashNode
    HashNode* node = (HashNode*) malloc(sizeof(HashNode));
    node->key = key;
    node->value = value;

    // Insert the node at the index
    table->nodes[index] = node;
    table->size++;
}

// Get the value associated with a key from the HashTable using open addressing
Mensagem* get(HashTable* table, int key) {
    // Hash the key to get an index
    int index = hash(key);

    // Traverse the HashTable using linear probing until the key is found or an empty slot is reached
    while (table->nodes[index] != NULL) {
        if (table->nodes[index]->key == key) {
            return table->nodes[index]->value;
        }
        index = (index + 1) % MAX_SIZE;
    }

    // Key not found
    return NULL;
}

// Delete a key-value pair from the HashTable using open addressing
void delete(HashTable* table, int key) {
    // Hash the key to get an index
    int index = hash(key);

    // Traverse the HashTable using linear probing until the key is found or an empty slot is reached
    while (table->nodes[index] != NULL) {
        if (table->nodes[index]->key == key) {
            // Delete the node at the index by setting it to NULL
            free (table->nodes[index]->value);
            free(table->nodes[index]);
            table->nodes[index] = NULL;
            table->size--;
            return;
        }
        index = (index + 1) % MAX_SIZE;
    }
}

// Find the index of a key in the HashTable using open addressing
int lookup(HashTable* table, int key) {
    // Hash the key to get an index
    int index = hash(key);

    // Traverse the HashTable using linear probing until the key is found or an empty slot is reached
    while (table->nodes[index] != NULL) {
        if (table->nodes[index]->key == key) {
            return index;
        }
        index = (index + 1) % MAX_SIZE;
    }

    // Key not found
    return -1;
}

void freeHashTable(HashTable* table) {
    if (table == NULL) {
        return;
    }
    for (int i = 0; i < table->size; i++) {
        HashNode* node = table->nodes[i];
        if (node != NULL) {
            free(node);
        }
    }
    free(table);
}
