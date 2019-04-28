#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct hashmap_entry{
    void * key;
    void * value;
    int defunct;
}hashmap_entry;

typedef struct hashmap{
    int filled;
    int size;
    hashmap_entry * entries;
}hashmap;


hashmap * hashmap_init();
void rehash(hashmap * self, int newsize);
void hashmap_free(hashmap * self);
void hashmap_put(hashmap * self, void * key, void * value);
void * hashmap_get(hashmap * self, void * key);
void * hashmap_delete(hashmap * self, void * key);
void hashmap_print(hashmap * self);


#endif