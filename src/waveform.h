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
    void resize(int);
    int getSize() const {return waveform.size(); }
    const std::vector<float> & getResizedWaveform() { return resizedWaveform; }
    void clear();
    
    
    void test();
    
private:
    std::vector<float> waveform;
    std::vector<float> resizedWaveform;
    bool valid = false;
    
};

#endif /* defined(__audioplayer__waveform__) */
