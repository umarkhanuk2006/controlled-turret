Project Desc: Arduino nano reads the reads the acceleration readings from the MPU and translates that into movement of the two dual axis servo motors to keep it pointing in the same direction. Tilting in the X axis moves one servo motor and tilting in the Y axis moves the other servo motor. The program also outputs the mean and standard deviation data of the last 128 raw inputs.
Components: Arduino Nano, MPU-6050, two servos
Wiring: MPU SDA → A4, SCL → A5; servo signals → D9 and D10
