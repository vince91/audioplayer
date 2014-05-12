//
//  waveform.h
//  audioplayer
//
//  Created by Vincent Timofti on 12/05/2014.
//
//

#ifndef __audioplayer__waveform__
#define __audioplayer__waveform__

#define WAVEFORM_SIZE 2000

#include <iostream>
#include <vector>

class Waveform
{
public:
    void add(float);
    int getSize() const {return waveform.size(); }
    
    void test();
    
private:
    std::vector<float> waveform;
    
};

#endif /* defined(__audioplayer__waveform__) */
