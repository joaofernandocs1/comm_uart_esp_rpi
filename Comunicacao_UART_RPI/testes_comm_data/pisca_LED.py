import gpiozero  # The GPIO library for Raspberry Pi
import time  # Enables Python to manage timing
 
led = gpiozero.LED(23) # Reference GPIO23
 
while True:
  led.on() # Turn the LED on
  time.sleep(5)
  led.off() # Turn the LED off
  time.sleep(5)  # Pause for 1 second