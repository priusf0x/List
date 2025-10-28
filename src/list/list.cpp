#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "Assert.h"
#include "tools.h"
#include "color.h"

const size_t CANARY_SIZE = 4;
const uint64_t CANARY_FILL = 0xB16B00B5;

const int NO_LINK = -1;
const uint8_t EMPTY_BYTE = 0;
const data_type EMPTY_ELEMENT = 0;
const size_t START_LIST_SIZE = 3;

struct list_element_t
{
    data_type element;
    int next;
    int previous;
};

struct list_t
{
    list_element_t* canary_start;
    list_element_t* data;
    list_element_t* canary_end;
    int free;
    size_t elements_count;
    size_t elements_capacity;
    size_t real_size_in_bytes;
};

static list_return_e SetCanary(void* pointer, uint64_t value);
static list_return_e NumerizeElements(list_t* pointer);
static list_return_e IncreaseCapacity(list_t* list);

//====================== ACTS_WITH_LIST ===========================

list_return_e
InitList(list_t** list)
{
    *list = (list_t*) calloc(1, sizeof(list_t));

    if (*list == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    (*list)->real_size_in_bytes = 2 * sizeof(uint64_t) * CANARY_SIZE
                               + START_LIST_SIZE * sizeof(list_element_t);

    (*list)->canary_start = (list_element_t*) malloc((*list)->real_size_in_bytes);

    if ((*list)->canary_start == NULL)
    {
        return LIST_RETURN_ALLOCATION_ERROR;
    }

    memset((*list)->canary_start, EMPTY_BYTE, (*list)->real_size_in_bytes);
    SetCanary((*list)->canary_start, CANARY_FILL);

    (*list)->data = (list_element_t*) ((uint8_t*) (*list)->canary_start
                                    + CANARY_SIZE * sizeof(uint64_t));

    (*list)->free = 1;
    (*list)->elements_count = 0;
    (*list)->elements_capacity = START_LIST_SIZE;

    size_t memory_alignment = (size_t) ((*list)->data+ (*list)->elements_capacity);

    while (memory_alignment % 8 != 0)
    {
        memory_alignment++;
    }

    SetCanary((void*) memory_alignment, CANARY_FILL);

    (*list)->canary_end = (list_element_t*) memory_alignment;

    NumerizeElements((*list));

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListAddElement(list_t* list,
               data_type value)
{
    ASSERT(list != NULL)

    list_return_e output = LIST_RETURN_SUCCESS;

    if (list->elements_count == list->elements_capacity - 1)
    {
        if ((output = IncreaseCapacity(list)) != LIST_RETURN_SUCCESS)
        {
            return output;
        }
    }

    list->data[list->free].element = value;
    list->data[list->free].previous = list->data[0].previous;
    list->data[list->data[0].previous].next = (int) list->free;
    list->data[0].previous = (int) list->free;
    list->free = (int) fabs(list->data[list->data[0].previous].next);
    list->data[list->data[0].previous].next = 0;
    list->elements_count += 1;

    return LIST_RETURN_SUCCESS;
}

list_return_e
ListDeleteElement(list_t* list,
                  size_t  index)
{
    if ((index > list->elements_capacity) || (list->data[index].previous == NO_LINK))
    {
        return LIST_RETURN_INCORRECT_VALUE;
    }

    list->data[list->data[index].previous].next = list->data[index].next;
    list->data[list->data[index].next].previous = list->data[index].previous;

    list->data[index].element = EMPTY_ELEMENT   ;
    list->data[index].previous = NO_LINK;
    list->data[index].next = -list->free;
    list->free = (int) index;

    return LIST_RETURN_SUCCESS;
}

list_return_e
DestroyList(list_t** list)
{
    if ((list != NULL) && (*list != NULL))
    {
        free((*list)->canary_start);
        memset(*list, EMPTY_BYTE, sizeof(list_t));

        free(*list);

        *list = NULL;
    }

    return LIST_RETURN_SUCCESS;
}

// ================== VERIFICATION =============================

const char* LOG_FILE_NAME = "logs/log_file.htm";

FILE*
GetLogFile()
{
    static FILE* log_file = fopen(LOG_FILE_NAME, "w+");
    return log_file;
}

list_return_e
ListDump(list_t* list)
{
    FILE* log_file = GetLogFile();
    if (log_file == NULL)
    {
        return LIST_RETURN_FILE_OPEN_ERROR;
    }

    fprintf(log_file, "<html>\n"
                      "<h1> LIST_DUMP </h1>\n");

    fprintf(log_file, "<p>List element capacity:.......................%zu<br/>",
            list->elements_capacity);
    fprintf(log_file, "List element count:..........................%zu<br/>",
            list->elements_count);
    fprintf(log_file, "List free element number:....................%d<br/>",
            list->free);
    fprintf(log_file, "List element capacity in bytes:..............%zu</p>",
            list->real_size_in_bytes);

    fprintf(log_file,"<h2>LIST_ELEMENTS</h2>");

    for (size_t index = 0; index < list->elements_capacity; index++)
    {
        fprintf(log_file,
                "<p><li>index    = %4zu<br/>"
                "value    = %4f<br/>"
                "previous = %4d<br/>"
                "next     = %4d<br/></p></li>", index, list->data[index].element,
                list->data[index].previous, list->data[index].next);
    }

    fprintf(log_file, "<h2>BYTE_ELEMENTS</h2><table><tr>");

    for (size_t index = 0; index < list->real_size_in_bytes; index++)
    {
        if (index % 8 == 0)
        {
            fprintf(log_file, "</tr><tr>");
        }

        fprintf(log_file, "<td>[%4zu] %3d  </td>", index,
                ((uint8_t*) list->canary_start)[index]);
    }

    fprintf(log_file, "</tr>");

    return LIST_RETURN_SUCCESS;
}

//============= HELPER_FUNCTIONS ======================

static list_return_e
IncreaseCapacity(list_t* list)
{
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


    for (int index = (int) list->elements_count + 1;
            index < (int) list->elements_capacity; index++)
    {
        list->data[index].previous = NO_LINK;
        list->data[index].next = -(index + 1);
    }

    list->free = (int) list->elements_count + 1;

    size_t memory_alignment = (size_t) (list->data + list->elements_capacity);

    while (memory_alignment % 8 != 0)
    {
        memory_alignment++;
    }

    SetCanary((void*) memory_alignment, CANARY_FILL);

    list->canary_end = (list_element_t*) memory_alignment;

    return LIST_RETURN_SUCCESS;
}

static list_return_e
SetCanary(void* pointer,
          uint64_t value)
{
    for (size_t index = 0; index < CANARY_SIZE; index++)
    {
        ((uint64_t*) pointer)[index] = value;
    }

    return LIST_RETURN_SUCCESS;
}

static list_return_e
NumerizeElements(list_t* list)
{
    for (int index = 1; index < (int) list->elements_capacity; index++)
    {
        (list->data[index]).next = -(index + 1);
        (list->data[index]).previous = NO_LINK;
    }

    (list->data[0]).next = 0;
    (list->data[0]).previous = 0;

    (*(list->data + list->elements_capacity - 1)).next = 0;

    return LIST_RETURN_SUCCESS;
}

//================= BETTER_NOT_TO_WATCH ====================

list_return_e
ListDot(list_t* list,
        size_t  dump_number)
{
    const size_t max_string_size = 20;

    char name_template[max_string_size] = {};

    snprintf(name_template, max_string_size - 1, "logs/%zu.gv", dump_number);
    FILE* dot_file = fopen(name_template, "w+");

    if (dot_file == NULL)
    {
        return LIST_RETURN_FILE_OPEN_ERROR;
    }

    fprintf(dot_file, "digraph  G{ bgcolor = \"#303030\"; splines = ortho; overlap=\"0:\"");

    for (size_t index = 1; index < list->elements_capacity; index++)
    {
        if  (list->data[index].previous != NO_LINK)
        {
            fprintf(dot_file, "p%zu[shape = box, style = filled, fillcolor "
                              "= \"#949494\", label = \"prev = %d\", width = 1.8"
                              ",pos = \"%zu.05, 10!\"];\n", index,
                              list->data[index].previous, 5 + 5 * index - 1);

            fprintf(dot_file, "i%zu[shape = box, style = filled, fillcolor ="
                              " \"#b6b6b6ff\", label = \"index = %zu\","
                              "width = 3.7,pos = \"%zu, 11.2!\"];", index,
                              index, 5 + 5 * index);

            fprintf(dot_file, "v%zu[shape = box, style = filled, fillcolor ="
                              "\"#b16261\", label = \"value = %f\""
                              ",width = 3.7, pos = \"%zu,10.6!\"];",
                              index, list->data[index].element, 5 + 5 * index);

            fprintf(dot_file, "n%zu[shape = box, style = filled, fillcolor ="
                              " \"#949494\", label = \"next = %d\", width = 1.8,"
                              " pos = \"%zu.95,10!\"];",
                              index, list->data[index].next,5 + 5 * index);

            fprintf(dot_file, "inv%zu[shape = record, style=\"invis\","
                              "height = 2, pos = \"%zu.5, 11!\"];",
                              index, 2 + 5 * index);

            if (list->data[index].previous != 0)
            {
                fprintf(dot_file, "p%zu -> n%d[color = \"#d1d1d1\"];",
                        index, list->data[index].previous);
            }
            if (list->data[index].next != 0)
            {
                fprintf(dot_file, "n%zu -> p%d[color = \"#d1d1d1\"];",
                        index, list->data[index].next);
            }
        }
        else
        {
            fprintf(dot_file, "p%zu[shape = box, style = filled, fillcolor "
                              "= \"#818181ff\", label = \"prev = %d\", width = 1.8"
                              ",pos = \"%zu.05, 10!\"];", index,
                              list->data[index].previous, 5 + 5 * index - 1);

            fprintf(dot_file, "i%zu[shape = box, style = filled, fillcolor ="
                              " \"#818181ff\", label = \"index = %zu\","
                              "width = 3.7,pos = \"%zu, 11.2!\"];", index,
                              index, 5 + 5 * index);

            fprintf(dot_file, "v%zu[shape = box, style = filled, fillcolor ="
                              "\"#818181ff\", label = \"value = %f\""
                              ",width = 3.7, pos = \"%zu,10.6!\"];",
                              index, list->data[index].element, 5 + 5 * index);

            fprintf(dot_file, "n%zu[shape = box, style = filled, fillcolor ="
                              " \"#818181ff\", label = \"next = %d\", width = 1.8,"
                              " pos = \"%zu.95,10!\"];",
                              index, list->data[index].next,5 + 5 * index);

            fprintf(dot_file, "inv%zu[shape = box, style=\"invis\","
                              "height = 2, pos = \"%zu.5, 11!\"];",
                              index, 7 + 5 * index);
        }
    }

    if (list->elements_count != 0)
    {
        fprintf(dot_file, "head[shape = box, style = filled, fillcolor ="
                          "\"#646464ff\", label = \"head = %d\""
                          ",width = 2, pos = \"%d,11.8!\"];",
                          list->data[0].next, 5 + 5 * list->data[0].next);

        fprintf(dot_file, "head -> i%d[color = \"#d1d1d1\"];",
                          list->data[0].next);

        fprintf(dot_file, "tail[shape = box, style = filled, fillcolor ="
                          "\"#646464ff\", label = \"tail = %d\""
                          ",width = 2, pos = \"%d,12.4!\"];",
                          list->data[0].previous, 5 + 5 * list->data[0].previous);

        fprintf(dot_file, "tail -> i%d[color = \"#d1d1d1\"];",
                          list->data[0].previous);
    }

    fprintf(dot_file, "free[shape = box, style = filled, fillcolor ="
                      "\"#646464ff\", label = \"free = %d\""
                      ",width = 2, pos = \"%d,13!\"];",
                      list->free, 5 + 5 * list->free);

    fprintf(dot_file, "free -> i%d[color = \"#d1d1d1\"];",
                      list->free);

    fprintf(dot_file, "}");

    if (fclose(dot_file) != 0)
    {
        return LIST_RETURN_FILE_CLOSE_ERROR;
    }

    return LIST_RETURN_SUCCESS;
}

// static bool
// CheckCanary(swag_t* swag)
// {
//     for (size_t index = 0; index < CANARY_SIZE; index++)
//     {
//         if ((((uint64_t*) swag->canary_start)[index] != CANARY_FILL) ||
//         (((uint64_t*) swag->canary_end)[index] != CANARY_FILL))
//         {
//             return false;
//         }
//     }
//
//     return true;
// }
//
// static stack_function_errors_e
// StackNormalizeSize(swag_t* swag)
// {
//     if (swag->size == swag->capacity)
//     {
//         SetCanary(swag->canary_end, 0);
//         (swag->canary_start) = (uint8_t*) recalloc(swag->canary_start,
//                                                    swag->real_capacity_in_bytes,
//                                                    swag->real_capacity_in_bytes
//                                                    + sizeof(value_type) * swag->capacity);
//
//         swag->real_capacity_in_bytes += sizeof(value_type) * swag->capacity;
//
//         swag->stack_data = (value_type*) (swag->canary_start + sizeof(uint64_t) * CANARY_SIZE);
//         swag->capacity *= 2;
//
//         swag->canary_end = (uint8_t*) (swag->stack_data + swag->capacity);
//         while ((size_t) swag->canary_end % 8 != 0)
//         {
//             (swag->canary_end)++;
//         }
//         SetCanary(swag->canary_end, CANARY_FILL);
//     }
//     else if((2 * swag->size < swag->capacity) && (swag->capacity > 2 * swag->minimal_capacity))
//     {
//         SetCanary(swag->canary_end, 0);
//
//         swag->canary_start = (uint8_t*) realloc(swag->canary_start,
//                                                   swag->real_capacity_in_bytes
//                                                   - sizeof(value_type) * ((swag->capacity) / 2));
//
//         swag->real_capacity_in_bytes -= sizeof(value_type) * swag->capacity / 2;
//
//         swag->stack_data = (value_type*) (swag->canary_start + sizeof(uint64_t) * CANARY_SIZE);
//         swag->capacity -= swag->capacity / 2;
//
//         swag->canary_end = (uint8_t*) (swag->stack_data + swag->capacity);
//         while  ((size_t) swag->canary_end % 8 != 0)
//         {
//             (swag->canary_end)++;
//         }
//         SetCanary(swag->canary_end, CANARY_FILL);
//     }
//
//     return STACK_FUNCTION_SUCCESS;
// }
//


