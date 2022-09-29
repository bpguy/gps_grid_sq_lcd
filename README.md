# gps_grid_sq_lcd

uses I2C LCD like this one
https://www.amazon.com/GeeekPi-Character-Backlight-Raspberry-Electrical/dp/B07S7PJYM6

and GPS module like this one
https://www.amazon.com/gp/product/B01D1D0F5M/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1
to display the curernt maidenhead grid sqaure on 2x16 LCD.

power required is USB to arduino or external 12v supply iffarduino hasd onboard voltage regulator (like UNO)
out out on LCD is
 ##.### ##.### for degrees latitude asnd degrees longitude to 3 dec places on line 1
 GRID XX##xx  # for 6 character gris square and nukber of sats seen by GPS module
 
 LCD will show "No Valid Fix"  until GPS module has valid fix data.
 
 sketch puts serial data at 9600 bps to serial montiro for more info
 *** in data fields show no GPS fix
 
GPS Grid Display
Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Chars Sentences Checksum
          (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  RX    RX        Fail


 
------------------------------------------------------------------------------------------------------------------
**** ***** ********** *********** **** ********** ******** **** ****** ****** ***** ***   63    0         0        
6    3.9   32.990337  -84.217842  50   09/29/2022 16:01:00 179  241.90 350.06 0.87  N     729   2         0        
EM72vx


