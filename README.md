# PIR_alarm
Alarm system using PIR motion sensor.

Hardware components: 
-  HC-SR501 PIR motion sensor
-  LCD 1602
-  I2C communication module
-  RGB led (common cathode)
-  active 5v buzzer
-  Keypad 4x4

As the sensor detects motion, the rgb led turns red and a small interface will display on the lcd, asking for a password, also the buzzer will start emitting a high pitched sound. If the correct password is not provided in about 10 seconds, the watchdog timer will perform a system reset and a watchdog interrupt (WDT_vect) will clear the lcd before resetting.
