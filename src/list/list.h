#ifndef LIST_H
#define LIST_H

#include <stdio.h>

struct list_s;

enum list_return_e
{
    LIST_RETURN_SUCCESS,
    LIST_RETURN_ALLOCATION_ERROR,
    LIST_RETURN_FILE_OPEN_ERROR,
    LIST_RETURN_FILE_CLOSE_ERROR,
    LIST_RETURN_INCORRECT_VALUE,
    LIST_RETURN_NULL_POINTER,
    LIST_RETURN_CANARY_DAMAGED,
    LIST_RETURN_NOT_CYCLE,
    LIST_RETURN_ELEMENT_COUNT_ERROR,
};

typedef double data_type;

//======================== LIST ===========================

typedef struct list_s* list_t;

// =================== MEMORY_CONTROL =====================

list_return_e InitList(list_s** list, size_t start_list_size);
list_return_e DestroyList(list_s** list);

// =================== ELEMENT_CONTROL ====================

list_return_e ListAddElement(list_s* list, data_type value);
list_return_e ListAddAfterElement(list_s* list, data_type value, size_t index);
list_return_e ListDeleteElement(list_s* list, size_t  index);

// ================= ELEMENTS_NAVIGATION(const proebal) ==================

list_return_e GetElementValue(const list_s* list, size_t element_index, data_type* value);
ssize_t GetNextElement(const list_s* list, size_t element_index);
ssize_t GetPreviousElement(const list_s* list, size_t element_index);
ssize_t GetHeadElement(const list_s* list);
ssize_t GetTailElement(const list_s* list);

// ====================== LIST_LOG ========================

list_return_e ListDump(const list_s* list, const char* comment);
void SetLogFileName(const char* log_file_name);

FILE* GetLogFile(); // single tone

//======================= VERIFY ==========================

list_return_e ListVerify(const list_s* list);
#ifndef NDEBUG
#define VERIFY_RET(___X) do{list_return_e ___output = ListVerify(___X); if (___output != LIST_RETURN_SUCCESS) { return ___output;}} while(0)
#else
#define VERIFY_RET(X)
#endif

//=========================================================

#endif //LIST_H
