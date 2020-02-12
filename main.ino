
/*
Sumber
https://www.playembedded.org/blog/an-object-counter-using-an-ir-sensor-and-arduino/ : IR sensor
https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/ :i2c lcd
https://create.arduino.cc/projecthub/SURYATEJA/use-a-buzzer-module-piezo-speaker-using-arduino-uno-89df45 : buzzer
https://medium.com/@cgrant/using-the-esp8266-wifi-module-with-arduino-uno-publishing-to-thingspeak-99fc77122e82 : thinkspeak
https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/ : Ultrasonic
*/

#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <SoftwareSerial.h> //Library to allow serial with any pin
#define IR1                     9        /* digital pin input for ir sensor  */
#define IR2                     8       /* digital pin input for ir sensor  */
#define trigPin  11
#define echoPin  10

//Pin esp8266
#define RX 4
#define TX 3
String AP = "wifiku";       // koneksi ke wifi
String PASS = "passku"; // password wifi

//Pengaturan bagian thinkspeak
String API = "RMRYXNAWG5XHX2SL";  //API key yang didapat dari thinkspeak
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "pensil";
string field2 = "slat"
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // for 16x2 LCD.
const int buzzer = 7; //buzzer to arduino pin 7

int IR1_out = HIGH; // Agar deteksi awal aman
int IR2_out = HIGH; //Idem
int counterpensil = 0;
int counterslat= 0;
int i = 0;
//Pengaturan sensor ultrasonik
long duration;
int distance;

void setup() {
  // Inisiasi LCD
  lcd.init();
  lcd.backlight();
  //Inisiasi IR sensor
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  //Inisiasi buzzer
  pinMode(buzzer, OUTPUT);
  
  //Inisialisasi ultrasonik
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //Inisiasi esp8266
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}


void loop() {
  //Pembacaan slat dan pensil yang melewati sensor IR
  countmany(IR1, counterpensil);
  countmany(IR2, counterslat);
  
  //Menampilkan jumlah pensil maupun selat:
  lcd.setCursor(0, 0); //Set kursor di kolom dan baris pertama.
  lcd.print("Pensil: ",counterpensil, DEC); // Menampilkan jumlah pensil yang melewati sensor
  lcd.setCursor(2, 1); //Set kursor di baris kedua dan kolom ketiga
  lcd.print("Slat: ", counterslat, DEC); //Menampilkan jumlah slat yang melewati sensor
  
    //Mekanisme alert kepada pegawai saat pensil dan slat mencapai kelipatan jumlah tertentu
  buzzer (counterpensil);
  buzzer (counterslat);
  
  //Mekanisme pengiriman ke thinkspeak
  String getData = "GET /update?api_key="+ API +"&"+ field1 +"="+String(counterpensil);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);delay(1500);countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
  
  String getData = "GET /update?api_key="+ API +"&"+ field2 +"="+String(counterslat);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);delay(1500);countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
}

//Fungsi buzzer
void buzzer (int jumlah, int target){
	if (jumlah  % target == 0){
		tone(buzzer, 1000); // Send 1KHz sound signal...
		delay(1000);        // ...for 1 sec
		noTone(buzzer);     // Stop sound...
		delay(1000);
	}
}

//Fungsi penghitungan pensil dan slat
void countmany (int pin, int counter){
	pinout=digitalRead(pin);
	if(piout == LOW){
		counter ++;
	}
	delay(100);
}

//fungsi pengecekan oleh ultrasonik
void ultrasonik(int inpin, int outpin, long durasi, long jarak){
	// Clears the trigPin
	digitalWrite(inpin, LOW);
	delayMicroseconds(2);
	// Sets the trigPin on HIGH state for 10 micro seconds
	digitalWrite(inpin, HIGH);
	delayMicroseconds(10);
	digitalWrite(inpin, LOW);
	// Reads the echoPin, returns the sound wave travel time in microseconds
	durasi	= pulseIn(outpin, HIGH);
	// Calculating the distance
	jarak= durasi*0.034/2;
	// Prints the distance on the Serial Monitor
	Serial.print("jarak: ");
	Serial.println(jarak);
}
//Fungsi pengiriman data ke thinkspeak
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
