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

  /*
   * czujnik swiatla:
   * 5V, DATA, przez 10k do GND
   */

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <dht.h>
#include <IRremote.h>
dht DHT;
char message_buff[100];

//******************************************************************************************************//
//do pilota TV:
unsigned int irLen = 0; // The length of the code
unsigned int irBuffer[RAWBUF]; // Storage for raw (UNKNOWN) codes
int codeType; // The type of code (NEC, SONY, UNKNOWN, etc.)
unsigned int codeValue; // The value of a known code
boolean codeReceived = false; // A boolean to keep track of if a code was received
int voldn;      //do volume mqtt
int volup;      //do volume mqtt

int d_lis_6 = 22;    //listwa 1
int d_lis_7 = 23;    //listwa 2
int d_lis_8 = 24;    //listwa 3
int d_lis_9 = 25;    //listwa 4
int d_lis_10 = 26;    //listwa 5
int d_io_1 = 27;     //wlacznik swiatla duzy pokoj
int k_io_1 = 30;     //wlacznik swiatla korytarz
int q_io_1 = 29;     //wlacznik swiatla kuchnia
#define DHT11_PIN A0 //czujnik temp i wilgoci na balkonie
int b_czuj_1 = A1;   //czujnik swiatla na balkonie
int lumen;           //do odczytu swiatla
int RECV_PIN = 3;    //czujnik IR pilota
int q_io_1_status=0; //do wlaczania swiatla pilotem
int d_io_1_status=0; //do wlaczania swiatla pilotem
int k_io_1_status=0; //do wlaczania swiatla pilotem
IRrecv irrecv(RECV_PIN); 
decode_results results;
IRsend irsend;       //nadajniki IR pilotów, pin 9
int lodowka=1;          //do wyciszania buzzera w trakcie snu

int q_czuj_1 = A2;   //czujnik otwartych drzwi lodowki
int q_czuj_2 = A3;   //czujnik otwartych drzwi zamrazarki
int magnet1;         //do odczytu cuzjnika drzwi lodowki
int magnet2;         //do odczytu czujnika drzwi zamrazarki
int buzzer = 6;      //buzzer
int button1 = A4;    //przycisk wlaczania swiatla w kuchni
int button2 = A5;    //przycisk wlaczania swiatla w pokoju

unsigned long code;  //do pilota

long interval1 = 60000;           // przerwa w update czujnika temp
unsigned long currentMillis1;
long previousMillis1 = 0;  

long interval2 = 1000;           // przerwa w update czujnika drzwi lodowki do mqtt
unsigned long currentMillis2;
long previousMillis2 = 0;  

long interval3 = 30000;           // buzzer sie wlaczy gdy drzwi beda otwarte tyle czasu
unsigned long currentMillis3;
long previousMillis3 = 0;  

long interval4 = 200;           // alarm buzzera bedzie pipal co tyle ms
unsigned long currentMillis4;
long previousMillis4 = 0;  

long interval5 = 2000;           // przerwa miedzy wlaczeniem/wylaczeniem listwy i wyslaniem sygnalu IR
unsigned long currentMillis5;
long previousMillis5 = 0;  

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xCA, 0xFB, 0xFE, 0xBA };
IPAddress ip(192, 168, 0, 178);      //adres Arduino
IPAddress server(192, 168, 0, 103);  //adres brokera

//************************************************************************************************************//

byte kom_ON[4] = "ON";       //wlacz listwe  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t m_lis_1/status -m ON
byte kom_OFF[5] = "OFF";     //wylacz listwe  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t m_lis_1/status -m OFF

byte kom_UP[4] = "UP";       //volume up   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - UP
byte kom_DN[4] = "DN";       //volume down CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - DN

