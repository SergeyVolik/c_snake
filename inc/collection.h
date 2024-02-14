#ifndef COLLECTION_SV_H
#define COLLECTION_SV_H
#include <stdlib.h>
#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>

//#define NDEBUG
#include <assert.h>

typedef struct NativeList
{
	size_t size;
	unsigned int count;
	unsigned int capacity;
	void* rawData;
} NativeList;

inline static NativeList nav_list_new(size_t size, unsigned int capacity)
{
	NativeList list = {0};

	list.rawData = malloc(capacity * size);

	assert(list.rawData != NULL);

	list.size = size;
	list.capacity = capacity;
	list.count = 0;

    return list;
}

inline static void* nav_list_get(NativeList* array, int index)
{
	assert(index < array->count);
	assert(index >= 0);

	return &array->rawData + index * array->size;
}

inline static void* array_set(NativeList* array, int index, void* data)
{
	void* toSet = &array->rawData + index * array->size;
	memcpy(toSet, data, array->size);
}

inline static void* nav_list_add(NativeList* array, void* data)
{
	if (array->capacity == array->count)
	{
		int newCap = array->capacity * 2;
		void* data = malloc((array->capacity + 1) * array->capacity);	
	
		assert(data != NULL);

		memcpy(data, array->rawData, array->size * array->capacity);
		free(array->rawData);
		array->rawData = data;
	}
	array->count++;
	void* toSet = &array->rawData + array->capacity + 1 * array->size;

	array_set(array, array->count, data);
}

inline static int nav_list_index_of(NativeList* array, void* elementData)
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

inline static void nav_list_remove_at_spawn_back(NativeList* array, int index)
{
	void* last =  &array->rawData + array->count * array->size;
	void* toRemove = &array->rawData + index * array->size;

	memcpy(toRemove, last, array->size);
	array->count--;
}

#endif