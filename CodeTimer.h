//
// Created by charles on 2016/07/20.
//

#ifndef TRACK4K_CODETIMER_H
#define TRACK4K_CODETIMER_H

#include <string>
#include <iostream>
#include <time.h>

class CodeTimer {
private:
    clock_t t;

public:
    void start();

    int stop(bool printText);

    float stop(int g);

    int stop();

};

#endif //TRACK4K_CODETIMER_H
