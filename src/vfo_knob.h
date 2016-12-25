//
//  vfo_knob.hpp
//  dspsdr
//
//  Created by Albin Stigö on 25/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#ifndef vfo_knob_h
#define vfo_knob_h

#define ENCODER_OPTIMIZE_INTERRUPTS
#include "Encoder.h"

class VFOKnob : Encoder  {
public:
    VFOKnob(uint8_t pin1, uint8_t pin2):Encoder(pin1, pin2) { };
    
    int32_t read() {
        // Slow down tuning a bit. The encoder has 600 pulses/rev and that's to much.
        return (int32_t) Encoder::read() >> 3;
    };
    
    void reset() {
        Encoder::write(0);
    }
};

#endif /* vfo_knob_hpp */