byte kom_0[3] = "0";       //wlacz listwe  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t kom_0/status -m OFF
byte kom_1[3] = "1";       //wlacz listwe  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t kom_1/status -m ON
byte kom_2[3] = "2";      //volume 10   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 2
byte kom_3[3] = "3";      //volume 10   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 3
byte kom_4[3] = "4";      //volume 10   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 4
byte kom_5[3] = "5";      //volume 10   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 5
byte kom_6[3] = "6";      //volume 10   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 6
byte kom_10[4] = "10";      //volume 10   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 10
byte kom_20[4] = "20";      //volume 20   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 20
byte kom_30[4] = "30";      //volume 30   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 30
byte kom_40[4] = "40";      //volume 40   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 40
byte kom_50[4] = "50";      //volume 50   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 50
byte kom_60[4] = "60";      //volume 60   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 60
byte kom_70[4] = "70";      //volume 70   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 70
byte kom_80[4] = "80";      //volume 80   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 80
byte kom_90[4] = "90";      //volume 90   CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 90
byte kom_100[5] = "100";    //volume 100  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot/amp - 100

byte pilotsen[5] = "sen";       //wylacz TV, DVBT i AMP  CMD-- mosquitto_pub -h 192.168.0.103 -p 1883 -t pilot -m sen

//**********************************************************************************************************//

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {  //wiadomosc z brokera
  
  //////////////////listwy//////////////////////
  
if (strcmp(topic,"lis/d/6")==0) {      //jesli w temacie jest wiadomosc dla listwy 6
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  digitalWrite(d_lis_6, HIGH);
  client.publish("d_lis_6/status","ON");
  Serial.println("lis/d/6=ON");
  }
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  digitalWrite(d_lis_6, LOW);
  client.publish("d_lis_6/status","OFF");
  Serial.println("lis/d/6=OFF");
}
}
if (strcmp(topic,"lis/d/7")==0) {      //jesli w temacie jest wiadomosc dla listwy 2
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  digitalWrite(d_lis_7, HIGH);
  client.publish("d_lis_7/status","ON");
  Serial.println("lis/d/7=ON");
}
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  digitalWrite(d_lis_7, LOW);
  client.publish("d_lis_7/status","OFF");
  Serial.println("lis/d/7=OFF");
}
}
if (strcmp(topic,"lis/d/8")==0) {      //jesli w temacie jest wiadomosc dla listwy 3
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  digitalWrite(d_lis_8, HIGH);
  client.publish("d_lis_8/status","ON");
  Serial.println("lis/d/8=ON");
}
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  digitalWrite(d_lis_8, LOW);
  client.publish("d_lis_8/status","OFF");
  Serial.println("lis/d/8=OFF");
}
}
if (strcmp(topic,"lis/d/9")==0) {      //jesli w temacie jest wiadomosc dla listwy 4
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  digitalWrite(d_lis_9, HIGH);
  client.publish("d_lis_9/status","ON");
  Serial.println("lis/d/9=ON");
}
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  digitalWrite(d_lis_9, LOW);
  client.publish("d_lis_9/status","OFF");
  Serial.println("lis/d/9=OFF");
}
}
if (strcmp(topic,"lis/d/10")==0) {      //jesli w temacie jest wiadomosc dla listwy 5
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  digitalWrite(d_lis_10, LOW);
  client.publish("d_lis_10/status","ON");
  Serial.println("lis/d/10=ON");
}
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  digitalWrite(d_lis_10, HIGH);
  client.publish("d_lis_10/status","OFF");
  Serial.println("lis/d/10=OFF");
}
}

//////////////wlaczniki swiatla//////////////////

