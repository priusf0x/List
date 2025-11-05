#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "Assert.h"
#include "tools.h"

const ssize_t CANARY_SIZE = 4;
const uint64_t CANARY_FILL = 0xBAB1BAB0BAB1BAB0;

const int NO_LINK = -1;
const data_type EMPTY_ELEMENT = 0;

struct list_element_t
{
    data_type element;
    ssize_t next;
    ssize_t previous;
};

struct list_t
{
    void* canary_start;
    list_element_t* data;
    void* canary_end;
    ssize_t free;
    size_t elements_count;
    size_t elements_capacity;
    size_t real_size_in_bytes;
    size_t dump_count;
};

static list_return_e SetCanary(void* pointer, uint64_t value);
static list_return_e NumerizeElements(list_t* list, int start_index);
static list_return_e IncreaseCapacity(list_t* list);
static list_return_e ListDot(list_t* list);

//====================== ACTS_WITH_LIST ===========================

list_return_e
InitList(list_t** list,
         size_t   start_list_size)
{
    ASSERT(list != NULL)

    *list = (list_t*) calloc(1, sizeof(list_t));

    if (*list == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    (*list)->real_size_in_bytes = 2 * sizeof(uint64_t) * CANARY_SIZE
                               + start_list_size * sizeof(list_element_t);

    (*list)->canary_start = calloc((*list)->real_size_in_bytes, sizeof(uint8_t));

    if ((*list)->canary_start == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    SetCanary((*list)->canary_start, CANARY_FILL);

    (*list)->data = (list_element_t*) ((uint8_t*)(*list)->canary_start
                                       + CANARY_SIZE * sizeof(uint64_t));

    (*list)->free = 1;
    (*list)->elements_count = 0;
    (*list)->elements_capacity = start_list_size;

    ssize_t memory_alignment = (ssize_t) ((*list)->data + (*list)->elements_capacity);

    while (memory_alignment % 8 != 0)
    {
        memory_alignment++;
    }

    SetCanary((void*) memory_alignment, CANARY_FILL);

    (*list)->canary_end = (void*) memory_alignment;

    NumerizeElements(*list, 1);

    ((*list)->data[0]).next = 0;
    ((*list)->data[0]).previous = 0;

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListAddElement(list_t* list,
               data_type value)
{
    ASSERT(list != NULL);
    VERIFY_RET(list);

    list_return_e output = LIST_RETURN_SUCCESS;

    if (list->elements_count >= list->elements_capacity - 2)
    {
        if ((output = IncreaseCapacity(list)) != LIST_RETURN_SUCCESS)
        {
            return output;
        }
    }

    list->data[list->free].element = value;
    list->data[list->free].previous = list->data[0].previous;
    list->data[list->data[0].previous].next = list->free;
    list->data[0].previous = list->free;
    list->free = list->data[list->data[0].previous].next;
    list->data[list->data[0].previous].next = 0;
    list->elements_count += 1;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListAddAfterElement(list_t*   list,
                    data_type value,
                    int       index)
{
    ASSERT(list != NULL);
    VERIFY_RET(list);

    if ((index >= (int) list->elements_capacity)
        || (index < 0)
        || (list->data[index].previous == NO_LINK))
    {
        return LIST_RETURN_INCORRECT_VALUE;
    }

    list_return_e output = LIST_RETURN_SUCCESS;
    if (list->elements_count >= list->elements_capacity - 2)
    {
        if ((output = IncreaseCapacity(list)) != LIST_RETURN_SUCCESS)
        {
            return output;
        }
    }

    ssize_t intermediate_value = 0;
    list->data[list->free].element = value;
    list->data[list->free].previous = index;

    intermediate_value = list->data[index].next;
    list->data[index].next = list->free;
    list->data[intermediate_value].previous = list->free;
    list->free = list->data[list->free].next;

    list->data[list->data[index].next].next = intermediate_value;

    list->elements_count++;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListDeleteElement(list_t* list,
                  size_t  index)
{
    ASSERT(list != NULL)
    VERIFY_RET(list);

    if ((index >= list->elements_capacity)
        || (index == 0)
        || (list->data[index].previous == NO_LINK))
    {
        return LIST_RETURN_INCORRECT_VALUE;
    }

    list->data[list->data[index].previous].next = list->data[index].next;
    list->data[list->data[index].next].previous = list->data[index].previous;

    list->data[index].element = EMPTY_ELEMENT;
    list->data[index].previous = NO_LINK;
    list->data[index].next = list->free;
    list->free = (int) index;

    list->elements_count--;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

list_return_e
DestroyList(list_t** list)
{
    if ((list != NULL) && (*list != NULL))
    {
        free((*list)->canary_start);
        memset(*list, 0, sizeof(list_t));

        free(*list);

        *list = NULL;
    }

    if (fclose(GetLogFile()) != 0)
    {
        return LIST_RETURN_CLOSE_FILE_ERROR;
    }

    return LIST_RETURN_SUCCESS;
}
// ================== NAVIGATION_IN_LIST =======================

list_return_e
GetElementValue(list_t*    list,
                size_t     element_index,
                data_type* value)
{
    ASSERT(list);
    ASSERT(value);
    VERIFY_RET(list);

     if ((element_index > list->elements_capacity)
        || (list->data[element_index].previous == NO_LINK)
        || (element_index == 0))
    {
        return LIST_RETURN_UNDEFINED_ELEMENT;
    }

    *value = list->data[element_index].element;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

ssize_t
GetNextElement(list_t* list,
               size_t  element_index)
{
    ASSERT(list);
    VERIFY_RET(list);

    if ((element_index > list->elements_capacity)
        || (list->data[element_index].previous == NO_LINK)
        || (element_index == 0))
    {
        return NO_LINK;
    }

    VERIFY_RET(list);

    return list->data[element_index].next;
}

ssize_t
GetPreviousElement(list_t* list,
                   size_t  element_index)
{
    ASSERT(list);
    VERIFY_RET(list);

    if ((element_index > list->elements_capacity)
        || (list->data[element_index].previous == NO_LINK)
        || (element_index == 0))
    {
        return NO_LINK;
    }

    VERIFY_RET(list);

    return list->data[element_index].previous;
}

ssize_t
GetHeadElement(list_t* list)
{
    ASSERT(list);
    VERIFY_RET(list);

    if (list->elements_capacity == 0)
    {
        return NO_LINK;
    }

    VERIFY_RET(list);

    return list->data[0].next;
}

ssize_t
GetTailElement(list_t* list)
{
    ASSERT(list);
    VERIFY_RET(list);

    if (list->elements_capacity == 0)
    {
        return NO_LINK;
    }

    VERIFY_RET(list);

    return list->data[0].previous;
}


// ================== LOGGER =============================

const char* LOG_FILE_NAME = "logs/log_file.htm";

static void PrintHTMLHeader(list_t* list, FILE* log_file);
static void PrintListInfo(list_t* list, FILE* log_file);
static void PrintElementsInfo(list_t* list, FILE* log_file);
static void PrintBytesInfo(list_t* list, FILE* log_file);

FILE*
GetLogFile()
{
    static FILE* log_file = fopen(LOG_FILE_NAME, "w+");
    return log_file;
}

list_return_e
ListDump(list_t*     list,
         const char* comment)
{
    ASSERT(list != NULL);
    ASSERT(comment != NULL);

    list->dump_count++;

    FILE* log_file = GetLogFile();
    if (log_file == NULL)
    {
        return LIST_RETURN_FILE_OPEN_ERROR;
    }

    PrintHTMLHeader(list, log_file);
    fprintf(log_file, "<h4>Comment:\"%s\"</h4>", comment);
    ListDot(list);
    PrintListInfo(list, log_file);
    PrintElementsInfo(list, log_file);
    PrintBytesInfo(list,log_file);

    return LIST_RETURN_SUCCESS;
}

// ================= LIST_VERIFY ======================

static bool CheckCanary(list_t* list);
static list_return_e CheckElements(list_t* list, size_t* real_count);

list_return_e
ListVerify(list_t* list)
{
    size_t real_count = 0;

    if ((list->canary_start == NULL)
        || (list->data == NULL)
        || (list->canary_end == NULL))
    {
        ListDump(list, "Null arg");
        return LIST_RETURN_NULL_POINTER;
    }
    else if (!CheckCanary(list))
    {
        ListDump(list, "Canary Error");
        return LIST_RETURN_CANARY_DAMAGED;
    }
    else if (CheckElements(list, &real_count) != LIST_RETURN_SUCCESS)
    {
        ListDump(list, "Wrong Elements");
        return LIST_RETURN_NOT_CYCLE;
    }
    else if (real_count != list->elements_count)
    {
        ListDump(list, "Count Error");
        return LIST_RETURN_ELEMENT_COUNT_ERROR;
    }

    return LIST_RETURN_SUCCESS;
}

//============= HELPER_FUNCTIONS ======================

static list_return_e
IncreaseCapacity(list_t* list)
{
    VERIFY_RET(list);

    SetCanary(list->canary_end, 0);
    (list->canary_start) = (list_element_t*) recalloc(list->canary_start,
                                                      list->real_size_in_bytes,
                                                      list->real_size_in_bytes
                                                      + sizeof(list_element_t)
                                                      * list->elements_capacity);

    if (list->canary_start == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    list->real_size_in_bytes += sizeof(list_element_t) * list->elements_capacity;

    list->data = (list_element_t*) ((uint8_t*) list->canary_start
                                    + sizeof(uint64_t) * CANARY_SIZE);
    list->elements_capacity *= 2;

    NumerizeElements(list, (int) list->elements_count + 1);

    list->free = (int) list->elements_count + 1;

    ssize_t memory_alignment = (ssize_t) (list->data + list->elements_capacity);

    while (memory_alignment % 8 != 0)
    {
        memory_alignment++;
    }

    SetCanary((void*) memory_alignment, CANARY_FILL);

    list->canary_end = (list_element_t*) memory_alignment;

    VERIFY_RET(list);

    return LIST_RETURN_SUCCESS;
}

static list_return_e
SetCanary(void*    pointer,
          uint64_t value)
{
    for (ssize_t index = 0; index < CANARY_SIZE; index++)
    {
        ((uint64_t*) pointer)[index] = value;
    }

    return LIST_RETURN_SUCCESS;
}

static list_return_e
NumerizeElements(list_t* list,
                 int     start_index)
{
    for (int index = start_index; index < (int) list->elements_capacity; index++)
    {
        (list->data[index]).next = index + 1;
        (list->data[index]).previous = NO_LINK;
    }

    list->data[list->elements_capacity - 1].next = NO_LINK;

    return LIST_RETURN_SUCCESS;
}

static bool
CheckCanary(list_t* list)
{
    for (size_t index = 0; index < CANARY_SIZE; index++)
    {
        if ((((uint64_t*) list->canary_start)[index] != CANARY_FILL) ||
        (((uint64_t*) list->canary_end)[index] != CANARY_FILL))
        {
            return false;
        }
    }

    return true;
}

static list_return_e
CheckElements(list_t* list,
              size_t* real_count)
{
    ssize_t current_elem = 0;
    ssize_t next_elem = 0;

    size_t count = 0;
    for (; count <= list->elements_count; count++)
    {
        next_elem = list->data[current_elem].next;

        if ((next_elem == NO_LINK) || (list->data[next_elem].previous != current_elem))
        {
            return LIST_RETURN_NOT_CYCLE;
        }

        current_elem = next_elem;
    }

    if (next_elem != 0)
    {
        return LIST_RETURN_ELEMENT_COUNT_ERROR;
    }

    *real_count = count - 1;

    return LIST_RETURN_SUCCESS;
}

//================= BETTER_NOT_TO_WATCH ====================

static void DrawFilledElement(list_t* list, size_t  index, FILE* dot_file);
static void DrawEmptyElement(list_t* list, size_t  index, FILE* dot_file);
static void DrawInfoElements(list_t* list, FILE* dot_file);

static list_return_e
ListDot(list_t* list)
{
    const ssize_t max_string_size = 20;

    char name_template[max_string_size] = {};

    snprintf(name_template, max_string_size - 1, "logs/%zu.gv", list->dump_count);
    FILE* dot_file = fopen(name_template, "w+");

    if (dot_file == NULL)
    {
        return LIST_RETURN_FILE_OPEN_ERROR;
    }

    fprintf(dot_file, "graph  G{ bgcolor = \"#303030\";"
                      "splines = ortho; node [pin = \"true\", shape = box,"
                       "style = filled,]");

    for (size_t index = 1; index < list->elements_capacity; index++)
    {
        if  (list->data[index].previous != NO_LINK)
        {
            DrawFilledElement(list, index, dot_file);
        }
        else
        {
            DrawEmptyElement(list, index, dot_file);
        }
    }

    DrawInfoElements(list, dot_file);

    if (fclose(dot_file) != 0)
    {
        return LIST_RETURN_FILE_CLOSE_ERROR;
    }

    const ssize_t max_command_size = 200;
    char command[max_command_size] = {};

    snprintf(command, max_command_size - 1, "neato -Tpng logs/%zu.gv -o"
             "logs/%zu.png", list->dump_count,
             list->dump_count);

    system(command);

    return LIST_RETURN_SUCCESS;
}

// ======================= PRINT_INFO_FUNCTION ===================

static void
PrintHTMLHeader(list_t* list, FILE* log_file)
{
    fprintf(log_file, "<html>\n"
                        "<style>"
                        "body{background-color: rgb(48, 48, 48);}"
                        "h1{color: rgb(212, 58, 56);}"
                        "h2{color: rgba(153, 26, 24, 1);}"
                        "h4{color: rgb(182, 182, 182);}"
                        "</style>"
                        "<h1> LIST_DUMP No-%zu</h1>\n",
                        list->dump_count);
}

static void
PrintListInfo(list_t* list,
              FILE* log_file)
{
    const ssize_t max_string_size = 50;
    char img_template[max_string_size] = {};
    snprintf(img_template, max_string_size - 1, "<img src=\"%zu.png\","
                                                "height = \"20%%\">",
                                                list->dump_count);
    fprintf(log_file, "%s", img_template);

    fprintf(log_file, "<p><h4>List element capacity:............................%zu<br/>",
            list->elements_capacity);
    fprintf(log_file, "List element count:.................................%zu<br/>",
            list->elements_count);
    fprintf(log_file, "List free element number:.....................%ld<br/>",
            list->free);
    fprintf(log_file, "List element capacity in bytes:..............%zu</h4>",
            list->real_size_in_bytes);
    fprintf(log_file,"<h2>LIST_ELEMENTS</h2>");
}

static void
PrintElementsInfo(list_t* list,
                  FILE* log_file)
{
    for (size_t index = 0; index < list->elements_capacity; index++)
        {
            fprintf(log_file,
                    "<h4><p><li>index    = %4zu<br/>"
                    "value    = %4f<br/>"
                    "previous = %4ld<br/>"
                    "next     = %4ld<br/></p></li></h4>", index,
                    list->data[index].element,
                    list->data[index].previous, list->data[index].next);
        }
}

static void
PrintBytesInfo(list_t* list, FILE* log_file)
{
    fprintf(log_file, "<h2>BYTE_LEGEND</h2><table style ="
                      "\"color:rgb(182, 182, 182);><tr>\"");

    for (size_t index = 0; index < list->real_size_in_bytes; index++)
    {
        if (index % 16 == 0)
        {
            fprintf(log_file, "</tr><tr><td><span style=\"color:"
                              "rgb(212, 58, 56)\"> %p:   </span></td>",
                              (uint8_t*) list->data + index);
        }

        fprintf(log_file, "<td>%4x</td>",
                ((uint8_t*) list->canary_start)[index]);
    }

    fprintf(log_file, "</tr></table>");
}

static void
DrawFilledElement(list_t* list,
                  size_t  index,
                  FILE* dot_file)
{
    fprintf(dot_file, "p%zu[ fillcolor = \"#949494\","
                        "label = \"prev = %ld\", width = 1.8"
                        ",pos = \"%zu.05, 10!\"];\n", index,
                        list->data[index].previous, 4 + 5 * index);

    fprintf(dot_file, "i%zu[fillcolor =\"#b6b6b6ff\","
                        "label = \"index = %zu\", width = 3.7,"
                        "pos = \"%zu, 11.2!\"];", index,
                        index, 5 + 5 * index);

    fprintf(dot_file, "v%zu[fillcolor =\"#b16261\","
                        "label = \"value = %f\"width = 3.7, "
                        "pos = \"%zu,10.6!\"];", index,
                        list->data[index].element, 5 + 5 * index);

    fprintf(dot_file, "n%zu[fillcolor =\"#949494\","
                        "label = \"next = %ld\", width = 1.8,"
                        " pos = \"%zu.95,10!\"];",
                        index, list->data[index].next,5 + 5 * index);

    fprintf(dot_file, "inv%zu[style=\"invis\","
                        "height = 2, pos = \"%zu.5, 11!\"];",
                        index, 7 + 5 * index);

    if (list->data[index].next != 0)
    {
        fprintf(dot_file, "n%zu -- p%ld[color = \"#d1d1d1\", dir = both];",
                index, list->data[index].next);
    }
}

static void
DrawEmptyElement(list_t* list,
                 size_t  index,
                 FILE* dot_file)
{
    fprintf(dot_file, "p%zu[fillcolor = \"#818181ff\","
                      "label = \"prev = %ld\", width = 1.8"
                      ",pos = \"%zu.05, 10!\"];", index,
                      list->data[index].previous, 4 + 5 * index);

    fprintf(dot_file, "i%zu[fillcolor =\"#818181ff\","
                      "label = \"index = %zu\","
                      "width = 3.7,pos = \"%zu, 11.2!\"];", index,
                      index, 5 + 5 * index);

    fprintf(dot_file, "v%zu[fillcolor =\"#818181ff\","
                      "label = \"value = %f\",width = 3.7,"
                      "pos = \"%zu,10.6!\"];", index,
                      list->data[index].element, 5 + 5 * index);

    fprintf(dot_file, "n%zu[fillcolor =\"#818181ff\","
                      "label = \"next = %ld\", width = 1.8,"
                      " pos = \"%zu.95,10!\"];", index,
                      list->data[index].next,5 + 5 * index);

    fprintf(dot_file, "inv%zu[style=\"invis\","
                      "height = 2, pos = \"%zu.5, 11!\"];",
                      index, 7 + 5 * index);

    if (list->data[index].next != NO_LINK)
    {
        fprintf(dot_file, "n%zu -- p%ld[color = \"#aaaaaa96\","
                "dir = forward];", index, list->data[index].next);
    }
}

static void
DrawInfoElements(list_t* list,
                 FILE* dot_file)
{
    if (list->elements_count != 0)
    {
        fprintf(dot_file, "head[fillcolor =\"#646464ff\","
                          "label = \"head = %ld\""
                          ",width = 2, pos = \"%ld,11.8!\"];",
                          list->data[0].next, 5 + 5 * list->data[0].next);

        fprintf(dot_file, "head -- i%ld[color = \"#d1d1d1\", dir = forward];",
                          list->data[0].next);

        fprintf(dot_file, "tail[fillcolor =\"#646464ff\","
                          "label = \"tail = %ld\""
                          ",width = 2, pos = \"%ld,12.4!\"];",
                          list->data[0].previous, 5 + 5 * list->data[0].previous);

        fprintf(dot_file, "tail -- i%ld[color = \"#d1d1d1\",dir = forward];",
                          list->data[0].previous);
    }

    fprintf(dot_file, "free[fillcolor = \"#646464ff\","
                      "label = \"free = %ld\""
                      ",width = 2, pos = \"%ld,13!\"];",
                      list->free, 5 + 5 * list->free);

    fprintf(dot_file, "free -- i%ld[color = \"#d1d1d1\",dir = forward];",
                      list->free);

    fprintf(dot_file, "}");
}



