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
    static void resize(int);
    int getSize() const {return waveform.size(); }
    static const std::vector<float> & getResizedWaveform() { return resizedWaveform; }
    static float getMax();
    
    
    static void test();
    
private:
    static std::vector<float> waveform;
    static std::vector<float> resizedWaveform;
    static bool valid;
    
};

#endif /* defined(__audioplayer__waveform__) */
