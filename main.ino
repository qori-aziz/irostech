
/*
Sumber
https://www.playembedded.org/blog/an-object-counter-using-an-ir-sensor-and-arduino/ : IR sensor
https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/ :i2c lcd
https://create.arduino.cc/projecthub/SURYATEJA/use-a-buzzer-module-piezo-speaker-using-arduino-uno-89df45 : buzzer
*/

#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#define IR1                     9        /* digital pin input for ir sensor  */
#define IR2                     8       /* digital pin input for ir sensor  */
// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // for 16x2 LCD.
const int buzzer = 7; //buzzer to arduino pin 7

int IR1_out = HIGH; /* Avoiding initial false detections.    */
int IR2_out = HIGH; /* Avoiding initial false detections.    */
int counterpensil = 0;
int counterslat= 0;
int i = 0;


void setup() {
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  //initiate the IR sensor
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  //initiate the buzzer
  pinMode(buzzer, OUTPUT);
}


void loop() {
  //Pembacaan slat dan pensil yang melewati sensor IR
  IR1_out = digitalRead(IR1);
  IR2_out = digitalRead(IR2);
  if(IR1_out == LOW) {
    counterpensil++;
  }
  if(IR2_out == LOW) {
    counterslat++;
  } 
  delay(100);
  //Menampilkan jumlah pensil maupun selat:
  lcd.setCursor(0, 0); //Set kursor di kolom dan baris pertama.
  lcd.print("Pensil: ",counterpensil, DEC); // Menampilkan jumlah pensil yang melewati sensor
  lcd.setCursor(2, 1); //Set kursor di baris kedua dan kolom ketiga
  lcd.print("Slat: %i", counterslat, DEC); //Menampilkan jumlah slat yang melewati sensor
  
  //Mekanisme alert kepada pegawai saat pensil dan slat mencapai jumlah tertentu
  if (counterpensil == 5000){
	tone(buzzer, 1000); // Send 1KHz sound signal...
	delay(1000);        // ...for 1 sec
	noTone(buzzer);     // Stop sound...
	delay(1000);
  }
  if (counterslat == 2000){
	tone(buzzer, 1000); // Send 1KHz sound signal...
	delay(1000);        // ...for 1 sec
	noTone(buzzer);     // Stop sound...
	delay(1000);
  }
}