#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <SD.h>


#ifdef __AVR__
  #include <avr/power.h>
#endif

/*
 * setting for gps 
 */
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  true

/*
 * setting for rgb led 8bit
 */
 #define PIN 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

// set up variables using the SD utility library functions:
const int chipSelect = 6;

//set up for rgb led 8 bit
const int onoff = 0;
const int gps =1;
const int sdCard = 2;
const int saving = 3;

//blue color -> callibration
//red color -> something goes wrong
//green -> turn on / done something!

File dataFile;

/* 
 *  setting for button
 */
int mode =-1;
int sw = 8;
int sw_status = 0; 

//buzzer pin 
int toneNum = 7;

//cds sensor value
int sensor_value = 0;


// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit GPS library basic test!");
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);
  
  mySerial.println(PMTK_Q_RELEASE);
  pinMode(sw, INPUT_PULLUP);
  strip.begin();
  strip.show();
  set_green_pin(onoff);
  initialize_sd();
}

//일단 처음 켰을 때는 gps를 파란색으로 만들어 놓고 
//gps값이 fix가 되었다면 초록색으로 만들어주기 
//처음 lcd를 이용했을 때는 on/off 모드가 있었지만
//이제는 led를 이용하니까 그냥 gps값이 fix되었을 때 버튼을 눌러 그 순간의 밝기값과 위치값과 그리고 시간을 sd카드에 저장만 해주기 
void loop() {

  int gps_status = check_gps_ready();
  
  if(digitalRead(sw)==LOW) //스위치가 눌리면 ..... 
  {
    if(sw_status==0 && gps_status==1)
    {
//      Serial.println(".................................");
//      Serial.println(".................................");
//      Serial.print("sw_status : ");
//      Serial.println(sw_status);
//      Serial.println(".................................");
//      Serial.println(".................................");
      
     
        tone(toneNum,1800,50);
        read_gps_write_to_sd();
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

/*
 * read cds sensor value 
 */
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
  return mean_sensor_value;
}


/*
 * setting for sd card
 */

void initialize_sd()
{
  if (!SD.begin(6)) {
    Serial.println("initialization failed!");
    set_red_pin(2);
    return;
  }
  else 
  {
     Serial.println("initialization done.");
     set_green_pin(2);
  }
}

/*
 * set rgb led 8bit pin 
 */
void set_red_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(255,0,0);
  strip.setPixelColor(pinNum, c);
  strip.show();
}

void set_green_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(0,255,0);
  strip.setPixelColor(pinNum, c);
  strip.show();
}
void turn_off(uint16_t pinNum)
{
  uint32_t c = strip.Color(0,0,0);
  strip.setPixelColor(pinNum, c);
  strip.show();
}

void set_blue_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(0,0,255);
  strip.setPixelColor(pinNum, c);
  strip.show();
}

void set_yellow_pin(uint16_t pinNum)
{
  uint32_t c = strip.Color(255,255,0);
  strip.setPixelColor(pinNum, c);
  strip.show();
}

/*
 * set gps module 
 */
 
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) 
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();
void read_gps_write_to_sd()
{
  int cds_value = read_cds_value();
  String dataString = "";
  String timeString = "";
  String dateString ="";
  String latitude = "";
  String longitude = "";
  String date;
  String time;
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
//  if (millis() - timer > 2000) { 
//    timer = millis(); // reset the timer
    String hour = String(GPS.hour+9);
    // 현재 사용하는 라이브러리가 GMT표준시간을 출력합니다.
    // 우리나라는 GMT표준시간보다 9시간 빠르므로 9시간 더해주면 됩니다.
    // 추가로 출력시 24시가 넘어가면 24시 이상값이 나오므로 맞게 편집해 주셔야 합니다.
    String minu = String(GPS.minute);
    String sec = String(GPS.seconds);
    
    time = hour + ":" + minu + ":" + sec;

    // 시분초를 HH:MM:SS형태로 바꿔주고, String값을 char*로 바꿔주어 LCD에 출력할 수 있도록 합니다.
    
    String day = String(GPS.day);
    String month = String(GPS.month);
    String year = String(GPS.year);
    
    date = "20" + year + "/" + month + "/" + day;
   
    char *fDate = new char[date.length() + 1];
 
    delete [] fDate;
    // 년월일을 LCD에 출력해 줍니다.
    
    if (GPS.fix) 
    {
      latitude += GPS.latitude;
      latitude +=",";
      longitude += GPS.longitude;
      latitude +=",";
      dateString += date;
      dateString += ",";
      timeString += time;
      timeString += ",";

//       Serial.println("..........................");
//      Serial.println("..........................");
//      Serial.println(GPS.latitude);
//      Serial.println("..........................");
//      Serial.println("..........................");

      dataFile = SD.open("file.csv", FILE_WRITE);
      if (dataFile) 
      {
        dataFile.print(date);
        dataFile.print(",");
        dataFile.print(time);
        dataFile.print(",");
        dataFile.print(GPS.latitude);
        dataFile.print(",");
        dataFile.println(GPS.longitude);
        dataFile.close();
        set_green_pin(3);
        delay(500);
        turn_off(3);
      }
      else 
      {
        set_red_pin(3);
        delay(500);
        turn_off(3);
      }  
    }
//  }
}

int check_gps_ready()
{

  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
   
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer

    if (GPS.fix)  //gps is fixed 
    {
      set_green_pin(1);
      return 1;
    }
    else   //gps is not fixed 
    {
      set_blue_pin(1);
      return 0;
    }
  }
}


