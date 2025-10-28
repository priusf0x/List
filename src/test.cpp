#include "list.h"

#include <stdio.h>

int main(void)
{
    list_t* list = NULL;

    InitList(&list);
    ListAddElement(list, 12);
    // ListAddElement(list, 4);
    // ListDeleteElement(list, 11);
    // ListAddElement(list, 7);
    // ListAddAfterElement(list, 9, 3);
    // ListAddElement(list, 20);
    // ListDeleteElement(list, 2);
    // ListAddAfterElement(list, 69, 5);
    // ListAddAfterElement(list, 69, 0);
    // ListAddAfterElement(list, 69, 2);

    ListDump(list);
    ListDot(list, 1);

    DestroyList(&list);

    return 0;
}
