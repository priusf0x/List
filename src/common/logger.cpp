#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "stack.h"
#include "tools.h"


const size_t LOG_BUFFER_SIZE = 100;
enum detalization_levels_e DETALIZATION_LEVEL = DETALIZATION_LEVEL_DEBUG;
char LOG_BUFFER[LOG_BUFFER_SIZE] = {0};

FILE*
GetLogFile()
{
    static FILE * log_file = OpenLogFile();
    return log_file;
}

FILE*
OpenLogFile()
{
    FILE * log_file = fopen(LOG_FILE, "a+");
    if (log_file == NULL)
    {
        fprintf(stderr, "FAILED TO READ FILE\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    fprintf(log_file, "\n-------------------------------------\n");
    return log_file;
}

log_function_return_value_e
LogMessage(const char * log_message,
           enum detalization_levels_e detalization_level)
{
    FILE * log_file = GetLogFile();

    if ((log_message == NULL) || (log_file == NULL))
    {
        return LOG_FUNCTION_NULL_POINTER_ERROR;
    }

    if (detalization_level <= DETALIZATION_LEVEL)
    {
        time_t t1 = time(NULL);
        tm t = *localtime(&t1);
        fprintf(log_file, "\n <%.2d:%.2d:%.2d> ",
                t.tm_hour, t.tm_min, t.tm_sec);
        fprintf(log_file, "%s", log_message);
    }

    return LOG_FUNCTION_SUCCESS;
}

void
SwitchDetailLevelTo(enum detalization_levels_e detalization_level)
{
    DETALIZATION_LEVEL = detalization_level;
}








