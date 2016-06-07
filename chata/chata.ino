/*
 * Sciagnac pubsubclient wersja co najmniej 2.4
 * odpalic pubsubclient.h
 * i zmienic wersje MQTT na 3_1 
 */

/* nazwy urzadzen/status
 * pomieszczenie_rodzaj przedmiotu_numer 
 * ------------------
 * korytarz       - k
 * maly pokoj     - m
 * duzo pokoj     - d
 * kuchnia        - q
 * lazienka       - l
 * balkon         - b
 * ------------------
 * czujnik        - czuj
 * wlacznik swiatla-io
 * listwa         - lis
 * nadajnik       - nad
 */

 /*
  * DHT11:
  * VCC DATA  X GND
  */
 

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <dht.h>
dht DHT;
char message_buff[100];

int k_czuj_1 = A0;     //czujnik ruchu w korytarzu
int status_k_czuj_1 = 0;
int lux = 0;           //do odczytow czujnika ruchu
int m_lis_1 = 2;
int m_lis_2 = 3;
int m_lis_3 = 4;
int m_lis_4 = 5;
int m_lis_5 = 6;
int d_lis_1 = 7;
int d_lis_2 = 8;
int d_lis_3 = 9;
int d_lis_4 = A4;
int d_lis_5 = A5;
#define DHT11_PIN A1
int status_d_czuj_1 = 0;


long interval = 1000;           // przerwa w update czujnika ruchu
unsigned long currentMillis;
long previousMillis = 0;  

long interval1 = 60000;           // przerwa w update czujnika temp
unsigned long currentMillis1;
long previousMillis1 = 0;  

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xAA, 0xFE, 0xFE, 0xBA };
IPAddress ip(192, 168, 0, 167);      //adres Arduino
IPAddress server(192, 168, 0, 103);  //adres brokera

byte lis_ON[4] = "ON";       //wlacz listwe  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t m_lis_1/status -m ON
byte lis_OFF[5] = "OFF";     //wylacz listwe  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t m_lis_1/status -m OFF

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {  //wiadomosc z brokera

////////////////////////////////////////////listwa 1/////////////////////////////////////////////////////
  
if (strcmp(topic,"lis/m/1")==0) {      //jesli w temacie jest wiadomosc dla listwy 1
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(m_lis_1, LOW);
  client.publish("m_lis_1/status","ON");
  }
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(m_lis_1, HIGH);
  client.publish("m_lis_1/status","OFF");
}
}
if (strcmp(topic,"lis/m/2")==0) {      //jesli w temacie jest wiadomosc dla listwy 2
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(m_lis_2, HIGH);
  client.publish("m_lis_2/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(m_lis_2, LOW);
  client.publish("m_lis_2/status","OFF");
}
}
if (strcmp(topic,"lis/m/3")==0) {      //jesli w temacie jest wiadomosc dla listwy 3
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(m_lis_3, HIGH);
  client.publish("m_lis_3/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(m_lis_3, LOW);
  client.publish("m_lis_3/status","OFF");
}
}
if (strcmp(topic,"lis/m/4")==0) {      //jesli w temacie jest wiadomosc dla listwy 4
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(m_lis_4, HIGH);
  client.publish("m_lis_4/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(m_lis_4, LOW);
  client.publish("m_lis_4/status","OFF");
}
}
if (strcmp(topic,"lis/m/5")==0) {      //jesli w temacie jest wiadomosc dla listwy 5
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(m_lis_5, HIGH);
  client.publish("m_lis_5/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(m_lis_5, LOW);
  client.publish("m_lis_5/status","OFF");
}
}

////////////////////////////////////listwa 2/////////////////////////////////////////////////////////

