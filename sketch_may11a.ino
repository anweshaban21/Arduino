/*
 # This sample code is used to test the pH meter V1.0.
 # Editor : YouYou
 # Ver    : 1.0
 # Product: analog pH meter
 # SKU    : SEN0161
*/
#include <ESP8266WiFi.h> //node mcu wifi
#include <ThingSpeak.h> //ThingSpeak

#define SensorPin A0            //pH meter Analog output to Arduino Analog Input 0
#define Offset 0.00            //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;
const char* ssid = "Anwesha";
const char* pass = "muktarani";
//Thingspeak
//-------------------------------------------------
WiFiClient client ;
long id = 2401064;
const char* api = "17W1AWDNMT8MIUUV" ;
char ip[] = "184.106.153.149";

void setup(void)
{
  // WIFI
//-----------------------------------------
Serial.begin(9600);
Serial.println("Connecting to WiFi");
WiFi.begin(ssid,pass);
while(WiFi.status()!= WL_CONNECTED){
 Serial.print(".");
 delay(500);
}
 Serial.println("WiFi Connected");
 //ThingSpeak
 //--------------------------------------------
 ThingSpeak.begin(client);

  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  Serial.println("pH meter experiment!");    //Test the serial monitor
}
void loop(void)
{
  

  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue,voltage;
  if(client.connect(ip,80)){

 //HCSR004
//  ------------------------------------------------------
//  Clears the trigPin condition
 digitalWrite(SensorPin, LOW);
 delayMicroseconds(2);
 // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
 digitalWrite(SensorPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(SensorPin, LOW);
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
        Serial.print(voltage,2);
        Serial.print("    pH value: ");
    Serial.println(pHValue,2);
        digitalWrite(LED,digitalRead(LED)^1);
        printTime=millis();
  }
  }
  ThingSpeak.setField(1,pHValue);
 // ThingSpeak.setField(2,voltage);
 ThingSpeak.writeFields(id,api);
 client.stop();
 delay(1000);
}
  
  


double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}