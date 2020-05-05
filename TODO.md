
# TODO

    Have UI gestures for more advanced functions:
    
    - [] writing presets
    
       - press left button once to enter "choose channel" mode
       - press again to write the current preset to that channel (&
         return to select mode)
         
    - [] $ - reset to factory settings (or erase all?)
    
       - erases all of the tunings & presets, so let's not.
       
    - [] MIDI layer commands
    
       - 42 10 x (omni) or 1-16 - #1 midi channel
       - 42 11 x (omni) or 1-16 - #2 midi channel
       - 42 12 0 or 1 (LAYER mode off/on: 0/1)
       - 49 (ascii '1') - subsequent commands control Unit 0
       -50 (ascii '2') - subsequent commands control Unit 1

       - press middle button once - send "42" commands to #1 -
         right knob chooses 0-16 & right knob sends command
       - press middle button again - send "42" commands to #2
       - press it again, send "49" - right button sends
       - press it again, send "50" - right button sends

