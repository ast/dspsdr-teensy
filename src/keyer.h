//
//  keyer.hpp
//  dspsdr
//
//  Created by Albin Stigö on 25/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#ifndef keyer_h
#define keyer_h

#include "WProgram.h"
#include <stdint.h>

#define     DIT_L      0x01     // Dit latch
#define     DAH_L      0x02     // Dah latch
#define     DIT_PROC   0x04     // Dit is being processed
#define     PDLSWAP    0x08     // 0 for normal, 1 for swap
#define     IAMBICB    0x10     // 0 for Iambic A, 1 for Iambic B

class Keyer {
    enum KeyerState {IDLE, CHK_DIT, CHK_DAH, KEYED_PREP, KEYED, INTER_ELEMENT };
    KeyerState keyer_state_ = IDLE;
    
    const uint8_t l_pin_, r_pin_, key_pin_;
    uint8_t dit_time_;
    uint8_t keyer_control_ = IAMBICB;
    uint32_t keyer_timer_;
    
    void update_paddle_latch();
    
public:
    void update();
    inline bool keyed() { return keyer_state_ == KEYED; };
    inline uint8_t wpm() { return 1200 / dit_time_; };
    inline void setWPM(uint8_t wpm) { dit_time_ = 1200 / wpm; };
    
    Keyer(uint8_t l_pin, uint8_t r_pin, uint8_t key_pin, uint8_t wpm = 20) :
    l_pin_(l_pin),
    r_pin_(r_pin),
    key_pin_(key_pin)
    {
        setWPM(wpm);
        pinMode(key_pin_, OUTPUT); // 17
        digitalWrite(key_pin_, HIGH);
        
        pinMode(l_pin_, INPUT_PULLUP);
        pinMode(r_pin_, INPUT_PULLUP);
    }
};

#endif /* keyer_h */
