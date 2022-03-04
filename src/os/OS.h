#ifndef OS_H
#define OS_H

#include "include.h"

class OS {
    virtual bool setup(string node) { return true; }
    virtual bool teardown(string node) { return true; }
};



#endif