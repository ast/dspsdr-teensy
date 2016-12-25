//
//  radio.cpp
//  dspsdr
//
//  Created by Albin Stigö on 21/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#include "radio.h"

#include "i2c_t3.h"


#define CMD_MODE ((uint8_t) 0x01 << 6)
#define CMD_GAIN ((uint8_t) 0x02 << 6)
#define CMD_FREQ ((uint8_t) 0x03 << 6)

#define SSB      ((uint8_t) 0x01 << 5)  // OR AM
#define WIDE     ((uint8_t) 0x01 << 4)  // OR NARROW
#define USB      ((uint8_t) 0x01 << 3)  // LSB
#define TX       ((uint8_t) 0x01 << 2)  // FORCED TX
#define KEY      ((uint8_t) 0x01 << 1)  // FORCED CW KEY

#define CW_TX_NOMOD ((uint8_t) 0x01 << 5)

#define TX_ATT(n)   ((uint8_t) (n & 0x03) << 6) // 1: 6 dB TX attenuation, 0: Full TX power
#define RX_ATT(n)   ((uint8_t) (n & 0x03) << 3) // 1: 6 dB TX attenuation, 0: Full TX power
#define CLAR(n)  ((uint8_t) n < 1)
#define GAIN(n)  ((uint8_t) n & 0x3f)

#define MAX_FREQ ((float) 120e6)
#define FREQ_BITS 25

// helper functions
static float freq_to_float(uint32_t nfreq) {
    return (float) (MAX_FREQ * nfreq / (1 << FREQ_BITS));
}

static uint32_t float_to_freq(float freq) {
    return (uint32_t) roundf( (freq / MAX_FREQ) * (1 << FREQ_BITS));
}

void Radio::i2c_write(uint8_t* bytes) {
    Wire.beginTransmission(i2c_addr_);
    Wire.send(bytes, 5);
    Wire.endTransmission();
}

float Radio::frequency() { return frequency_; }

void Radio::tuneFrequency(int32_t dfreq) {
    // dfreq can be +/-.
    uint32_t nfreq = (uint32_t) (nfrequency_ + dfreq);
    setFrequency(nfreq);
}

void Radio::setFrequency(uint32_t nfreq) {
    frequency_ = freq_to_float(nfreq);
    nfrequency_ = nfreq;
    write_frequency();
}

void Radio::setFrequency(float f) {
    nfrequency_ = float_to_freq(f);
    frequency_ = freq_to_float(nfrequency_);
    write_frequency();
}

void Radio::write_frequency() {
    uint8_t bytes[5] = { 0 };
    
    bytes[0] = CMD_FREQ;

    // TODO. potential problem here with twos complement?
    //bytes[1] |= CLAR(clarifier_);
    
    // Freq is 25 bits
    bytes[1] |= ((uint8_t) (nfrequency_ >> 24) & 0x01);
    bytes[2] |= ((uint8_t) (nfrequency_ >> 16) & 0xff);
    bytes[3] |= ((uint8_t) (nfrequency_ >> 8) & 0xff);;
    bytes[4] |= ((uint8_t) (nfrequency_ & 0xff));
                
    i2c_write(bytes);
}

void Radio::write_mode() {
    uint8_t bytes[5] = { 0 };
    
    bytes[0] |= CMD_MODE;
    
    // Mode.
    switch (mode_) {
        case MODE_AM:
            // this is default.
            break;
        case MODE_USB:
            bytes[0] |= SSB;
            bytes[0] |= USB;
            break;
        case MODE_LSB:
            bytes[0] |= SSB;
            break;
        default: break;
    }
    
    // Filter.
    switch (filter_) {
        case FILTER_NARROW:
            // this is default.
            break;
        case FILTER_WIDE:
            bytes[0] |= WIDE;
            break;
        default: break;
    }
    
    if (forced_tx_) {
        bytes[0] |= TX;
    }
    
    if (forced_cw_) {
        bytes[0] |= KEY;
    }
    
    bytes[1] |= TX_ATT(tx_att_);
    bytes[1] |= RX_ATT(rx_att_);

    // TODO: add option for this
    // IF_FREQ = 001
    bytes[2] |= ((uint8_t) 0x01 << 7);
    
    if (cw_tx_nomod_) {
        // CW no mod
        bytes[2] |= CW_TX_NOMOD;
    }
    
    i2c_write(bytes);
}

Radio::Filter Radio::filter() {
    return filter_;
}

void Radio::setFilter(Filter filter) {
    filter_ = filter;
    write_mode();
}

Radio::Mode Radio::mode() {
    return mode_;
}

void Radio::setMode(Mode mode) {
    mode_ = mode;
    write_mode();
}

// Read status byte from FPGA.
uint8_t Radio::status() {
    Wire.requestFrom(i2c_addr_, (size_t) 1);
    delay(10);
    if(Wire.getError()) {
        return 0;
    } else {
        return Wire.readByte();
    }
}

bool Radio::tx() {
    return !digitalRead(ptt_pin_);
}

void Radio::setTx(bool tx) {
    digitalWrite(ptt_pin_, !tx);
}

float Radio::clarifier() { return clarifier_; }
void Radio::setClarifier(float f) {}
void Radio::setClarifier(int8_t f) {}

void Radio::setAFGain(uint8_t gain) {
    uint8_t bytes[5] = { 0 }; // all elements to 0
    
    bytes[0] = CMD_GAIN;
    bytes[4] = GAIN(gain); // 0 - 63
    
    i2c_write(bytes);
}

void Radio::begin() {
    // It worked much better with external 2.2k pullups than the internal 47k.
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100);
    // 100ms timeout. We should probably never timeout
    Wire.setDefaultTimeout(100000);
    
    write_mode();
}
