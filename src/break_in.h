//
//  break_in.hpp
//  dspsdr
//
//  Created by Albin Stigö on 25/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#ifndef break_in_h
#define break_in_h

#include "WProgram.h"

#include "radio.h"
#include "keyer.h"

class BreakInTimer {
    Radio& radio_;
    Keyer& keyer_;
    uint16_t timeout_ms_;
    elapsedMillis since_keyed_;
    
public:
    BreakInTimer(Radio &radio, Keyer &keyer, uint16_t timeout_ms = 1000) :
    radio_(radio),
    keyer_(keyer),
    timeout_ms_(timeout_ms) {
        since_keyed_ = timeout_ms;
        
    }
    
    inline void update() {
        // reset timer
        if (keyer_.keyed()) since_keyed_ = 0;
        
        if ( (since_keyed_ < timeout_ms_) && (!radio_.tx()) ) {
            radio_.setTx(true);
        } else if ( (since_keyed_ > timeout_ms_) && radio_.tx() ) {
            radio_.setTx(false);
        }
    }
};


#endif /* break_in_h */
