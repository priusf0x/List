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
    LIST_RETURN_INCORRECT_VALUE
};

typedef double data_type;

list_return_e InitList(list_t** list);
list_return_e ListAddElement(list_t* list, data_type value);
list_return_e ListDeleteElement(list_t* list, size_t  index);
list_return_e ListDump(list_t* list);
list_return_e ListDot(list_t* list, size_t  dump_number);
list_return_e DestroyList(list_t** list);

FILE* GetLogFile();

#endif //LIST_H
