#include <Wire.h>
#include "rgb_lcd.h"
#include <Twitter.h>
#include "Ethernet.h";
#include <SPI.h>

rgb_lcd lcd;

byte mac[] = { 0x78, 0x4b, 0x87, 0xaa, 0xad, 0x19 };
char server[] = "api.thingspeak.com";
IPAddress ip(192, 168, 0, 177);
EthernetClient client;
const String apiKey = "ASW3XH7WESWCG12G";
const String sendNumber = "17326106075";
int colorR = 0;
int colorG = 100;
int colorB = 100;
//int sensorValue = 0;
const int touch = 7;
const int led = 13;
const int button = 8;
bool armed = false;
int trigger = 0;
//int buttonState = 0;
int previous = LOW;
int buttonState = digitalRead(button);
int sensorValue = digitalRead(touch);


void setup() {
  Serial.begin(9600);
  pinMode(touch, INPUT);
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  lcd.begin(16, 1);
  pinMode(4, OUTPUT);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("READY");
}
void sendMessage()
{
  Serial.begin(9600);

  //set up Ethernet:
  setupEthernet();

  //send the sms
  Serial.println("Sending SMS");

  //this function will send the sms

  sendSMS(sendNumber, URLEncode("House Entry"));

}
void sendSMS(String number, String message)
{

  if (client.connect(server, 80))
  {

    client.print("GET /apps/thinghttp/send_request?api_key=");
    client.print(apiKey);
    client.print("&number=");
    client.print(number);
    client.print("&message=");
    client.print(message);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    Serial.println(F("Connection failed"));
  }

  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      Serial.print(c);
    }
  }
  Serial.println();
  client.stop();
}
void setupEthernet()
{
  Serial.println("Setting up Ethernet...");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

String URLEncode(const char* msg)
{
  const char *hex = "0123456789abcdef";
  String encodedMsg = "";

  while (*msg != '\0') {
    if ( ('a' <= *msg && *msg <= 'z')
         || ('A' <= *msg && *msg <= 'Z')
         || ('0' <= *msg && *msg <= '9') ) {
      encodedMsg += *msg;
    }
    else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}

void loop() {
  buttonState = digitalRead(button);
  sensorValue = digitalRead(touch);
  Serial.println(buttonState);
  if (buttonState == 1 && trigger == 0 && sensorValue == LOW) {
    digitalWrite(led, LOW);
    Serial.println("n");
    lcd.print("ARMED               ");

  }
  else {
    digitalRead(touch);
    if ((sensorValue == HIGH) && (buttonState == 1)) {
      digitalWrite(led, HIGH);
      lcd.print("ALERT. ALERT. ALERT.");
      Serial.println("y");

      digitalWrite(4, HIGH);
      delay(1000);
      digitalWrite(4, LOW);
      sendMessage();
      trigger = 1;

    }
  }
}
