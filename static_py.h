#ifndef STATIC_PY_H
#define STATIC_PY_H

#include <stdio.h>
#include <stdlib.h>

void* static_py_init();
void static_py_execute(void* context, char* data);
void static_py_destroy(void*);


#endif // STATIC_PY_H
