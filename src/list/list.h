#ifndef LIST_H
#define LIST_H

#include <stdio.h>

struct list_t;

/**
 * \brief Enum class for returning errors
 */

enum list_return_e
{
    LIST_RETURN_SUCCESS,            /*!<Successfull attemp*/
    LIST_RETURN_ALLOCATION_ERROR,   /*!<If allocation was failed*/
    LIST_RETURN_FILE_OPEN_ERROR,    /*!<Programm failed to open the file*/
    LIST_RETURN_FILE_CLOSE_ERROR,   /*!<Programm failed to close the file safely*/
    LIST_RETURN_INCORRECT_VALUE,    /*!<Function got incorrect value*/
    LIST_RETURN_NULL_POINTER,       /*!<One of the pointers in list is null*/
    LIST_RETURN_CANARY_DAMAGED,     /*!<Canary was damaged*/
    LIST_RETURN_NOT_CYCLE,          /*!<List became not cycled*/
    LIST_RETURN_ELEMENT_COUNT_ERROR,/*!<Something happened with element count*/
};

/**
 * \brief Data type which uses list for storing values
 */
typedef double data_type;

// =================== MEMORY_CONTROL =====================

/**
 * \brief Init function for list
 * \param start_list_size *size_t* start list capacity
 * \param[out] list *list_t*** dst where struct will be storaged
 * \return *list_return_e* returns the result of initalization
 */
list_return_e InitList(list_t** list, size_t start_list_size);
/**
 * \brief Destructor function for list
 * \param list *list_t*** list that should be destroyed
 * \return *list_return_e* returns the result of destruction
 */
list_return_e DestroyList(list_t** list);

// =================== ELEMENT_CONTROL ====================
/**
 * \brief Function for adding elements to list
 * Adds the element in the nearest one free position in list.
 * \param list *list_t** list where the element will be added
 * \param[in] value *data_type* value of the element that should be added
 * \return *list_return_e* returns the result of adding
 */
list_return_e ListAddElement(list_t* list, data_type value);
/**
 * \brief Function for adding elements to list
 * Adds the element after defined index in list.
 * \param list *list_t** list where the element will be added
 * \param[in] value *data_type* value of the element that should be added
 * \param index *ssize_t* the index after which element will be placed
 * \return *list_return_e* returns the result of adding
 */
list_return_e ListAddAfterElement(list_t* list, data_type value, size_t index);
/**
 * \brief Function for deleting elements from list
 * Deletes the element on *index* position
 * \param list *list_t** list where the element will be deleted
 * \param[in] value *data_type* value of the element that should be added
 * \return *list_return_e* returns the result of deleting
 */
list_return_e ListDeleteElement(list_t* list, size_t  index);

// ================= ELEMENTS_NAVIGATION ==================

/**
 * \brief Function for getting indo
 * Function for getting element value.
 * \param list *list_t**
 * \param element_index *size_t* index of outcome value
 * \param[out] value *data_type** pointer for returning a value
 * \return *list_return_e* returns the result of adressing
 */
list_return_e GetElementValue(list_t* list, size_t element_index, data_type* value);
/**
 * \brief Navigation function
 * Function for getting index of next element.
 * \param list *list_t**
 * \param element_index *size_t*
 * \return *ssize_t* index of the next element
 */
ssize_t GetNextElement(list_t* list, size_t element_index);
/**
 * \brief Navigation function
 * Function for getting index of previous element.
 * \param list *list_t**
 * \param element_index *size_t*
 * \return *ssize_t* index of the previous element
 */
ssize_t GetPreviousElement(list_t* list, size_t element_index);
/**
 * \brief Navigation function
 * Function for getting index of the first list element.
 * \param list *list_t**
 * \return *ssize_t* index of the first element
 */
ssize_t GetHeadElement(list_t* list);
/**
 * \brief Navigation function
 * Function for getting index of the last list element.
 * \param list *list_t**
 * \return *ssize_t* index of the last element
 */
ssize_t GetTailElement(list_t* list);

// ====================== LIST_LOG ========================
/**
 * \brief Navigation function
 * Function for getting index of the last list element.
 * \param comment *const char** comment for adding in dump.
 * \param list *list_t**
 */
list_return_e ListDump(list_t* list, const char* comment);

FILE* GetLogFile(); // single tone

//======================= VERIFY ==========================
/**
 * \brief List Verification function
 */
list_return_e ListVerify(list_t* list);

/**
 * If defined *NDEBUG* no verification during calculatuons will be done
 */
#ifndef NDEBUG
#define VERIFY_RET(X) if (ListVerify(X) != 0) {return ListVerify(X);}
#else
#define VERIFY_RET(X)
#endif


//=========================================================

#endif //LIST_H
