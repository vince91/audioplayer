//
//  waveform.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 12/05/2014.
//
//

#include "waveform.h"
#include <cmath>

bool Waveform::valid = false;
std::vector<float> Waveform::waveform;
std::vector<float> Waveform::resizedWaveform;

void Waveform::add(float value)
{
    waveform.push_back(value);
}

void Waveform::test()
{
    for (int i = 0; i < resizedWaveform.size(); ++i) {
        std::cout << resizedWaveform[i] << ",";
        
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
    
    std::cout << width << ";" << resizedWaveform.size() << std::endl;
}

float Waveform::getMax()
{
    float max = 0;
    float current;
    
    for (int i = 0; i < resizedWaveform.size(); ++i) {
        if ((current = resizedWaveform[i]) > max)
            max = current;
    }
    
    return max;
}