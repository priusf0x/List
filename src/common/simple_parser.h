#ifndef SIMPLE_PARSER_H
#define SIMPLE_PARSER_H

enum read_flags_return_e
{
    READ_FLAGS_RETURN_SUCCESS,
    READ_FLAGS_RETURN_ERROR
};

enum read_flags_return_e ReadFlags(int                argc,
                                   const char* const* argv,
                                   const char** input_file,
                                   const char** output_file);

#endif
