#include "list.h"

#include <stdio.h>

int main(void)
{
    list_t* list = NULL;

    InitList(&list, 2);
    ListDump(list, "bar");
    ListAddElement(list, 12);
    ListDump(list, "foo");

    ListAddAfterElement(list, 69, 0);
    ListAddElement(list, 12);
    ListAddElement(list, 11);
    ListAddElement(list, 25);
    ListAddElement(list, 32);
    ListAddElement(list, 2);
    ListDeleteElement(list, 5);
    ListDeleteElement(list, 1);

    ListDump(list, "meow");

    DestroyList(&list);

    return 0;
}
