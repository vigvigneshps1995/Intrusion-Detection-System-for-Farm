
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "StepperMotor.h"
#include <math.h>

#define PIN_STEPPER_IN1 14
#define PIN_STEPPER_IN2 12
#define PIN_STEPPER_IN3 13
#define PIN_STEPPER_IN4 15
#define PIN_SERVO 16

const char* ssid = "Vignesh";
const char* password = "21021995";
const char* mqttServer = "broker.hivemq.com";
char nodeClientId[100] = "Camera-1";
const int numTopics = 4;
String topics[numTopics] = {"CameraSensor/ Camera-1/Pan", "CameraSensor/Camera-1/Tilt", "CameraSensor/Camera-1/Reset","CameraSensor/Camera-1/Calibrate"};

WiFiClient EspClient;
PubSubClient Client1(EspClient);
Servo Servo1;
StepperMotor Motor1;

void setup_wifi()
{
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
  randomSeed(micros());
}

void connect_broker()
{
  int i;

  Client1.setServer(mqttServer, 1883);

  while (!Client1.connect(nodeClientId))
    delay(100);

  for (i = 0; i < numTopics; i++)
    Client1.subscribe(topics[i].c_str());

  Client1.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int len)
{
  int angle;
  char message[10];
      
  memcpy(message,payload,len);

      Serial.println("message");
    Serial.println(message);

  //CAMERA PAN
  if (( strcmp(topic, topics[0].c_str() )) == 0)
  {
    angle = ( (String)(message) ).toInt();
    Motor1.stepperPan(angle);
  }

  //CAMERA TILT
  if (( strcmp(topic, topics[1].c_str() )) == 0)
  {
    angle = ( (String)(message) ).toInt();

    //HIGHER THE DEGREE LOWER THECAMERA LOOKS
    Servo1.attach(PIN_SERVO);
    Servo1.write(angle);
  }

  //BRING BACK THE CAMERA TO INITIAL POSITION
  if (( strcmp(topic, topics[2].c_str() )) == 0)
  {
    Motor1.stepperPan(0);
    
    Servo1.attach(PIN_SERVO);
    Servo1.write(180);
  }
  //CALIBRATE THE SENSOR
  if (( strcmp(topic, topics[3].c_str() )) == 0)
  {
    angle = ( (String)(message) ).toInt();
    Motor1.stepperCalibrate(angle);
  }
  strcpy(message,"");

}

void reconnect()
{
  while (!Client1.connected())
    connect_broker();
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_STEPPER_IN1, OUTPUT);
  pinMode(PIN_STEPPER_IN2, OUTPUT);
  pinMode(PIN_STEPPER_IN3, OUTPUT);
  pinMode(PIN_STEPPER_IN4, OUTPUT);
  pinMode(PIN_SERVO, OUTPUT);
  
  Motor1.attachPins(PIN_STEPPER_IN1, PIN_STEPPER_IN2, PIN_STEPPER_IN3, PIN_STEPPER_IN4);

  Serial.begin(9600);

  setup_wifi();
  connect_broker();
}

void loop()
{

  if (!Client1.connected())
    reconnect();
  
  /* long now = millis();
    if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    Client1.publish("outTopic", msg);
    }*/

  Client1.loop();
  Client1.setCallback(callback);

}


