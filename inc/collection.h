#ifndef COLLECTION_SV_H
#define COLLECTION_SV_H

#include <stdlib.h>
#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>

//#define NDEBUG
#include <assert.h>

typedef unsigned char byte_t;

typedef struct DynamicBufferData
{
	int count;
	int capacity;
	int element_size;
	void* rawData;
} DynamicBufferData;

typedef struct DynamicBuffer
{
	DynamicBufferData* data;

} DynamicBuffer;

typedef struct DynamicBufferIter
{
	DynamicBuffer* list;
	int current_index;

} DynamicBufferIter;

inline static bool buffer_iter_next(DynamicBufferIter* iter);
inline static DynamicBufferIter buffer_iter(DynamicBuffer* list);
inline static void* buffer_iter_get(DynamicBufferIter* iter);
inline static DynamicBuffer buffer_new(int element_size, int capacity);
inline static void* buffer_get_item(DynamicBuffer* array, int index);
inline static void buffer_set(DynamicBuffer* array, int index, void* data);
inline static void buffer_add(DynamicBuffer* array, void* data);
inline static int buffer_index_of(DynamicBuffer* array, void* elementData);
inline static void buffer_free(DynamicBuffer* array);
inline static void buffer_remove_at_spawn_back(DynamicBuffer* array, int index);
inline static void* buffer_get_data(DynamicBuffer* list);

inline static DynamicBufferIter buffer_iter(DynamicBuffer* list)
{
	DynamicBufferIter iter = { .list = list, .current_index = -1 };

	return iter;
}

inline static bool buffer_iter_next(DynamicBufferIter* iter)
{
	if (iter->current_index == iter->list->data->count-1)
	{
		return false;
	}

	iter->current_index++;

	return true;
}

inline static void* buffer_iter_get(DynamicBufferIter* iter)
{
	return buffer_get_item(iter->list, iter->current_index);
}

inline static DynamicBuffer buffer_new(int element_size, int capacity)
{
	DynamicBuffer list = {0};

	list.data = malloc(sizeof(DynamicBufferData));
	list.data->rawData = malloc(capacity * element_size);
	assert(list.data != NULL);
	assert(list.data->rawData);
	list.data->element_size = element_size;
	list.data->capacity = capacity;
	list.data->count = 0;

	return list;
}

inline static void* buffer_get_data(DynamicBuffer* list)
{
	return list->data->rawData;
}

inline static void* buffer_get_item(DynamicBuffer* array, int index)
{
	assert(index >= 0);
	assert(index <= array->data->count - 1);

	return ((byte_t*)array->data->rawData) + index * (array->data->element_size);
}

inline static void buffer_set(DynamicBuffer* array, int index, void* data)
{
	void* toSet = ((byte_t*)array->data->rawData) + index * array->data->element_size;
	memcpy(toSet, data, array->data->element_size);
}

inline static void buffer_add(DynamicBuffer* array, void* data)
{
	if (array->data->capacity == array->data->count)
	{
		array->data->capacity += 50;
		int newSize = array->data->capacity * array->data->element_size;
		void* new_array = realloc(array->data->rawData, newSize);
		assert(new_array != NULL);
		array->data->rawData = new_array;
	}

	buffer_set(array, array->data->count, data);
	array->data->count++;
}

inline static int buffer_index_of(DynamicBuffer* array, void* elementData)
{
	byte_t* pointer = (byte_t*)array->data->rawData;
	size_t size = array->data->element_size;

	for (int i = 0; i < array->data->count; i++)
	{
		if (memcmp(pointer, elementData, size) == 0)
		{
			return i;
		}

		pointer = pointer + size;
	}

	return -1;
}

inline static void buffer_remove_at_spawn_back(DynamicBuffer* array, int index)
{
	void* last = ((byte_t*)array->data->rawData) + array->data->count * array->data->element_size;
	void* toRemove = ((byte_t*)array->data->rawData) + index * array->data->element_size;

	memcpy(toRemove, last, array->data->element_size);
	array->data->count--;
}

inline static void buffer_free(DynamicBuffer* array)
{
	if (array->data == NULL)
		return;

	free(array->data->rawData);
	free(array->data);
}
#endif