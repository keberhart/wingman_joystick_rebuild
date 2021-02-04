# wingman_joystick_rebuild
This project supports the rebuild of my logitech wingman extreme joystick.
     After 20+ years the potentiometers were worn out and needed replacement.
     I elected to use Hall Effect sensors and magnets for the axes instead of
     potentiometers and added a toggle switch as well.

     This project uses the Joystick library from; 
          https://github.com/MHeironimus/ArduinoJoystickLibrary

     I am manually scanning the button matrix using the example at,
        https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
     pretty much verbatim. As stated on that site there are optimizations possible
     but it is working pretty well now.

     As you can see from the code the rudder has been comented out. The magnet to
     sensor orientation needs to be pretty exact and I was unble to
     compensate for the slop in the stick rotation. I also found that the X axis
     still has travel beyond the maximum sensor reading. In the future magnet
     placement needs to be more precise.

     3FEB21 - Kyle Eberhart - First commit. Barred from work due to positive 
        COVID-19 test, though my quarantine has been complete since 26JAN21.
