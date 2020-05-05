// XFM2 three-knob front panel by dylan mcnamee
// dylan.mcnamee@gmail.com // mecodegoodsomeday.org
// no rights reserved

// customized Robust Rotary encoder reading
// from https://www.best-microcontroller-projects.com/rotary-encoder.html
// Copyright John Main - best-microcontroller-projects.com
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "XFM2_params.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

typedef struct Encoder {
    int data_pin;
    int clk_pin;
    uint16_t store;
    uint8_t prevNextCode;
} Encoder;

//------------------------------------------------------------------------------------------------------------
// set_unit(), set_parameter()
//
//------------------------------------------------------------------------------------------------------------
void set_unit( int unit ) {
        // Default to unit 1
        Serial.write( unit == 2 ? '2' : '1' );
}

void drain_serial() {
    int r = 0;
    while (r >= 0) {
        r = Serial.read();
        delay(1);
    }
}

void set_preset (int value) {
        // somehow want to assert 1 <= value <= 127

        Serial.write('r');
        Serial.write(value);
        drain_serial();
        return;
}

void set_parameter( int paramIndex, int value ) {

    Serial.write('s'); // 's' = Set Parameter
    uint16_t param = pgm_read_word_near(XFM2_param_nums + paramIndex);
        if( param >= 255 ) {
                // Parameters above 255 have a two-byte format: b1 = 255, b2 = x-256
                Serial.write( 255 );
                Serial.write( param - 255   );
        }
        else {
                Serial.write( param );
        }
        Serial.write( value );
}

unsigned int get_parameter (int paramIndex) {
    uint16_t param = pgm_read_word_near(XFM2_param_nums + paramIndex);
    int ret = -1;
    int tries = 0;
    Serial.write('g');
    if( param >= 255 ) {
        // Parameters above 255 have a two-byte format: b1 = 255, b2 = x-255
        Serial.write(255);
        Serial.write(param - 255);
    } else {
        Serial.write(param);
    }
    while((tries++ < 20) && (Serial.available() == 0)) {
        delay(10);
    }
    ret = Serial.read();
    if (ret < 0) ret = 555; // XXX TMP debugging info
    return (unsigned int) ret;
}

int8_t read_rotary(Encoder *e);

Encoder *newEncoder(int clk, int data) {
    Encoder *ret = (Encoder *)calloc(1, sizeof(Encoder));
    ret->data_pin = data;
    ret->clk_pin = clk;
    ret->store = 0;
    ret->prevNextCode = 0;
    pinMode(clk, INPUT);
    pinMode(clk, INPUT_PULLUP);
    pinMode(data, INPUT);
    pinMode(data, INPUT_PULLUP);
    return ret;
}

Encoder *leftEncoder;
Encoder *rightEncoder;
Encoder *presetEncoder;

long stamp = 0;
int presetNum = 0;

void setup() {
    rightEncoder=newEncoder(PD3,PD2);
    leftEncoder=newEncoder(PD5, PD4);
    // i2c display is pins 4/5
    presetEncoder=newEncoder(PD7,PD6);
    Serial.begin (500000);
    // Serial.println(F("Start:"));

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(1000);
    display.clearDisplay();
    // Serial.println(F("running..."));
    stamp = millis();
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setTextSize(1);
}

char label_buf[32];

void loop() {
    static int paramIndex, paramValue;
    int val;
    bool anyChange = false;
    bool valChange = false;
    long newStamp = millis();
    long delta = newStamp - stamp;
    int step = 1;
    if (delta < 300) {
        step = 5;
    }

    // read whether user is changing the preset-program
    val=read_rotary(presetEncoder);
    if (val != 0) {
        presetNum += val;
        if (presetNum < 1) presetNum = 127;
        if (presetNum > 127) presetNum = 1;
        set_preset(presetNum);
        delay(10); // getting parameters wasn't working without this.
        // since we're on a new preset, get the value corresponding to this parameter
        paramValue = get_parameter(paramIndex);
        anyChange = true;
    }
    val=read_rotary(leftEncoder);
    if( val != 0 ) {
        paramIndex +=(val * step);
        if (paramIndex < 0) paramIndex = XFM2_PARAM_COUNT - 1;
        if (paramIndex >= XFM2_PARAM_COUNT) paramIndex = 0;
        // Serial.print(paramIndex);Serial.print(" ");
        anyChange = true;
        paramValue = get_parameter(paramIndex);
        delay(10);
    } else {
        val=read_rotary(rightEncoder);
        if ( val != 0 ) {
            paramValue += (val * step);
            if (paramValue < 0) paramValue = 255;
            if (paramValue > 255) paramValue = 0;
            // Serial.print(paramValue);Serial.print(" ");
            anyChange = true;
            valChange = true;
        }
    }
    if (!anyChange) return;
    // only update the timestamp & display if the knobs moved

    stamp = newStamp;

    // only set the value if the value (right knob) has changed
    if (valChange) {
        set_parameter(paramIndex, paramValue);
    }
    display.clearDisplay();
    display.setCursor(0,0);

    display.print(presetNum);
    display.print(F(": "));
    display.println(paramIndex);
    // fetch the pointer to our label from program memory
    char *label_ptr = pgm_read_word_near(XFM2_param_labels + paramIndex);
    // fetch the label itself from program memory, for displaying
    display.setTextSize(2);
    strcpy_P(label_buf, label_ptr); // "(char *) XFM2_param_labels+paramIndex);
    display.println(label_buf);
    display.setTextSize(1);
    display.print(paramValue);
    // display.print(F("delta: "));
    // display.println(diff);
    display.display();
}

// A vald CW or CCW move returns 1, invalid returns 0.
int8_t read_rotary(Encoder *e) {
    static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

    e->prevNextCode <<= 2;
    if (digitalRead(e->data_pin)) e->prevNextCode |= 0x02;
    if (digitalRead(e->clk_pin)) e->prevNextCode |= 0x01;
    e->prevNextCode &= 0x0f;

    // If valid then store as 16 bit data.
    if (rot_enc_table[e->prevNextCode] ) {
        e->store <<= 4;
        e->store |= e->prevNextCode;
        //if (store==0xd42b) return 1;
        //if (store==0xe817) return -1;
        if ((e->store&0xff)==0x2b) return 1;
        if ((e->store&0xff)==0x17) return -1;
    }
    return 0;
}
