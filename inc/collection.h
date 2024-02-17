#ifndef COLLECTION_SV_H
#define COLLECTION_SV_H

#include <stdlib.h>
#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>

//#define NDEBUG
#include <assert.h>

typedef struct NativeListData
{
	int count;
	int capacity;
	int element_size;
	void* rawData;
} NativeListData;

typedef struct NativeList
{
	NativeListData* data;

} NativeList;

typedef struct NativeListIter
{
	NativeList* list;
	int current_index;

} NativeListIter;

inline static bool nav_list_iter_next(NativeListIter* iter);
inline static NativeListIter nav_list_iter(NativeList* list);
inline static void* nav_list_iter_get(NativeListIter* iter);
inline static NativeList nav_list_new(int element_size, int capacity);
inline static void* nav_list_get_item(NativeList* array, int index);
inline static void nav_list_set(NativeList* array, int index, void* data);
inline static void nav_list_add(NativeList* array, void* data);
inline static int nav_list_index_of(NativeList* array, void* elementData);
inline static void nav_list_free(NativeList* array);
inline static void nav_list_remove_at_spawn_back(NativeList* array, int index);
inline static void* nav_array_get(NativeList* list);

inline static NativeListIter nav_list_iter(NativeList* list)
{
	NativeListIter iter = { .list = list, .current_index = -1 };

	return iter;
}

inline static bool nav_list_iter_next(NativeListIter* iter)
{
	if (iter->current_index == iter->list->data->count-1)
	{
		return false;
	}

	iter->current_index++;

	return true;
}

inline static void* nav_list_iter_get(NativeListIter* iter)
{
	return nav_list_get_item(iter->list, iter->current_index);
}

inline static NativeList nav_list_new(int element_size, int capacity)
{
	NativeList list = {0};

	list.data = malloc(sizeof(NativeListData));
	list.data->rawData = malloc(capacity * element_size);
	assert(list.data != NULL);
	assert(list.data->rawData);
	list.data->element_size = element_size;
	list.data->capacity = capacity;
	list.data->count = 0;

	return list;
}

inline static void* nav_array_get(NativeList* list)
{
	return list->data->rawData;
}

inline static void* nav_list_get_item(NativeList* array, int index)
{
	//assert(index >= 0);
	//assert(index <= array->count - 1);

	return ((char*)array->data->rawData) + index * (array->data->element_size);
}

inline static void nav_list_set(NativeList* array, int index, void* data)
{
	void* toSet = ((char*)array->data->rawData) + index * array->data->element_size;
	memcpy(toSet, data, array->data->element_size);
}

inline static void nav_list_add(NativeList* array, void* data)
{
	if (array->data->capacity == array->data->count)
	{
		array->data->capacity *= 2;
		int newSize = array->data->capacity * array->data->element_size;
		void* new_array = realloc(array->data->rawData, newSize);
		assert(new_array != NULL);
		array->data->rawData = new_array;
	}

	nav_list_set(array, array->data->count, data);
	array->data->count++;
}

inline static int nav_list_index_of(NativeList* array, void* elementData)
{
	char* pointer = (char*)array->data->rawData;
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

inline static void nav_list_free(NativeList* array)
{
	if (array->data == NULL)
		return;

	free(array->data->rawData);
	free(array->data);
}

inline static void nav_list_remove_at_spawn_back(NativeList* array, int index)
{
	void* last = ((char*)array->data->rawData) + array->data->count * array->data->element_size;
	void* toRemove = ((char*)array->data->rawData) + index * array->data->element_size;

	memcpy(toRemove, last, array->data->element_size);
	array->data->count--;
}

#endif