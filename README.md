= A simple, Arduino-based XFM2 front panel

== Introduction

I built a Futur3soundz synthesizer using instructions here: https://www.futur3soundz.com/xfm2
I got the parts from Mouser, total was about $100, which isn't at all bad given the capabilities
of the synthesizer when you're done: "64-voice, 2-part, 6-operator FM Synthesizer with built-in effects processor"

When complete, you can plug it in to a MIDI controller to both play notes and control the voices, or you can
connect to it over serial at 500,000 baud (8N1) and send XFM2-specific commands.

I decided to build an Arduino interface, as minimal as possible (but still user-friendly and useful), and to bundle
the XFM2 into the same box. This github repository describes that project.

== Components

 - XFM2 synthesizer
 - Arduino Nano
 - 1306-based OLED display, such as this one: https://www.adafruit.com/product/938
 - 3 rotary encoders - I used these: https://www.amazon.com/dp/B06XQTHDRR/
 - a breadboard or protoboard
 - wire, soldering iron, solder, etc.

== Wiring

The OLED has four connections, power, ground, SDA and SCL. SDA connects to A4, SCL connects to A5.
The rotary encoders have five connections, but my Arduino sketch doesn't use the buttons (yet! that's coming).
They all get connected to power and ground, and the CLK and DT connections for the three knobs are connected
to the Arduino: D2/D3 for the first knob D4/D5 for the second, and D6/D7 for the third. If the knob rotation goes
backwards (clockwise causes a number to decrease) you can either swap the pins in the sketch, or swap the wires on the Arduino.

== Programming

The sketch is in the repository. I had to do some trickery putting the Parameter labels into program memory, because they won't all fit in DRAM at once. The Adafruit splash screen stays on until you twist any of the knobs.

If you're using platformio, it should download the dependencies for you. If you're using Arduino, you'll need to install the Adafruit GFX Library and the Adafruit SSD1306 Library. I found the "Encoder" library didn't work reliably for me, so I adopted a "robust rotary encoder" library (and have notified the author - he hasn't replied yet).

== Operation

The left knob cycles among the presets. I believe the XFM2 has about 17 built-in presets and the remaining 110 are ours for the taking. The middle knob cycles among the parameters. The top line of the display shows the preset you're on, followed by the parameter index. The second line is the text description of the current parameter. The third knob changes the value of that parameter, and the third line of the display shows that value.

== Future work

There is a TODO of things I'd like to do, but at a high level, I'd like to enable more control of the XFM2 via the knobs.
Writing the current paramers to a preset slot should be easy to do, and changing the MIDI parameters as well.
