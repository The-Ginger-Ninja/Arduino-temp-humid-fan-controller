#include "DHT.h"           // include dht sensor library
 
#define DHTPIN 2           // temp/hum sensor pin
#define DHTTYPE DHT22      // type of DHTxx sensor = DHT 22  (AM2302)
#define fan 4              // fan on-off pin (relay)
#define fanPWM 3           // fanPWM speed control signal pin
#define heater 5           // heater pin

int setHum = 55;       // humidity setpoint % 0-100
int setTemp = 15;      // temperature setpoint degrees Celsius

int minHum = 50;       // humidity MIN % 0-100
int maxHum = 70;       // humidity MAX % 0-100
int minTemp = 10;      // temperature MIN degrees Celsius
int maxTemp = 30;      // temperature MAX degrees Celsius

DHT dht(DHTPIN, DHTTYPE);      // Initialize DHT sensor for normal 16mhz Arduino 
 
void setup() {
  Serial.begin(9600);           // enable serial monitor
  pinMode(fan, OUTPUT);         // enable fan output
  pinMode(fanPWM, OUTPUT);      // enable fanPWM signal output
  pinMode(heater, OUTPUT);      // enable heater output
  
  dht.begin();                  // begin dht sensor library
  
////////////////////Fan Speed 25KHz Clock Speed Setup////////////////////
  
   TCCR2A = 0x23;  // set time clock multiplier
   TCCR2B = 0x09;  // select clock
   OCR2A = 79;     // aiming for 25kHz
   OCR2B = 30;     // set the PWM duty cycle
}
 
void loop() { 
 
  ////////////////////Temp/Hum Sensor Readings////////////////////

   delay(2000);        // Wait a few seconds between measurements.

   float h = dht.readHumidity();           // read humidity
   float t = dht.readTemperature();        // read temperature as Celsius
   float f = dht.readTemperature(true);    // read temperature as Fahrenheit  

    if (isnan(h) || isnan(t)) {             // Check if any reads failed and exit early (to try again)
    Serial.println("Failed to read from DHT sensor!");
    return;
    }
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  
    float hi = dht.computeHeatIndex(f, h);  // compute heat index (temp must be sent in fahrenheit)
    
    Serial.print("Fan Speed: ");        // print fanSpeed readings to serial
    Serial.print(OCR2B);
    Serial.print("\t Humidity: ");      // print temp/hum readings to serial
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("\t Heat index: ");
    Serial.print(hi);
    Serial.println(" *F");
    
  
  
     if(t < minTemp) {    // if temperature falls below minTemp or minHum - turn fans OFF
        digitalWrite(fan, LOW);
        analogWrite(fanPWM, LOW);
        digitalWrite(heater, HIGH);
        OCR2B = 0;                      // speed of fan to 0   
     }
     else{                              // keep minimum airflow
        digitalWrite(fan, HIGH);
        analogWrite(fanPWM, HIGH);
        digitalWrite(heater, LOW);

  ////////////////////Fan Logic////////////////////
   
     if(h >= minHum || t >= minTemp)   // speed level 1
       OCR2B = 20;                     // fan = minimum
     if(t >= setTemp)             // speed level 2
        OCR2B = 30;               // fan + 10
     if(t >= setTemp + 5)         // speed level 3
        OCR2B = 40;               // fan + 20
     if(t >= setTemp + 10)        // speed level 4
        OCR2B = 50;               // fan + 30
     if(t >= setTemp + 15)        // speed level 5
        OCR2B = 60;               // fan + 40
     if(h >= maxHum || t >= maxTemp)    // speed level maximum
        OCR2B = 79;                     // fan = maximum
        }
}
