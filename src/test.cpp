#include "list.h"

#include <stdio.h>

int main(void)
{
    list_t* list = NULL;

    InitList(&list);
    ListAddElement(list, 12);
    ListAddElement(list, 4);
    ListAddElement(list, 7);
    ListAddElement(list, 20);

    ListDeleteElement(list, 2);

    ListDump(list);
    ListDot(list, 1);

    DestroyList(&list);

    return 0;
}