if (strcmp(topic,"lis/d/1")==0) {      //jesli w temacie jest wiadomosc dla listwy 2
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(d_lis_1, HIGH);
  client.publish("d_lis_1/status","ON");
  }
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(d_lis_1, LOW);
  client.publish("d_lis_1/status","OFF");
}
}
if (strcmp(topic,"lis/d/2")==0) {      //jesli w temacie jest wiadomosc dla listwy 2
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(d_lis_2, LOW);
  client.publish("d_lis_2/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(d_lis_2, HIGH);
  client.publish("d_lis_2/status","OFF");
}
}
if (strcmp(topic,"lis/d/3")==0) {      //jesli w temacie jest wiadomosc dla listwy 3
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(d_lis_3, LOW);
  client.publish("d_lis_3/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(d_lis_3, HIGH);
  client.publish("d_lis_3/status","OFF");
}
}
if (strcmp(topic,"lis/d/4")==0) {      //jesli w temacie jest wiadomosc dla listwy 4
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(d_lis_4, LOW);
  client.publish("d_lis_4/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(d_lis_4, HIGH);
  client.publish("d_lis_4/status","OFF");
}
}
if (strcmp(topic,"lis/d/5")==0) {      //jesli w temacie jest wiadomosc dla listwy 5
if ((length == 2) & (memcmp(payload, lis_ON, 2) == 0) ){
  digitalWrite(d_lis_5, LOW);
  client.publish("d_lis_5/status","ON");
}
if ((length == 3) & (memcmp(payload, lis_OFF, 3) == 0) ){
  digitalWrite(d_lis_5, HIGH);
  client.publish("d_lis_5/status","OFF");
}
}

}

//*************************************************************************************************

void setup(){
pinMode(k_czuj_1, INPUT);
pinMode(m_lis_1, OUTPUT);
pinMode(m_lis_2, OUTPUT);
pinMode(m_lis_3, OUTPUT);
pinMode(m_lis_4, OUTPUT);
pinMode(m_lis_5, OUTPUT);
pinMode(d_lis_1, OUTPUT);
pinMode(d_lis_2, OUTPUT);
pinMode(d_lis_3, OUTPUT);
pinMode(d_lis_4, OUTPUT);
pinMode(d_lis_5, OUTPUT);
digitalWrite(m_lis_1, HIGH);    
digitalWrite(m_lis_2, LOW);
digitalWrite(m_lis_3, LOW);
digitalWrite(m_lis_4, LOW);
digitalWrite(m_lis_5, LOW);
digitalWrite(d_lis_1, LOW);    
digitalWrite(d_lis_2, HIGH);
digitalWrite(d_lis_3, HIGH);
digitalWrite(d_lis_4, HIGH);
digitalWrite(d_lis_5, HIGH);
client.publish("m_lis_1/status","OFF");
client.publish("m_lis_2/status","OFF");
client.publish("m_lis_3/status","OFF");
client.publish("m_lis_4/status","OFF");
client.publish("m_lis_5/status","OFF");
client.publish("d_lis_1/status","OFF");
client.publish("d_lis_2/status","OFF");
client.publish("d_lis_3/status","OFF");
client.publish("d_lis_4/status","OFF");
client.publish("d_lis_5/status","OFF");

Serial.begin(9600);
  Ethernet.begin(mac, ip);
  if (client.connect("arduinoClient")) {
    client.publish("outTopic","hello world");
    client.subscribe("lis/#");  //sluchaj wszystkiego pod topikiem listwy
  }
}

//************************************************************************************************

void loop(){
    client.loop(); 
    
lux = analogRead(k_czuj_1);
      currentMillis = millis(); // do czujnika ruchu
      currentMillis1 = millis(); // do czujnika temp

 if(currentMillis1 - previousMillis1 > interval1) {  //jesli juz minal czas to wyslij update czujnikow temp
int chk = DHT.read11(DHT11_PIN);
switch (chk)
  {
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                client.publish("czuj/d/1/wilg","N/A");    // error
                client.publish("czuj/d/1/temp","N/A");    // error
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                client.publish("czuj/d/1/wilg","N/A");    // error
                client.publish("czuj/d/1/temp","N/A");    // error
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                client.publish("czuj/d/1/wilg","N/A");    // error
                client.publish("czuj/d/1/temp","N/A");    // error
                break;
  }
Serial.print(",\t");
Serial.print(DHT.humidity ,1);
Serial.print(",\t");
Serial.println(DHT.temperature ,1);
  client.publish("czuj/d/1/wilg",String(DHT.humidity, 1).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
  client.publish("czuj/d/1/temp",String(DHT.temperature, 1).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
 previousMillis1 = currentMillis1;
 }
 if(currentMillis - previousMillis > interval) {  //jesli juz minal czas to wyslij update czujnikow ruchu
if (lux>= 450){                     //ruch wykryty
status_k_czuj_1=1;
  }
else{
  status_k_czuj_1=0;
  }
  client.publish("czuj/k/1",String(status_k_czuj_1).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1


Serial.print("ruch=  ");
Serial.print(lux);

previousMillis = currentMillis;
}
}
