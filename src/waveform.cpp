//
//  waveform.cpp
//  audioplayer
//
//  Created by Vincent Timofti on 12/05/2014.
//
//

#include "waveform.h"

void Waveform::add(float value)
{
    waveform.push_back(value);
}

void Waveform::test()
{
    for (int i = 0; i < waveform.size(); ++i) {
        std::cout << waveform[i] << ",";
        
    }
    std::cout << std::endl;
}
