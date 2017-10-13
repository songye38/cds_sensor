#include <Wire.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>


// set up variables using the SD utility library functions:
File dataFile;
const int chipSelect = 6;

//set up for zs-042 module
#define DS3231_I2C_ADDRESS 0x68

//button 
int mode =-1;
int sw = 8;
int sw_status = 0; 

//buzzer pin 
int toneNum = 7;

//cds sensor value
int sensor_value = 0;

//lcd display
const int rs = 10, en = 9, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  pinMode(sw, INPUT_PULLUP);
  lcd.begin(16, 2);
  initialize_sd();
  initialize_time();
}

//다시 처음으로 돌아가는 모드 추가 여기에 나중에 부저를 추가하면 더 좋다 
void loop() {
  batterylevel(15,0);
  
  if(digitalRead(sw)==LOW) //스위치가 눌리면 ..... 
  {
    if(sw_status==0){
      mode *=-1;
      //Serial.println(mode);
      if(mode==1) {
        tone(toneNum,1800,50);
        display_onoff(mode);
        int cds_value = read_cds_value();
        write_to_sd(cds_value);
        displayTime();
      }
      else if(mode==-1)
      {
        tone(toneNum,2000,50);
        display_onoff(mode);
      }
    }
    sw_status = 1;
    delay(20);
  }
  else  //스위치가 안눌렸을 때 
  {
    sw_status = 0;
    delay(20);
  }
}
int read_cds_value()
{
  int sensor_value_array[10];
  int mean_sensor_value;
  for(int i=0; i<10; i++)
  {
    sensor_value = analogRead(A0);
    sensor_value_array[i] = sensor_value;
    mean_sensor_value += sensor_value_array[i];
  }
  mean_sensor_value /=10;
  lcd.setCursor(0, 1);
  lcd.print("brightness: ");
  lcd.print(mean_sensor_value);
  return mean_sensor_value;
}
void display_onoff(int mode)
{
  if(mode==1){
    lcd.setCursor(0,1);
    lcd.print("");
    lcd.setCursor(0, 1);
    lcd.print("welcome back!!!");
    delay(1000);
    lcd.clear();
  }
  else if(mode==-1)
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Good Bye~~~");
    delay(1000);
    lcd.clear();
  }
}
//9v max
void batterylevel(int xpos,int ypos)
{
  //read the voltage and convert it to volt
  double curvolt = double( readVcc() ) / 1000;
  // check if voltge is bigger than 4.2 volt so this is a power source
  if(curvolt > 8.8)
  {
    byte batlevel[8] = {  //9v를 기준으로 다 차 있을 때
    B01110,
    B11111,
    B10101,
    B10001,
    B11011,
    B11011,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 8.8 && curvolt > 8.0)
  {
    byte batlevel[8] = {
    B01110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 8.0 && curvolt > 7.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 7.0 && curvolt > 6.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 6.0 && curvolt > 5.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 5.0 && curvolt > 4.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 4.0 && curvolt > 3.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 3.0 && curvolt > 2.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }
  if(curvolt <= 2.0 && curvolt > 1.0)
  {
    byte batlevel[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    };
    lcd.createChar(0 , batlevel);
    lcd.setCursor(xpos,ypos);
    lcd.write(byte(0));
  }  
}
long readVcc() 
{
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}
void write_to_sd(int value)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.print("cds value : ");
    dataFile.println(value);
    dataFile.close();
    lcd.print("o");
    Serial.println(value);
  }
  else {
    lcd.print("x");
  }
}
void initialize_sd()
{
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}


byte decToBcd(byte val)
{
return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

void initialize_time()
{
  Wire.begin();
  //현재 시간을 정하는 부분
  Serial.begin(9600);
  setDS3231time(00,55,9,11,10,17); 
  Serial.println("time set complete");
}
//시계의 날짜를 정하는 함수
void setDS3231time(byte second, byte minute, byte hour, byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  //Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second,
  byte *minute,
  byte *hour,
  byte *dayOfMonth,
  byte *month,
  byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime()
{
byte second, minute, hour, dayOfMonth, month, year;
readDS3231time(&second, &minute, &hour, &dayOfMonth, &month,
&year);
// send it to the serial monitor
Serial.print(hour, DEC);
// convert the byte variable to a decimal number when displayed
Serial.print(":");
if (minute<10)
{
Serial.print("0");
}
Serial.print(minute, DEC);
Serial.print(":");
if (second<10)
{
Serial.print("0");
}
Serial.print(second, DEC);
Serial.print(" ");

Serial.print(year , DEC);
Serial.print("/");
Serial.print(month, DEC);
Serial.print("/");
Serial.print(dayOfMonth, DEC);
Serial.print(" ");

}




