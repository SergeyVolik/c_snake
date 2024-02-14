#ifndef COLLECTION_SV_H
#define COLLECTION_SV_H
#include <stdlib.h>
#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>

//#define NDEBUG
#include <assert.h>

struct Array
{
	size_t size;
	unsigned int capacity;
	unsigned int count;
	void* rawData;
};

/** Allocate a new buffer_t with "count" entries of "size" size. */
inline Array* array_new(size_t size, unsigned int capacity)
{
	Array* p = (Array*) malloc(offsetof(Array, rawData) + capacity * size);

	assert(p != NULL);

    p->size = size;
    p->count = 0;
	p->capacity = capacity;
    return p;
}

inline void* array_get(Array* array, int index)
{
	assert(index < array->count);
	assert(index >= 0);

	return &array->rawData + index * array->size;
}

inline int array_index_of(Array* array, void* elementData)
{
	void* pointer = &array->rawData;
	size_t size = array->size;
	for (int i = 0; i < array->count; i++)
	{
		if (memcmp(pointer, elementData, size) == 0)
		{
			return i;
		}

		pointer = &pointer + size;
	}

	return -1;
}

inline void array_remove_at_spawn_back(Array* array, int index)
{
	void* last =  &array->rawData + array->count * array->size;
	void* toRemove = &array->rawData + index * array->size;

	memcpy(toRemove, last, array->size);
	array->count--;
}



#endif