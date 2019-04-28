#include <hashmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const int hashmap_startsize = 10;
const double fillfactor = 0.7;

hashmap * hashmap_init(){
    
    hashmap * res = malloc(1*sizeof(hashmap));
    *res = (hashmap){
        .filled=0,
        .size=hashmap_startsize,
        .entries=malloc(hashmap_startsize * sizeof(hashmap_entry)),
    };

    for(int i = 0; i < res->size; i++){
        res->entries[i] = (hashmap_entry){
            NULL, NULL, 0,
        };
    }

    return res;
}

void rehash(hashmap * self, int newsize){
    printf("rehashing\n");

    hashmap_entry * new =  malloc(newsize * sizeof(hashmap_entry));
    hashmap_entry * old = self->entries;
    int oldsize = self->size;


    for(int i = 0; i < newsize; i++){
        new[i] = (hashmap_entry){
            NULL, NULL, 0,
        };
    }
    self->entries = new;
    self->size = newsize;
    self->filled = 0;

    for(int i = 0; i < oldsize; i++){
        if(old[i].key != NULL && !old[i].defunct){
            hashmap_put(self,old[i].key, old[i].value);
        }
    }

    free(old);
}

void hashmap_free(hashmap * self){
    free(self->entries);
    free(self);
}


void hashmap_put(hashmap * self, void * key, void * value){
    hashmap_entry entry = {
        key,
        value,
        0
    };

    size_t addr = (size_t)key;
    size_t index = addr % self->size;        
    
    while(1){
        if(self->entries[index].key == key){
            break;
        }else if(self->entries[index].defunct){
            break;
        }else if(self->entries[index].key == NULL){
            break;
        }
        index = (index + 1) % self->size;
    }

    self->filled++;
    self->entries[index] = entry;


    if(((double)self->filled / (double)self->size) > fillfactor){
        rehash(self, self->size << 1);
    }
}

void * hashmap_get(hashmap * self, void * key){
    size_t addr = (size_t)key;
    size_t index = addr % self->size;

    for(int i = 0; i < self->size; i++){
        if(self->entries[index].key == NULL && !self->entries[index].defunct){
            return NULL;
        }else if(self->entries[index].key == key){
            return self->entries[index].value;
        }
        index = (index + 1) % self->size;
    }
    return NULL;
 
}

void * hashmap_delete(hashmap * self, void * key){
    size_t addr = (size_t)key;
    size_t index = addr % self->size;

    for(int i = 0; i < self->size; i++){
        if(self->entries[index].key == NULL && !self->entries[index].defunct){
            return NULL;
        }else if(self->entries[index].key == key){
            void * res = self->entries[index].value;
            self->entries[index].defunct = true;
            self->entries[index].value = NULL;
            self->entries[index].key = NULL;

            self->filled--;
            return res;
        }
        index = (index + 1) % self->size;
    }
    return NULL;
}


void hashmap_print(hashmap * self){
    printf("hashmap of size %i (filled %i): [\n", self->size, self->filled);

    for(int i = 0; i < self->size; i++){
        if(!self->entries[i].defunct && !(self->entries[i].key == NULL)){
            printf("key: 0x%lx, value: 0x%lx,\n", (size_t)self->entries[i].key, (size_t)self->entries[i].value);   
        }
    }
    printf("]\n");
}
