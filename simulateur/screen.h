#ifndef H_SCREEN
#define H_SCREEN


#include "structs.h"
#include <thread>
#include <iostream>
#include <cstdio>
class Screen {
    public:
        virtual ~Screen() {};
        virtual void updateContent(uword *memory) = 0;
};



#endif