if (strcmp(topic,"io/d")==0) {      //jesli w temacie jest wiadomosc dla wlacznika w duzym pokoju
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  delay(50);
  digitalWrite(d_io_1, HIGH);
  d_io_1_status=1;
  digitalWrite(d_io_1, HIGH);
  client.publish("d_io_1/status","ON");
  Serial.println("io/d=ON");
  }
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  delay(50);
  digitalWrite(d_io_1, LOW);
  d_io_1_status=0;
  digitalWrite(d_io_1, LOW);
  client.publish("d_io_1/status","OFF");
  Serial.println("io/d=OFF");
}
}
if (strcmp(topic,"io/k")==0) {      //jesli w temacie jest wiadomosc dla wlacznika w korytarzu
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  delay(50);
  digitalWrite(k_io_1, HIGH);
  k_io_1_status=1;
  client.publish("k_io_1/status","ON");
  Serial.println("io/k=ON");
  }
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  delay(50);
  digitalWrite(k_io_1, LOW);
  k_io_1_status=0;
  client.publish("k_io_1/status","OFF");
  Serial.println("io/k=OFF");
}
}
if (strcmp(topic,"io/q")==0) {      //jesli w temacie jest wiadomosc dla wlacznika w kuchni
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  delay(50);
  digitalWrite(q_io_1, HIGH);
  q_io_1_status=1;
  digitalWrite(q_io_1, HIGH);
  client.publish("q_io_1/status","ON");
  Serial.println("io/q=ON");
  }
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  delay(50);
  digitalWrite(q_io_1, LOW);
  q_io_1_status=0;
  digitalWrite(q_io_1, LOW);
  client.publish("q_io_1/status","OFF");
  Serial.println("io/q=OFF");
}
}

/////////////////////////////pilot//////////////////

if (strcmp(topic,"wylacznik")==0) {      //jesli w temacie jest wiadomosc dla pilota amp wlacz tryb nocny AMP
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  irsend.sendNEC(0x5EA1A956, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
  Serial.println("AMP tryb nocny");
  delay(40);
irsend.sendNEC(0xFFFFFFFF, 32);
delay(40);
irrecv.enableIRIn();
irrecv.resume();
}
}

