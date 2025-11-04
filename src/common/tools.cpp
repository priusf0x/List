#include "tools.h"

#include <string.h>

#include "Assert.h"

void* recalloc(void*  pointer,
               size_t current_size,
               size_t new_size)
{
    ASSERT(pointer != NULL);

    void* backup_pointer = NULL;

    backup_pointer = realloc(pointer, new_size);

    if (backup_pointer == NULL)
    {
        return NULL;
    }

    pointer = backup_pointer;

    memset((char*) pointer + current_size, 0, new_size - current_size);

    return pointer;
}
