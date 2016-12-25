//
//  main.cpp
//  dspsdr
//
//  Created by Albin Stigö on 21/12/16.
//  Copyright © 2016 Albin Stigo. All rights reserved.
//

#include "WProgram.h"
#include "i2c_t3.h"
#include "LiquidCrystal_I2C.h"

#include "vfo_knob.h"

#include "stdlib.h"

#include "command.h"
#include "radio.h"
#include "keyer.h"
#include "break_in.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// outputs
#define KEY_PIN 17
#define PTT_PIN 16

// inputs
#define L_PIN 3
#define R_PIN 2

VFOKnob vfo_knob(11, 12);
LiquidCrystal_I2C lcd(0x27, 16, 2);

Radio radio(PTT_PIN);
Keyer keyer(L_PIN, R_PIN, KEY_PIN);
BreakInTimer breakin(radio, keyer);

/* print I2C status
void print_i2c_status(void)
{
    switch(Wire.status())
    {
    case I2C_WAITING:  Serial.printf("I2C waiting, no errors\r\n"); break;
    case I2C_ADDR_NAK: Serial.printf("Slave addr not acknowledged\r\n"); break;
    case I2C_DATA_NAK: Serial.printf("Slave data not acknowledged\r\n"); break;
    case I2C_ARB_LOST: Serial.printf("Bus Error: Arbitration Lost\r\n"); break;
    case I2C_TIMEOUT:  Serial.printf("I2C timeout\r\n"); break;
    case I2C_BUF_OVF:  Serial.printf("I2C buffer overflow\r\n"); break;
    default:           Serial.printf("I2C busy\r\n"); break;
    }
}*/

void agn_cmd(const char* arg, int len) {
    if (len) {
        uint8_t gain = (uint8_t) atoi(arg);
        radio.setAFGain(gain);
    }
}

void fre_cmd(const char* arg, int len) {
    if (len) {
        uint32_t freq = (uint32_t) atoi(arg);
        radio.setFrequency((float) freq);
    }
}

void fil_cmd(const char* arg, int len) {
    if (len) {
        switch (arg[0]) {
            case 'n':
                radio.setFilter(Radio::FILTER_NARROW);
                break;
            case 'w':
                radio.setFilter(Radio::FILTER_WIDE);
                break;
            default: break;
        }
    }
}

void mod_cmd(const char* arg, int len) {
    if (len) {
        switch (arg[0]) {
            case 'a':
                radio.setMode(Radio::MODE_AM);
                break;
            case 'u':
                radio.setMode(Radio::MODE_USB);
                break;
            case 'l':
                radio.setMode(Radio::MODE_LSB);
                break;
            default: break;
        }
    }
}

void fup_cmd(const char* arg, int len) {
    radio.tuneFrequency(1);
}

void fdn_cmd(const char* arg, int len) {
    radio.tuneFrequency(-1);
}

command_t commands[] = {
    {"agn", agn_cmd},
    {"fre", fre_cmd},
    {"fil", fil_cmd},
    {"mod", mod_cmd},
    {"fup", fup_cmd},
    {"fdn", fdn_cmd},
};

void parse_command() {
    static char buf[80];
    while(Serial.available()) {
        int len = readline(Serial.read(), buf, sizeof(buf));
        if (len > 2) {
            for (uint8_t i = 0; i < ARRAY_SIZE(commands); i++) {
                if(strcmp3(buf, commands[i].cmd)) {
                    commands[i].fun(buf + 3, len - 3);
                    // 
                    break;
                }
            }
        }
    }
}

void setup() {
    // Pins
    //pinMode(KEY_PIN, OUTPUT); // 17
    //digitalWrite(KEY_PIN, LOW);
    
    pinMode(PTT_PIN, OUTPUT); // 16
    digitalWrite(PTT_PIN, HIGH);
    
    //pinMode(KEY1_IN_PIN, INPUT_PULLUP); // 2
    //pinMode(KEY2_IN_PIN, INPUT_PULLUP); // 3
}

void key_update() {
    //digitalWrite(KEY_PIN, digitalRead(KEY2_IN_PIN));
    // digitalWrite(PTT_PIN, digitalRead(KEY2_IN_PIN));
}


int main(void)
{
    setup();
    
    // Setup serial
    // USB is always 12 Mbit/sec
    Serial.begin(9600);
    
    radio.begin();
    radio.setFrequency(14000000.f);
    radio.setAFGain((uint8_t) 0x10);
    
    // LCD
    lcd.begin();
    
    // Main loop
    while(true) {
        
        // Tune
        if(vfo_knob.read()) {
            lcd.home();
            
            radio.tuneFrequency(vfo_knob.read());
            lcd.printf("%8d MHz", (int) (radio.frequency() / 10));
            // Serial.printf("%f\r\n", radio.frequency());
            // Serial.printf("%d\r\n", vfo_knob.read());
            vfo_knob.reset();
        }
        
        // S meter
        //uint8_t rssi = radio.status();
        //lcd.setCursor(0, 1);
        //lcd.printf("%3d", rssi);
        
        keyer.update();
        breakin.update();
        
        // Handle commands over USB
        parse_command();
    }
}
