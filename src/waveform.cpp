//
//  waveform.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 12/05/2014.
//
//

#include "waveform.h"
#include <cmath>


void Waveform::add(float value)
{
    waveform.push_back(value);
}

void Waveform::test()
{
    for (unsigned int i = 0; i < waveform.size(); ++i) {
        std::cout << waveform[i] << ",";
        
    }
    std::cout << std::endl;
}

void Waveform::resize(int width)
{
    resizedWaveform.clear();
    unsigned int size = waveform.size();
    
    if (size == 0)
        return;
    
    float z;
    for (int i = 0; i < width; ++i) {
        z = i * (size - 1) / (width - 1);
        resizedWaveform.push_back(waveform[round(z)]);
    }
    
    //std::cout << width << ";" << resizedWaveform.size() << std::endl;
}

void Waveform::clear()
{
    waveform.clear();
    resizedWaveform.clear();
    valid = false;
}