#include "list.h"

#include <stdio.h>

int main(void)
{
    list_t* list = NULL;

    InitList(&list);
    ListAddElement(list, 12);
    ListDump(list);

    ListAddAfterElement(list, 69, 0);

    ListDump(list);

    DestroyList(&list);

    return 0;
}
