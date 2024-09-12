#include "HashTable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static unsigned int _hashFunction(HashTable* hashTable, const char* key);
static char* _get(HashTable* hashTable, const char* key);
static char* _set(HashTable* hashTable, char* key, char* value);
static char* _toString(HashTable* hashTable);

HashTable* newHashTable(int size) {
    HashTable* hashTable = malloc(sizeof(HashTable));
    hashTable->size = size;
    hashTable->_hashFunction = _hashFunction;
    hashTable->get = _get;
    hashTable->set = _set;
    hashTable->toString = _toString;
    return hashTable;
}

void destroyHashTable(HashTable* hashTable) {
    free(hashTable);
}

static unsigned int _hashFunction(HashTable* hashTable, const char* key) {
    int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash += key[i];
    }
    return hash % hashTable->size;
}

static char* _get(HashTable* hashTable, const char* key) {
    int index = _hashFunction(hashTable, key);
    return hashTable->data[index]->value;
}

static char* _set(HashTable* hashTable, char* key, char* value) {
    int index = _hashFunction(hashTable, key);
    hashTable->data[index] = malloc(sizeof(HashTableEntry));
    hashTable->data[index]->key = malloc(sizeof(char) * strlen(key) + 1);
    hashTable->data[index]->value = malloc(sizeof(char) * strlen(value) + 1);
    strcpy(hashTable->data[index]->key, key);
    strcpy(hashTable->data[index]->value, value);
    return hashTable->data[index]->value;
}

static char* _toString(HashTable* hashTable) {

    /*
    TODO: Fix this output:
                                                shTable {
    key3: value3
    key4: value4
    key5: value5
    key6: value6
    key7: value7
    key8: value8
    key10: value10
    key1: value1
    key2: value2

    }
    
     */

    char* string = malloc(sizeof(char) * 2720);
    sprintf(string, "\n%s%s", string, "HashTable {\n");
    for (int i = 0; i < hashTable->size; i++) {
        if (hashTable->data[i] != NULL) {
            sprintf(string, "    %s%s: %s\n", string, hashTable->data[i]->key, hashTable->data[i]->value);
        }
    }
    sprintf(string, "%s%s", string, "\n}");

    return string;
}