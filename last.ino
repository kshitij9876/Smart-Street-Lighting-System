//Here we begin with the code
#include <ESP8266WiFi.h>;  
#include<WiFiClient.h>;
#include<BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial // Enables Serial Monitor
#include <ThingSpeak.h>;  //Importing thigspeak library

const char* ssid = "realme Narzo 20";   //wifi name
const char* pass = "kshitijk";          //wifi password

WiFiClient client;

unsigned long myChannelNumber = 1430952;         //Thingspeak channel number
const char * myWriteAPIKey = "VV604FV82A7TGKLV"; //Thingspeak API key

char auth[] = "1mKYOsvaY-C3cc0gSZAex_RIs5At_9eC"; //Blynk authorisation key

//Defining variables
int val;
int input_val=0;
int LDRpin = A0;
int led1 = 14;   //D5
int led2 = 12;  //D6
int led3 = 13;  //D7
int P_C = 0;
float P_sum =0.0;
float P1=0.0;
float P2=0.0;
float P3=0.0;
float sum = 0.0;
float last_time ;
float current_time;
float Wh = 0;
float price = 0; 
float W=0;
float C_price = 0;
float saving =0;
int input_price=6;

#define BULB_PIN 14,12,13 // D5,D6,D7
WidgetLED BULB(V5);       //Bulb status in blynk app

void setup() {
  Blynk.begin(auth, ssid, pass);
  Serial.begin(9600);
  
  pinMode(16, INPUT); //D0
  pinMode(5, INPUT); //D1
  pinMode(4, INPUT); //D2
  pinMode(LDRpin, INPUT);

  pinMode(led1, OUTPUT);  //D5
  pinMode(led2, OUTPUT);  //D6
  pinMode(led3, OUTPUT);  //D7
  delay(10);
  WiFi.begin(ssid, pass);

  ThingSpeak.begin(client);

}

BLYNK_WRITE(V6)      //For writing LDR threshhold value from Blynk
{
  input_val = param.asInt();  //assigning incoming value from pin V6 to a variable
  Serial.print("The Threshold Value is: ");
  Serial.print(input_val);
  Serial.println();
}

void loop() {
  
  Blynk.run();
  val = analogRead(LDRpin);
  Serial.println(val);
  Serial.println(".Microwave sensor 1:"+digitalRead(16));
  Serial.println(".Microwave sensor 2:"+digitalRead(5));
  Serial.println(".Microwave sensor 3:"+digitalRead(4));

  delay(100);
  //sending data to thingspeak in respective channel number and field
  ThingSpeak.writeField(myChannelNumber, 1, val, myWriteAPIKey);           
  ThingSpeak.writeField(myChannelNumber, 2, digitalRead(16), myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3, digitalRead(5), myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 4, digitalRead(4), myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 6, P_C, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 5, P_sum, myWriteAPIKey);

  //Comparing threshold value
  if (val <= input_val)
  {
    P_C = 27;  //power consumption when all the bulbs are glown with its full brightness
    digitalWrite(led1, HIGH);
    analogWrite(led1, 255 / 20);

    digitalWrite(led2, HIGH);
    analogWrite(led2, 255 / 20);

    digitalWrite(led3, HIGH);
    analogWrite(led3, 255 / 20);
    P1=1;
    P2=1;
    P3=1;
    

    if (digitalRead(16) > 0) {  //If senor detects the vehicles
      digitalWrite(led1, HIGH);
      analogWrite(led1, 255);
      BULB.on();
      delay(1000);
      P1 = 9;

    }
    else {
      digitalWrite(led1, HIGH);
      analogWrite(led1, 255 / 20);
      P1 = 5;
      
      BULB.on();

    }//----------------------------------------------
    if (digitalRead(5) > 0) {
      digitalWrite(led2, HIGH);
      analogWrite(led2, 255);
      P2=9;
      BULB.on();
      delay(1000);

    }
    else {
      digitalWrite(led2, HIGH);
      analogWrite(led2, 255 / 20);
      P2=5;
      BULB.on();

    }//---------------------------------------------
    if (digitalRead(4) > 0) {
      digitalWrite(led3, HIGH);
      analogWrite(led3, 255);
      P3=9;
      BULB.on();
      delay(1000);

    }
    else {
      digitalWrite(led3, HIGH);
      analogWrite(led3, 255 / 20);
      P3 = 5;
      BULB.on();

    }//----------------------------------------------------------



  }
  else {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    P_C = 0;     //power consumption when no bulb glows
    P1=0;
    P2=0;
    P3=0;
    
    BULB.off();

  }
  //For electricity consumption and power cost calculation
  P_sum = P1+P2+P3;
  last_time = current_time;
  current_time = millis();
  Wh = Wh +P_sum*((current_time-last_time)/3600000.0);
  price = (Wh*input_price);
  W = W +P_C*((current_time-last_time)/3600000.0);
  C_price = (W*input_price);
  saving = saving +(C_price - price);
  
  ThingSpeak.writeField(myChannelNumber, 7, price, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 8, saving, myWriteAPIKey);

//sending data to blynk in virtual key
  Blynk.virtualWrite(V1,val);
  Blynk.virtualWrite(V2,digitalRead(16));
  Blynk.virtualWrite(V3,digitalRead(5));
  Blynk.virtualWrite(V4,digitalRead(4));
  Blynk.virtualWrite(V7,Wh);
  Blynk.virtualWrite(V8,price);
  Blynk.virtualWrite(V9,saving);

  delay(1000);
}
