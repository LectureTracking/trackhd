//
// Created by Charles Fitzhenry on 2016/08/09.
//

#ifndef TRACK4K_TRACK4KPREPROCESS_H
#define TRACK4K_TRACK4KPREPROCESS_H

#include "../FileReader.h"
#include "../MetaFrame.h"
#include "BoardDetection.h"
#include "../PersistentData.h"


class Track4KPreProcess {

public:

    void preProcessDriver(PersistentData &persistentData);
};


#endif //TRACK4K_TRACK4KPREPROCESS_H
