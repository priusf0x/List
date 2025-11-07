#include "list.h"

#include <stdio.h>

int main(void)
{
    list_t* list = NULL;
    SetLogFileName("logs/bebra.htm");

    InitList(&list, 2);
    ListAddElement(list, 12);
    ListAddAfterElement(list, 69, 0);
    ListAddElement(list, 12);
    ListDump(list,"");
    ListAddElement(list, 11);
    ListDump(list,"");
    ListAddElement(list, 25);
    ListAddElement(list, 32);
    ListAddElement(list, 2);
    ListDeleteElement(list, 5);
    ListDeleteElement(list, 1);

    DestroyList(&list);

    return 0;
}
