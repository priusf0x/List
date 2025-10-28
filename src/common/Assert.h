#ifndef ASSERT_H
#define ASSERT_H
#include <stdio.h>

#ifndef NDEBUG
#define ASSERT(X) \
if (!(X)) {fprintf(stderr ,"ERROR(" #X ") in %d line in file %s",\
           __LINE__, __FILE__); abort();}
#else
#define ASSERT(X)
#endif

#endif //ASSERT_H

