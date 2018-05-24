#include <SFE_BMP180.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


#define PROCESSOR_OK 13
#define ERROR_MESSAGE 15
#define BATTERY_ONE 12
#define BATTERY_TWO 14


#define TRY_COUNT 50
#define LOGIN "Innopolis"
#define PASSWORD "Innopolis"
#define IP "138.68.84.189"
#define PORT 9090
#define DELAY 5000


//Errors
#define NO_PROBLEM            0
#define BMP_INIT_FAIL         1
#define BMP_PRESSURE_ERROR    2
#define BMP_TEMP_ERROR        3
#define WIFI_CONNECT_ERROR    4
#define SERVER_CONNECT_ERROR  5
#define BATTERY_LOW           6
#define SERVER_ECHO_ERROR     7


void(* resetFunc) (void) = 0;

SFE_BMP180 sensor;
ESP8266WiFiMulti WiFiMulti;
WiFiClient client;

int send_counter = 0;
int err_counter = NO_PROBLEM;
const int analog_ip = A0;

void setup()
{
  pinMode(PROCESSOR_OK,  OUTPUT);
  pinMode(ERROR_MESSAGE, OUTPUT);
  pinMode(BATTERY_ONE,   OUTPUT);
  pinMode(BATTERY_TWO,   OUTPUT); 
  digitalWrite(PROCESSOR_OK,HIGH);
  
  Serial.begin(74880);
  Serial.println();
  if (sensor.begin())
    err_counter = NO_PROBLEM;
  else
  {
    err_counter = BMP_INIT_FAIL;
  }


  WiFi.mode(WIFI_STA);                                              //Trying to connect WiFi TRY_COUNT times
  WiFiMulti.addAP( LOGIN , PASSWORD);
  int counter = 0;
  while(WiFiMulti.run() != WL_CONNECTED && counter <= TRY_COUNT) {
        delay(500);
        counter++;
  }


  if (counter >= TRY_COUNT)
    err_counter = WIFI_CONNECT_ERROR;
  error_message(err_counter); 
}

void loop() {                           //Handling BMP180 temperature-pressure sensor
    int temperature,pressure;    
    char status;
    double T,P;
    status = sensor.startTemperature();
    if (status != 0) {
      delay(status);
      status = sensor.getTemperature(T);
      if (status != 0) {
        temperature = int(T*100);
        status = sensor.startPressure(3);
        if (status != 0) {
          delay(status);
          status = sensor.getPressure(P,T);
          if (status != 0) {
            pressure = int(P*100);
          }
          else err_counter = BMP_PRESSURE_ERROR;
        }
        else err_counter = BMP_PRESSURE_ERROR;
      }
      else err_counter = BMP_TEMP_ERROR;
    }
    else err_counter = BMP_TEMP_ERROR;
    
  float inputVal = 0;
  
  for (int i = 0; i < DELAY/500; i++) {   //Delay + Blinking PROCESSOR_OK LED
    digitalWrite(PROCESSOR_OK,HIGH);
    delay(250);
    digitalWrite(PROCESSOR_OK,LOW);
    delay(250);
    inputVal = analogRead (analog_ip) * 3.3 / 1024; 

    if (inputVal >= 2.34) {                // Handling battery voltage
      digitalWrite(BATTERY_ONE, HIGH);
      digitalWrite(BATTERY_TWO, HIGH);
    } else if (inputVal >= 1.95) {
      digitalWrite(BATTERY_ONE, HIGH);
      digitalWrite(BATTERY_TWO, LOW);
    } else if (inputVal >= 1.43) {
      digitalWrite(BATTERY_ONE, LOW);
      digitalWrite(BATTERY_TWO, LOW);
    } else
      error_message(BATTERY_LOW);
  }
  
    if (!client.connect(IP, PORT)) {       //Establishing connection
        error_message(SERVER_CONNECT_ERROR);
    }
    client.println(String(temperature) + " " + String(pressure));     //Measure Output

    
    if (client.readStringUntil('\n') != "1")
      send_counter += 1;
    else
      send_counter = (send_counter - 1) >= 0 ? (send_counter - 1) : 0;
      
    if (send_counter >=5)
      error_message(SERVER_ECHO_ERROR);

    client.stop();                            
}


void error_message(int code) {      // Handling errors
  digitalWrite(BATTERY_ONE, LOW);
  digitalWrite(BATTERY_TWO, LOW);
  if (code == NO_PROBLEM)                    //No error
    return;
  else
  for (int j = 0; j <= 5; j++) {
    for (int i = 1; i <= code; i++) {
      digitalWrite(ERROR_MESSAGE, HIGH);
      delay(200);
      digitalWrite(ERROR_MESSAGE, LOW);
      delay(200);
    }
  delay(2000);
    if (code == BATTERY_LOW)           //in case of low battery error
      j--;                             //repeat loop forever
  }
  resetFunc();
}

