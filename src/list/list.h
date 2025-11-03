#ifndef LIST_H
#define LIST_H

#include <stdio.h>

struct list_t;

enum list_return_e
{
    LIST_RETURN_SUCCESS,
    LIST_RETURN_ALLOCATION_ERROR,
    LIST_RETURN_FILE_OPEN_ERROR,
    LIST_RETURN_FILE_CLOSE_ERROR,
    LIST_RETURN_INCORRECT_VALUE,
    LIST_RETURN_UNDEFINED_ELEMENT
};

typedef double data_type;

// =================== MEMORY_CONTROL =====================

list_return_e InitList(list_t** list, size_t start_list_size);
list_return_e DestroyList(list_t** list);

// =================== ELEMENT_CONTROL ====================

list_return_e ListAddElement(list_t* list, data_type value);
list_return_e ListAddAfterElement(list_t* list, data_type value, int index);
list_return_e ListDeleteElement(list_t* list, size_t  index);

// ================= ELEMENTS_NAVIGATION ==================

list_return_e GetElementValue(list_t* list, size_t element_index, data_type* value);
ssize_t GetNextElement(list_t* list, size_t element_index);
ssize_t GetPreviousElement(list_t* list, size_t element_index);
ssize_t GetHeadElement(list_t* list);
ssize_t GetTailElement(list_t* list);

// ====================== LIST_LOG ========================

list_return_e ListDump(list_t* list);

FILE* GetLogFile(); // single tone

//=========================================================

#endif //LIST_H
