// #include <LiquidCrystal.h>

#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>

// Choose two Arduino pins to use for software serial to interface to GPS module
int RXPin = 2;
int TXPin = 3;

char A_Z[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ", a_z[27] = "abcdefghijklmnopqrstuvwxyz", grid[7];  // Array of characters for the Grid Square function
float lat, lon;  //  variable for latitude and longitude function
float LAT, LON;  // latitude and longitude values
bool ClearLCD = false; //To remember if the LCD has been cleared -NX9O 2022 10 13

//Default baud of NEO-6M is 9600
int GPSBaud = 9600;
TinyGPSPlus gps;

// enter the I2C address and the dimensions of your LCD here
#define LCD_ADDRESS 0x27
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);  // LCD is 16 char x 2 lines


// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {

  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
  
  //set up LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("GPS GRID SQUARE"); //Added the word SQUARE - NX9O 2022 10 13
  Serial.println("GPS Grid Display");
  delay(5000);

  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  RX    RX        Fail"));
  Serial.println(F("------------------------------------------------------------------------------------------------------------------"));


}

void loop() {

  displayInfo();
}

void displayInfo() {

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();

  // write to the LCD as well



if (gps.location.isValid() ) 
  {
  lat = gps.location.lat();
  lon = gps.location.lng();
  LAT = lat*1000000;  // grid sq routine expects fix in slightly diff format than tinygps++ library... (no decimal)
  LON = lon*1000000;

  GridSquare(LAT, LON);  // Calulate the Grid Square
  Serial.println(grid);

  // Blank the display if not already- NX9O 2022 10 13
  if (!ClearLCD)
  {
    lcd.clear();
    ClearLCD = true;
    //lcd.setCursor(0, 1);
    //lcd.print("Sat");
  }

  // Changed the order of LCD lines and removed the word "GRID" NX9O 2022 10 13
  //lcd.setCursor(0, 0);
  // lcd.print("GRID            ");
  lcd.setCursor(0, 0); 
  lcd.print(grid);
  lcd.setCursor(0, 1);
  lcd.print(gps.satellites.value());

  lcd.setCursor(7, 0);
  lcd.print(lat,6);  // decimals was ,3 - NX9O 2022 10 13
  if (lon < 100)  // Move lat over 1 character if 100 degrees or higher; later test for - and -100 - NX9O 2022 10 13
  {
    lcd.setCursor(5, 1);
    lcd.print(" ");
    lcd.setCursor(6, 1);
    lcd.print(lon,6);
  } else {
    lcd.setCursor(5, 1);
    lcd.print(lon,6);
  }

  }
  else
  {
  lcd.setCursor(0, 1);
  lcd.print("No Valid Fix    ");
  }

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

  Serial.println();
  Serial.println();
}


// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec) {
  if (!valid) {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  } else {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1);  // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3
                           : vi >= 10  ? 2
                                       : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len) {
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t) {
  if (!d.isValid()) {
    Serial.print(F("********** "));
  } else {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }

  if (!t.isValid()) {
    Serial.print(F("******** "));
  } else {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len) {
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartDelay(0);
}

// this is the function I found from another project... it used a diff GPS library so I had to change to fix format to accomodate...
// https://github.com/ke7uia/GridSquareDisplay

void GridSquare(float latitude,float longtitude)  // Calculate the Grid Square from the latitude and longitude
{
  // Maidenhead Grid Square Calculation
  float lat_0,lat_1, long_0, long_1, lat_2, long_2, lat_3, long_3,calc_long, calc_lat, calc_long_2, calc_lat_2, calc_long_3, calc_lat_3;  // Set up the function variables
  lat_0 = latitude/1000000;
  long_0 = longtitude/1000000;

  int grid_long_1, grid_lat_1, grid_long_2, grid_lat_2, grid_long_3, grid_lat_3;

  // Begin Calcs
  calc_long = (long_0 + 180);  // Calculate the first 2 characters of the Grid Square
  calc_lat = (lat_0 + 90);
  long_1 = calc_long/20;
  lat_1 = (lat_0 + 90)/10;

  grid_lat_1 = int(lat_1);
  grid_long_1 = int(long_1);
  
  calc_long_2 = (long_0+180) - (grid_long_1 * 20);  // Calculate the next 2 digits of the Grid Square
  long_2 = calc_long_2 / 2;
  lat_2 = (lat_0 + 90) - (grid_lat_1 * 10);
  grid_long_2 = int(long_2);
  grid_lat_2 = int(lat_2);
  
  calc_long_3 = calc_long_2 - (grid_long_2 * 2);  // Calculate the last 2 characters of the Grid Square
  long_3 = calc_long_3 / .083333;
  grid_long_3 = int(long_3);
  
  lat_3 = (lat_2 - int(lat_2)) / .0416665;
  grid_lat_3 = int(lat_3);

  // Here's the first 2 characters of Grid Square - place into array
  grid[0] = A_Z[grid_long_1]; 
  grid[1] = A_Z[grid_lat_1];
  
  // The second set of the grid square
  grid[2] = (grid_long_2 + 48);  // ascii 0 0x30
  grid[3] = (grid_lat_2 + 48);
  
  // The final 2 characters
  grid[4] = a_z[grid_long_3];
  grid[5] = a_z[grid_lat_3];
 
  return;
  
}
