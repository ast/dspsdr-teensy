//
//  radio.hpp
//  dspsdr
//
//  Created by Albin Stigö on 21/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#ifndef radio_h
#define radio_h

#include "WProgram.h"
#include <stdint.h>

class Radio {
public:
    enum Mode { MODE_AM, MODE_USB, MODE_LSB };
    enum Filter { FILTER_NARROW, FILTER_WIDE };
    enum IF { IF_45, IF214 };
    
    Radio(uint8_t ptt_pin, uint8_t i2c_addr = 0x23) : ptt_pin_(ptt_pin), i2c_addr_(i2c_addr) {
        pinMode(ptt_pin_, OUTPUT);
        digitalWrite(ptt_pin_, HIGH);
    };
    
    void begin();

    bool tx();
    void setTx(bool tx);
    
    float frequency();
    void tuneFrequency(int32_t dfreq);
    void setFrequency(float f);
    void setFrequency(uint32_t nfreq);
    
    float clarifier();
    void tuneClarifier(uint32_t dfreq);
    void setClarifier(float f);
    void setClarifier(int8_t f);
    
    uint8_t afgain() { return afgain_; };
    void setAFGain(float gain);
    void setAFGain(uint8_t gain);
    
    uint8_t status();
    
    void setFilter(Filter filter);
    Filter filter();
    void setMode(Mode mode);
    Mode mode();

private:
    const uint8_t ptt_pin_, i2c_addr_;

    void i2c_write(uint8_t* bytes);
    
    void write_mode();
    void write_frequency();
    
    float clarifier_;
    int8_t nclarifier_;
    float frequency_;
    uint32_t nfrequency_;
    uint8_t afgain_;
    
    Mode mode_ = MODE_AM;
    Filter filter_ = FILTER_NARROW;
    
    bool forced_tx_ = false;
    bool forced_cw_ = false;
    bool cw_tx_nomod_ = true;
    uint8_t tx_att_ = 0;
    uint8_t rx_att_ = 0;
};

#endif /* radio_h */