if (strcmp(topic,"pilot/amp")==0) {      //jesli w temacie jest wiadomosc dla pilota amp
  irrecv.resume();
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  irsend.sendNEC(0x5EA1906E, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
  client.publish("pilot/amp/status","ON");
  Serial.println("AMP ON");
  }
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  irsend.sendNEC(0x7E81FE01, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
  client.publish("pilot/amp/status","OFF");
  Serial.println("AMP OFF");
}
if ((length == 2) & (memcmp(payload, kom_UP, 2) == 0) ){
  irsend.sendNEC(0x5EA158A7, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
  client.publish("pilot/amp/status","UP");
  Serial.println("AMP Vol UP");
  }
if ((length == 2) & (memcmp(payload, kom_DN, 2) == 0) ){
  irsend.sendNEC(0x5EA1D827, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
  client.publish("pilot/amp/status","DN");
  Serial.println("AMP VOL DOWN");
}
if ((length == 1) & (memcmp(payload, kom_1, 1) == 0) )
  volup=1;
if ((length == 1) & (memcmp(payload, kom_2, 1) == 0) )
  volup=5;
if ((length == 1) & (memcmp(payload, kom_3, 1) == 0) )
  volup=10;
if ((length == 1) & (memcmp(payload, kom_4, 1) == 0) )
  voldn=1;
if ((length == 1) & (memcmp(payload, kom_5, 1) == 0) )
  voldn=5;
if ((length == 1) & (memcmp(payload, kom_6, 1) == 0) )
  voldn=10;

if ((length == 2) & (memcmp(payload, kom_10, 2) == 0) )
  voldn=20;
if ((length == 2) & (memcmp(payload, kom_20, 2) == 0) )
  voldn=16;
if ((length == 2) & (memcmp(payload, kom_30, 2) == 0) )
  voldn=12;
if ((length == 2) & (memcmp(payload, kom_40, 2) == 0) )
  voldn=8;
if ((length == 2) & (memcmp(payload, kom_50, 2) == 0) )
  voldn=4;
if ((length == 2) & (memcmp(payload, kom_60, 2) == 0) )
  volup=4;
if ((length == 2) & (memcmp(payload, kom_70, 2) == 0) )
  volup=8;
if ((length == 2) & (memcmp(payload, kom_80, 2) == 0) )
  volup=12;
if ((length == 2) & (memcmp(payload, kom_90, 2) == 0) )
  volup=16;
if ((length == 3) & (memcmp(payload, kom_100, 3) == 0) )
  volup=20;

if (voldn > 0){        //zmniejszyc vol
Serial.print("AMP VOL DOWN ");
Serial.println(voldn);
for (int x = 0; x < voldn; x ++){
irsend.sendNEC(0x5EA1D827, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
delay(40);
irsend.sendNEC(0xFFFFFFFF, 32);
delay(40);
irrecv.enableIRIn();
irrecv.resume();
}
}

if (volup > 0){       //zwiekszyc vol
Serial.print("AMP VOL UP ");
Serial.println(volup);
for (int x = 0; x < volup; x ++){
irsend.sendNEC(0x5EA158A7, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
delay(40);
irsend.sendNEC(0xFFFFFFFF, 32);
delay(40);
irrecv.enableIRIn();
irrecv.resume();
}
}

delay(40);
irsend.sendNEC(0xFFFFFFFF, 32);
delay(40);
irrecv.enableIRIn();
irrecv.resume();
voldn = 0;
volup = 0;
}

if (strcmp(topic,"pilot/tv")==0) {      //jesli w temacie jest wiadomosc dla pilota amp
irrecv.resume();
if ((length == 2) & (memcmp(payload, kom_ON, 2) == 0) ){
  Serial.println("TV ON");
irsend.sendRC5(2060, 12);             //wylacz TV
delay(40);
irsend.sendRC5(12, 12);  
}
if ((length == 3) & (memcmp(payload, kom_OFF, 3) == 0) ){
  Serial.println("TV OFF");
irsend.sendRC5(2060, 12);             //wylacz TV
delay(40);
irsend.sendRC5(12, 12);  
delay(40);
}
}

if (strcmp(topic,"scena")==0) {      //jesli w temacie jest wiadomosc dla pilota amp
if ((length == 1) & (memcmp(payload, kom_3, 1) == 0) )
lodowka=0;
else
lodowka=1;
}
}

//*******************************************************************************************************************//

void setup(){
pinMode(d_lis_6, OUTPUT);
pinMode(d_lis_7, OUTPUT);
pinMode(d_lis_8, OUTPUT);
pinMode(d_lis_9, OUTPUT);
pinMode(d_lis_10, OUTPUT);
pinMode(d_io_1, OUTPUT);
pinMode(k_io_1, OUTPUT);
pinMode(q_io_1, OUTPUT);
pinMode(buzzer, OUTPUT);
/*digitalWrite(d_lis_1, HIGH);    //HIGH=OFF
digitalWrite(d_lis_2, HIGH);
digitalWrite(d_lis_3, HIGH);
digitalWrite(d_lis_4, HIGH);
digitalWrite(d_lis_5, HIGH);
client.publish("d_lis_1/status","OFF");
client.publish("d_lis_2/status","OFF");
client.publish("d_lis_3/status","OFF");
client.publish("d_lis_4/status","OFF");
client.publish("d_lis_5/status","OFF");*/
irrecv.enableIRIn();            // Start the receiver

Serial.begin(9600);
  Ethernet.begin(mac, ip);
  if (client.connect("arduinoClient")) {
    client.publish("outTopic","hello world");
    client.subscribe("lis/#");  //sluchaj wszystkiego pod topikiem listwy
    client.subscribe("io/#");  //sluchaj wszystkiego pod topikiem wlacznika
    client.subscribe("czuj/k/1");  //sluchaj wszystkiego pod topikiem czujnika ruchu
    client.subscribe("pilot/#");  //sluchaj wszystkiego pod topikiem pilota
    client.subscribe("scena");
    client.subscribe("wylacznik");
  }
}

//**********************************************************************************************************************//

void loop(){
    client.loop();          //do mqtt
    pilot();                //do pilota
    meteo();                //do czujników temp, wilgoci i swiatla na balkonie
    button();
    if (lodowka==1)         //jesli wlaczam wylacznaie, czyli przycisk ustawspanie to nie brzecz
    magnet();               //do czujnikoów otwartych drzwi lodówki
    
}

//**************************************************************************************************************************//

void pilot(){
code=0;
if (irrecv.decode(&results)) {
code = results.value;
Serial.print("Protocol: ");
Serial.println(results.decode_type, DEC);
Serial.println(results.value, HEX);
//Serial.println(code);
//client.publish("pilot",String(code, 20).c_str());
irrecv.resume(); // Receive the next value
/*
 * Ustawienia dla DVB-T
Pilot Yamaha ustawiony na DVR 
Wcisnac i trzymac przycisk DVR i przez 3 sekundy trzymac razem z POWER AV, potem kod 2078:
*/

/*    ON/OFF dla TV
 *    codeValue = 2060;
      irLen = 12;
      Serial.println(codeValue, HEX);
      irsend.sendRC5(codeValue, irLen);
      delay(40);
irsend.sendRC5(12, 12);
 */
switch (code){
case 0x9E618877:                  //jesli nacisniety przycisk 1 pod CD swiatlo w kuchni
if(q_io_1_status==1){
Serial.println("io/q OFF");
  client.publish("q_io_1/status","OFF");
  client.publish("io/q","OFF");
  Serial.print("io/q ");
  Serial.println(q_io_1_status);
}
if(q_io_1_status==0){
Serial.println("io/q ON");
  client.publish("q_io_1/status","ON"); 
  client.publish("io/q","ON");
  Serial.print("io/q ");
  Serial.println(q_io_1_status);
}
break; 

case 0x9E6148B7:                  //jesli nacisniety przycisk 2 pod CD swiatlo w pokoju
if(d_io_1_status==1){
Serial.println("io/d OFF");
  client.publish("d_io_1/status","OFF");
  client.publish("io/d","OFF");
  Serial.print("io/d ");
  Serial.println(d_io_1_status);
}
if(d_io_1_status==0){
Serial.println("io/d ON");
  client.publish("d_io_1/status","ON"); 
  client.publish("io/d","ON");
  Serial.print("io/d ");
  Serial.println(d_io_1_status);
}
break; 

case 0x9E61C837:                  //jesli nacisniety przycisk 3 pod CD swiatlo w korytarzu
if(k_io_1_status==1){
Serial.println("io/k OFF");
  client.publish("k_io_1/status","OFF");
  client.publish("io/k","OFF");
  Serial.print("io/k ");
  Serial.println(k_io_1_status);
}
if(k_io_1_status==0){
Serial.println("io/k ON");
  client.publish("k_io_1/status","ON"); 
  client.publish("io/k","ON");
  Serial.print("io/k ");
  Serial.println(k_io_1_status);
}
break; 
  
case 0x7E817E81:                  //jesli nacisniety przycisk ON AMP na pilocie to wlacz listwe i daj update do openhaba na listwe i AMP
Serial.println("ON AMP");
  digitalWrite(d_lis_6, HIGH);
  client.publish("pilot/amp/status","ON");
  client.publish("d_lis_6/status","ON");
break; 
case 0x8B7F807:
Serial.println("ONOFF");
  irsend.sendNEC(0x23750AF, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B752AD:
  Serial.println("UP");
  irsend.sendNEC(0x237D02F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7D22D:
  Serial.println("DOWN");
  irsend.sendNEC(0x237708F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B710EF:
  Serial.println("RIGHT");
  irsend.sendNEC(0x2378877, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7D02F:
  Serial.println("LEFT");
  irsend.sendNEC(0x23708F7, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7926D:
  Serial.println("OK");
  irsend.sendNEC(0x237B04F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B70AF5:
  Serial.println("INFO");
  irsend.sendNEC(0x23758A7, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B79867:
  Serial.println("EXIT");
  irsend.sendNEC(0x237827D, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7A25D:
  Serial.println("BLUE");
  irsend.sendNEC(0x2379867, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7827D:
  Serial.println("MENU");
  irsend.sendNEC(0x23702FD, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B718E7:
  Serial.println("0");
  irsend.sendNEC(0x23700FF, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B73AC5:
  Serial.println("1");
  irsend.sendNEC(0x237807F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B77887:
  Serial.println("2");
  irsend.sendNEC(0x23740BF, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B77A85:
  Serial.println("3");
  irsend.sendNEC(0x237C03F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B71AE5:
  Serial.println("4");
  irsend.sendNEC(0x23720DF, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B758A7:
  Serial.println("5");
  irsend.sendNEC(0x237A05F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7DA25:
  Serial.println("6");
  irsend.sendNEC(0x237609F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7D827:
  Serial.println("7");
  irsend.sendNEC(0x237E01F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B79A65:
  Serial.println("8");
  irsend.sendNEC(0x23710EF, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B75AA5:
  Serial.println("9");
  irsend.sendNEC(0x237906F, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
case 0x8B7E01F:
  Serial.println("REC");
  irsend.sendNEC(0x2372AD5, 32); // DVBT jedzie na protokolo 1 czyli NEC, zaczyna sie na 0x
break;
}
delay(40);
irsend.sendNEC(0xFFFFFFFF, 32);
delay(40);
irrecv.enableIRIn();
irrecv.resume();
}
}

//**************************************************************************************************************//

void meteo(){
lumen = analogRead(b_czuj_1);
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
                client.publish("czuj/b/1/wilg","N/A");    // error
                client.publish("czuj/b/1/temp","N/A");    // error
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                client.publish("czuj/b/1/wilg","N/A");    // error
                client.publish("czuj/b/1/temp","N/A");    // error
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                client.publish("czuj/b/1/wilg","N/A");    // error
                client.publish("czuj/b/1/temp","N/A");    // error
                break;
  }
Serial.print(",\t");
Serial.print(DHT.humidity ,1);
Serial.print(",\t");
Serial.println(DHT.temperature ,1);
  client.publish("czuj/b/1/wilg",String(DHT.humidity, 1).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
  client.publish("czuj/b/1/temp",String(DHT.temperature, 1).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
  client.publish("czuj/b/2",String(lumen).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
 previousMillis1 = currentMillis1;
 }  
}

//************************************************************************************************************//

void magnet(){
currentMillis2 = millis(); // do czujnika drzwi
currentMillis3 = millis(); // do alarmu drzwi
magnet1 = !digitalRead(q_czuj_1);
magnet2 = !digitalRead(q_czuj_2);
if(currentMillis2 - previousMillis2 > interval2) {
client.publish("czuj/q/1",String(magnet1).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
client.publish("czuj/q/2",String(magnet2).c_str());    //wydruk odczytu czujnika CMD-- mosquitto_sub -h 192.168.0.103 -p 1883 -t k_czuj_1
previousMillis2 = currentMillis2;
}
if (magnet1 == 0 || magnet2 == 0 ){                     //jesli ktores drzwi sa otwarte
if(currentMillis3 - previousMillis3 > interval3) {     //jesli drzwi sa otwarte dluzej niz interval3=10 sekund
  currentMillis4 = millis(); // do buzzera
  if(currentMillis4 - previousMillis4 > interval4) {
    analogWrite(buzzer, 0);
    if(currentMillis4 - previousMillis4 > 2*interval4){
    previousMillis4 = currentMillis4;
      }
  }
  else{
    analogWrite(buzzer, 100);
    }
}
else{
  analogWrite(buzzer, 0);  
}
}
else{
  analogWrite(buzzer, 0);
  previousMillis3 = currentMillis3;
}
}

void button(){
  if (digitalRead(button1) == HIGH){
  if(q_io_1_status==1){
Serial.println("io/q OFF");
  client.publish("q_io_1/status","OFF");
  client.publish("io/q","OFF");
  Serial.print("io/q ");
  Serial.println(q_io_1_status);
}
if(q_io_1_status==0){
Serial.println("io/q ON");
  client.publish("q_io_1/status","ON"); 
  client.publish("io/q","ON");
  Serial.print("io/q ");
  Serial.println(q_io_1_status);
}
delay(200);
}
 if (digitalRead(button2) == HIGH){
  if(d_io_1_status==1){
Serial.println("io/d OFF");
  client.publish("d_io_1/status","OFF");
  client.publish("io/d","OFF");
  Serial.print("io/d ");
  Serial.println(d_io_1_status);
}
if(d_io_1_status==0){
Serial.println("io/d ON");
  client.publish("d_io_1/status","ON"); 
  client.publish("io/d","ON");
  Serial.print("io/d ");
  Serial.println(d_io_1_status);
}
delay(200);
}

}

