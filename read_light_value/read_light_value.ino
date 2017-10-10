
#include <LiquidCrystal.h>
//button 
int mode =-1;
int sw = 8;
int sw_status = 0; 

//buzzer pin 
int toneNum = 7;

//cds sensor value
int sensor_value = 0;

//lcd display
const int rs = 12, en = 11, d4 = 5, d5 = 6, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sw, INPUT_PULLUP);
  lcd.begin(16, 2);
}
//다시 처음으로 돌아가는 모드 추가 여기에 나중에 부저를 추가하면 더 좋다 
void loop() {
  
  if(digitalRead(sw)==LOW) //스위치가 눌리면 ..... 
  {
    if(sw_status==0){
      mode *=-1;
      //Serial.println(mode);
      if(mode==1) {
        tone(toneNum,1800,50);
        display_lcd(mode);
        read_cds_value();
      }
      else if(mode==-1)
      {
        tone(toneNum,2000,50);
        display_lcd(mode);
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
void read_cds_value()
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
  lcd.print("brightness: ");
  lcd.print(mean_sensor_value);
}
void display_lcd(int mode)
{
  if(mode==1){
    lcd.clear();
    lcd.print("welcome back!!!");
    delay(1000);
    lcd.clear();
  }
  else if(mode==-1)
  {
    lcd.clear();
    lcd.print("Good Bye~~~");
    delay(1000);
    lcd.clear();
  }
}
long readVcc() {
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


