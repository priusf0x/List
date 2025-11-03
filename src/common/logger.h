#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

enum detalization_levels_e
{
    DETALIZATION_LEVEL_ERROR = 0,
    DETALIZATION_LEVEL_DEBUG = 1
};

extern const ssize_t LOG_BUFFER_SIZE;
extern char LOG_BUFFER[];
extern enum detalization_levels_e DETALIZATION_LEVEL;
const char LOG_FILE[] = "logs/Logs.txt";


enum log_function_return_value_e
{
    LOG_FUNCTION_SUCCESS,
    LOG_FUNCTION_NULL_POINTER_ERROR
};

FILE*                       GetLogFile();
FILE*                       OpenLogFile();
log_function_return_value_e LogMessage(const char * log_message, enum detalization_levels_e detalization_level);
void                        SwitchDetailLevelTo(enum detalization_levels_e detalization_level);

#define LOGSHIT(det_level, ...)\
    snprintf(LOG_BUFFER, LOG_BUFFER_SIZE, __VA_ARGS__);\
    LogMessage(LOG_BUFFER, det_level);

#endif //LOGGER_H
