#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct HashTable HashTable;

typedef struct HashTableEntry {
    char* key;
    char* value;
} HashTableEntry;

struct HashTable {
    HashTableEntry* data[200]; //TODO: make this an ArrayList for dynamic resizing 
    int size;
    unsigned int (*_hashFunction)(struct HashTable* hashTable, const char* key);
    char* (*get)(struct HashTable* hashTable, const char* key);
    char* (*set)(struct HashTable* hashTable, char* key, char* value);
    char* (*toString)(struct HashTable* hashTable);
};

HashTable* newHashTable(int size);
void destroyHashTable(HashTable* hashTable);

#endif