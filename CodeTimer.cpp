//
// Created by charles on 2016/07/20.
//
#include "CodeTimer.h"

void CodeTimer::start() {
    t = clock();
}

int CodeTimer::stop(bool printText)
{
    t = clock() - t;
    if(printText){
        std::cout << "*** Time taken: " << (((float) t) / CLOCKS_PER_SEC) * 1000 << "ms (" <<
                  (((float) t) / CLOCKS_PER_SEC) << " sec) ***" << std::endl;
    }

    return  ((float)t / CLOCKS_PER_SEC);


}

int CodeTimer::stop()
{
    t = clock() - t;
    return  ((int)t / CLOCKS_PER_SEC);


}

float CodeTimer::stop(int g){

    t = clock() - t;

    return  ((float)t / CLOCKS_PER_SEC);
}



