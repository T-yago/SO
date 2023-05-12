#include "execute.h"
#include "hashTable.h"


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
void insert(HashTable* table, int key, Mensagem* value) {
    // Hash the key to get an index
    int index = hash(key);

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



int main () {

    HashTable* table = newHashTable();



    Mensagem* m1 = (Mensagem*) malloc(sizeof(Mensagem));
    m1->pid = 1;
    m1->type = 1;
    m1->tempo.tv_usec = 1000000;
    strcpy(m1->name_program, "Program 1");

    printf("Inserting Mensagem 1 with key %d\n", m1->pid);
    insert(table, m1->pid, m1);

    Mensagem* m2 = (Mensagem*) malloc(sizeof(Mensagem));
    m2->pid = 2;
    m2->type = 2;
    m2->tempo.tv_usec = 2000000;
    strcpy(m2->name_program, "Program 2");

    printf("Inserting Mensagem 2 with key %d\n", m2->pid);
    insert(table, m2->pid, m2);

    // Test the get function to retrieve the objects
    Mensagem* retrieved_m1 = (Mensagem*) get(table, m1->pid);
    if (retrieved_m1 == NULL || retrieved_m1 != m1) {
        printf("Error: failed to retrieve Mensagem 1\n");
        return 1;
    }
    printf("Retrieved Mensagem 1 with key %d\n", retrieved_m1->pid);

    Mensagem* retrieved_m2 = (Mensagem*) get(table, m2->pid);
    if (retrieved_m2 == NULL || retrieved_m2 != m2) {
        printf("Error: failed to retrieve Mensagem 2\n");
        return 1;
    }
    printf("Retrieved Mensagem 2 with key %d\n", retrieved_m2->pid);

    // Test the lookup function
    if (lookup(table, m1->pid) == 0) {
        printf("Error: failed to find key %d in the hashtable\n", m1->pid);
        return 1;
    }
    printf("Found key %d in the hashtable\n", m1->pid);

    if (lookup(table, m2->pid) == 0) {
        printf("Error: failed to find key %d in the hashtable\n", m2->pid);
        return 1;
    }
    printf("Found key %d in the hashtable\n", m2->pid);


    freeHashTable(table);
return 0;
}