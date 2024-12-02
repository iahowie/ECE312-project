# Power measurements for the Railroad Crossing System

1. __LCD Display__
   
   - For the display characters:

      $V = 4.76 \ V$
      
      $I = 0.14  \ mA$
      
      $P_{1, display} = V * I = 4.76 * 0.14 = 0.666 \ mW$

   - For the backlight:

      $V = 5.01 \ V$
      
      $I = 21.2  \ mA$
      
      $P_{1, backlight} = V * I = 5.01 * 21.2 = 106.2 \ mW$

   - Total power dissipation:

      $P_{1} = P_{1, display} + P_{1, backlight} = 0.666 + 106.2 = 106.9 \ mW$
   
3. __Ultrasonic Sensors (x2)__

   - For a single sensor:
   
      $V = 5.01 \ V$
   
      $I = 2.22 \ mA$
   
      $P_{2, single} = V * I = 5.01*2.22 = 11.1 \ mW$

   - For two sensors:

      $P_{2, total} = 2*P_{2, single} = 2 * 11.1 = 22.2 \ mW$

4. __Speaker__

   $V = 0.258 \ V$

   $I = 34.4 \ mA$

   $P_3 = V * I = 0.258 * 34.4 = 8.88 \  mW$
   
5. __Servo Motor__

   $V = 4.69 \ V$

   $I = 0.217 \ A$

   $P_4 = V * I = 4.69*0.217 = 1.02 \ W$

6. __RGB LED Common Cathode__

   $V = 0.616 \ V$
   
   $I = 5.79 \ mA $
   
   $P_5 = V * I = 5.79 * 0.616 = 3.57 \ mW$

### Total Peak Power Dissipation:

$P_{total} = 106.9 + 22.2 + 8.88 + 1020 + 3.57 = __{1160 \ mW}__ = __{1.16 W}__$
