//  Iambic Morse Code Keyer Sketch
//  Copyright (c) 2009 Steven T. Elliott
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details:
//
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
//  Boston, MA  02111-1307  USA
//
//  keyer.cpp
//  dspsdr
//
//  Modified by Albin Stigö on 25/12/16.
//

#include "keyer.h"

void Keyer::update() {
    switch (keyer_state_) {
        case IDLE:
            // Wait for direct or latched paddle press
            if ((digitalRead(l_pin_) == LOW) ||
                (digitalRead(r_pin_) == LOW) ||
                (keyer_control_ & 0x03)) {
                update_paddle_latch();
                keyer_state_ = CHK_DIT;
            }
            break;
            
        case CHK_DIT:
            // See if the dit paddle was pressed
            if (keyer_control_ & DIT_L) {
                keyer_control_ |= DIT_PROC;
                keyer_timer_ = dit_time_;
                keyer_state_ = KEYED_PREP;
            }
            else {
                keyer_state_ = CHK_DAH;
            }
            break;
            
        case CHK_DAH:
            // See if dah paddle was pressed
            if (keyer_control_ & DAH_L) {
                keyer_timer_ = dit_time_ * 3;
                keyer_state_ = KEYED_PREP;
            }
            else {
                keyer_state_ = IDLE;
            }
            break;
            
        case KEYED_PREP:
            // Assert key down, start timing, state shared for dit or dah
            digitalWrite(key_pin_, LOW); // key down
            keyer_timer_ += millis(); // set ktimer to interval end time
            keyer_control_ &= ~(DIT_L + DAH_L);   // clear both paddle latch bits
            keyer_state_ = KEYED;                 // next state
            break;
            
        case KEYED:
            // Wait for timer to expire
            if (millis() > keyer_timer_) {            // are we at end of key down ?
                digitalWrite(key_pin_, HIGH); // key up
                keyer_timer_ = millis() + dit_time_;    // inter-element time
                keyer_state_ = INTER_ELEMENT;     // next state
            }
            else if (keyer_control_ & IAMBICB) {
                update_paddle_latch();           // early paddle latch in Iambic B mode
            }
            break;
            
        case INTER_ELEMENT:
            // Insert time between dits/dahs
            update_paddle_latch();               // latch paddle state
            if (millis() > keyer_timer_) {            // are we at end of inter-space ?
                if (keyer_control_ & DIT_PROC) {             // was it a dit or dah ?
                    keyer_control_ &= ~(DIT_L + DIT_PROC);   // clear two bits
                    keyer_state_ = CHK_DAH;                  // dit done, check for dah
                }
                else {
                    keyer_control_ &= ~(DAH_L);              // clear dah latch
                    keyer_state_ = IDLE;                     // go idle
                }
            }
            break;
    }
}

void Keyer::update_paddle_latch() {
    if (digitalRead(l_pin_) == LOW) {
        keyer_control_ |= DIT_L;
    }
    if (digitalRead(r_pin_) == LOW) {
        keyer_control_ |= DAH_L;
    }
}
