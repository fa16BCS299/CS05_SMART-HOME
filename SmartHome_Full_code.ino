#include <FirebaseESP32.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include <DNSServer.h>
WiFiServer server(80);
#include <BluetoothSerial.h>

#include <Arduino.h>

#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"


BluetoothSerial BT; // Connect Tx to pin 16--> RX of ESP32 and Rx to pin 17--> TX of ESP32 of HC-05/HC-06
WiFiClient client = server.available();   // listen for incoming clients


int  flag = 0;
int  flagMagnaticSwitch = 0;

String readData;          // String for storing data send from the Bluetooth device

//Define FirebaseESP32 data object
FirebaseData firebaseData;

int  flagBT = 1, flagWifi = 1, flagFB = 1;
#define FIREBASE_HOST "rjee-smarthome.firebaseio.com"
#define FIREBASE_AUTH "6H1nXWCcbXgSL2frkMYss71lZhxVdSdNz6ZnrRiq"

#define LED  2
#define Relay1  13
#define Relay2  12
#define Relay3  14
#define Relay4  27
#define Relay5  33
#define Relay6  32
#define Relay7  5
#define Relay8  18
#define flameSensor  36
#define MegnaticSwitch 34
#define GasSwitch 35

Talkie voice;


//int rel1 , rel2 , rel3 , rel4 , rel5 , rel6 , rel7, rel8;
int buttonState = 0;

void setup()
{
  delay(100);

  Serial.begin(115200);

  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);

  flagBT   = digitalRead(21);
  flagWifi = digitalRead(22);
  flagFB   = digitalRead(23);

  pinMode(LED, OUTPUT);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(Relay5, OUTPUT);
  pinMode(Relay6, OUTPUT);
  pinMode(Relay7, OUTPUT);
  pinMode(Relay8, OUTPUT);
  pinMode(MegnaticSwitch, INPUT_PULLUP);
  pinMode(flameSensor, INPUT);
  pinMode(GasSwitch, OUTPUT);



  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);
  digitalWrite(Relay3, HIGH);
  digitalWrite(Relay4, HIGH);
  digitalWrite(Relay5, HIGH);
  digitalWrite(Relay6, HIGH);
  digitalWrite(Relay7, HIGH);
  digitalWrite(Relay8, HIGH);


  flamewarning();
  Gaswarning();
  DoorOpenNotification();

  //  BlueTooth Code ~~~~~~~~~~~~~~~

  if (flagBT == 0) {
    flagWifi = 1;
    flagFB = 1;
    BT.begin("SmartHome_BlueTooth_Config");
  }

  //  Wi-Fi Code ~~~~~~~~~~~~~~~

  if (flagWifi == 0) {
    flagBT = 1;
    flagFB = 1;
    Wifisetup();

  }

  //  FireBase Code ~~~~~~~~~~~~~~~

  if (flagFB == 0) {
    flagWifi = 1;
    flagBT = 1;

    FireBasesetup();

  }
}

void loop()
{
  digitalWrite(LED, LOW);
  flagBT   = digitalRead(21);
  flagWifi = digitalRead(22);
  flagFB   = digitalRead(23);



  flamewarning();
  Gaswarning();
  DoorOpenNotification();




  if (flagBT == 0) {
    flagWifi = 1;
    flagFB = 1;
    //Serial.println("BT");
    digitalWrite(LED, HIGH);
    //delay(50);
    //digitalWrite(LED, LOW);
    //delay(50);
    //int  MAlarm = digitalRead(MegnaticSwitch);
    //int  FAlarm = analogRead(flameSensor);
    //int  GAlarm = analogRead(GasSwitch);

    while (BT.available())
    {
      delay(10);
      char c = BT.read();
      readData += c;

      //      if (MAlarm == 1) {
      //        delay(10);
      //        BT.write(1);
      //        Serial.println("Fire1");
      //      }
      //      else if (MAlarm == 0) {
      //        BT.write(0);
      //        Serial.println("Fire0");
      //
      //      }

    }
    if (readData.length() > 0)
    {
      Serial.println(readData);

      BTswitching(readData, "S1 on", "S1 off", Relay1);
      BTswitching(readData, "S2 on", "S2 off", Relay2);
      BTswitching(readData, "S3 on", "S3 off", Relay3);
      BTswitching(readData, "S4 on", "S4 off", Relay4);
      BTswitching(readData, "S5 on", "S5 off", Relay5);
      BTswitching(readData, "S6 on", "S6 off", Relay6);
      BTswitching(readData, "S7 on", "S7 off", Relay7);
      BTswitching(readData, "S8 on", "S8 off", Relay8);


      readData = "";



    }
    digitalWrite(LED, LOW);

  }

  if (flagWifi == 0) {
    flagBT = 1;
    flagFB = 1;

    //int  MAlarm = digitalRead(MegnaticSwitch);
    //int  FAlarm = analogRead(flameSensor);
    //int  GAlarm = analogRead(GasSwitch);


    //Serial.println("WIFI");
    digitalWrite(LED, HIGH);
    //delay(50);
    //digitalWrite(LED, LOW);
    //delay(50);
    //WiFiClient client = server.available();   // listen for incoming clients

    WiFiClient client = server.available();
    if (!client) {
      return;
    }

    Serial.println("new client");
    while (!client.available())
    {
      delay(20);
    }
    String req = client.readStringUntil('\r');
    Serial.println(req);
    //client.flush();


    WiFiswitching(req , "/S1on", "/S1off", Relay1);
    WiFiswitching(req , "/S2on", "/S2off", Relay2);
    WiFiswitching(req , "/S3on", "/S3off", Relay3);
    WiFiswitching(req , "/S4on", "/S4off", Relay4);
    WiFiswitching(req , "/S5on", "/S5off", Relay5);
    WiFiswitching(req , "/S6on", "/S6off", Relay6);
    WiFiswitching(req , "/S7on", "/S7off", Relay7);
    WiFiswitching(req , "/S8on", "/S8off", Relay8);

    digitalWrite(LED, LOW);


  }

  if (flagFB == 0) {
    int  MAlarm = digitalRead(MegnaticSwitch);
    int  FAlarm = analogRead(flameSensor);
    int  GAlarm = analogRead(GasSwitch);

    flagWifi = 1;
    flagBT = 1;
    //Serial.println("FB");
    digitalWrite(LED, HIGH);
    //delay(30);
    //digitalWrite(LED, LOW);
    //delay(30);
    

    if (MAlarm == 1){     // && flagMagnaticSwitch == 0) {
      Firebase.set(firebaseData, "MAlarm", 1);
      //flagMagnaticSwitch = 1;
    }
    else if (MAlarm == 0) {
      Firebase.set(firebaseData, "MAlarm", 0);
      //flagMagnaticSwitch = 0;
    }

    if (FAlarm > 10 && FAlarm < 4000  ) {
      Firebase.set(firebaseData, "FAlarm", 1);
    }
    else if (FAlarm >= 4000) {
      Firebase.set(firebaseData, "FAlarm", 0);
    }
    if (GAlarm <= 2200) {
      Firebase.set(firebaseData, "GAlarm", 0);
    }
    else if (GAlarm > 2200) {
      Firebase.set(firebaseData, "GAlarm", 1);
    }


    if (WiFi.status() == WL_CONNECTED) {


      FireBase_Switching(firebaseData,"Switch1",Relay1 );
      FireBase_Switching(firebaseData,"Switch2",Relay2 );
      FireBase_Switching(firebaseData,"Switch3",Relay3 );
      FireBase_Switching(firebaseData,"Switch4",Relay4 );
      FireBase_Switching(firebaseData,"Switch5",Relay5 );
      FireBase_Switching(firebaseData,"Switch6",Relay6 );
      FireBase_Switching(firebaseData,"Switch7",Relay7 );
      FireBase_Switching(firebaseData,"Switch8",Relay8 );
      
      /*
      Firebase.getString(firebaseData, "Switch2"); //Reading the value of the varialble Status from the firebase
      rel2 = printResult(firebaseData);
      switchoutput(firebaseData.stringData().toInt(), Relay2);

      Firebase.getString(firebaseData, "Switch3"); //Reading the value of the varialble Status from the firebase
      rel3 = printResult(firebaseData);
      switchoutput(rel3, Relay3);
      Firebase.getString(firebaseData, "Switch4"); //Reading the value of the varialble Status from the firebase
      rel4 = printResult(firebaseData);
      switchoutput(rel4, Relay4);
      Firebase.getString(firebaseData, "Switch5"); //Reading the value of the varialble Status from the firebase
      rel5 = printResult(firebaseData);
      switchoutput(rel5, Relay5);
      Firebase.getString(firebaseData, "Switch6"); //Reading the value of the varialble Status from the firebase
      rel6 = printResult(firebaseData);
      switchoutput(rel6, Relay6);
      Firebase.getString(firebaseData, "Switch7"); //Reading the value of the varialble Status from the firebase
      rel7 = printResult(firebaseData);
      switchoutput(rel7, Relay7);
      Firebase.getString(firebaseData, "Switch8"); //Reading the value of the varialble Status from the firebase
      rel8 = printResult(firebaseData);
      switchoutput(rel8, Relay8);

      */

    }
    
  }
  digitalWrite(LED, LOW);
}

void Wifisetup()
{
  WiFiManager wifiManager;
  wifiManager.resetSettings();

  Serial.println("Conecting.....");
  wifiManager.autoConnect("SmartHome_Wi-Fi_Config", "12345678");
  //Serial.println("connected.");

  while (WiFi.status() != WL_CONNECTED ) {
    Serial.print(".");
    delay(500);
    Serial.println("failed to connect, we should reset as see if it connects");
    wifiManager.autoConnect("SmartHome_Wi-Fi_Config", "12345678");
  }
  //Serial.println();
  server.begin();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());

  //Serial.println("Server Begin!");

}


void FireBasesetup() {


  WiFiManager wifiManager;
  wifiManager.resetSettings();

  Serial.println("Conecting.....");
  wifiManager.autoConnect("SmartHome_FireBase_Config", "12345678");
  //Serial.println("connected.");

  while (WiFi.status() != WL_CONNECTED ) {
    Serial.print(".");
    delay(500);
    Serial.println("failed to connect, we should reset as see if it connects");
    wifiManager.autoConnect("SmartHome_FireBase_Config", "12345678");
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)
  //Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //Firebase.setReadTimeout(firebaseData, 1000);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  //Firebase.setwriteSizeLimit(firebaseData, "tiny");


  //Here the varialbe needs to be the one which is used in our Firebase and MIT App Inventor
  //String path = "/Test/Data";

  Firebase.set(firebaseData, "Switch1", 0);
  Firebase.set(firebaseData, "Switch2", 0);
  Firebase.set(firebaseData, "Switch3", 0);
  Firebase.set(firebaseData, "Switch4", 0);
  Firebase.set(firebaseData, "Switch5", 0);
  Firebase.set(firebaseData, "Switch6", 0);
  Firebase.set(firebaseData, "Switch7", 0);
  Firebase.set(firebaseData, "Switch8", 0);
  Firebase.set(firebaseData, "MAlarm", 0);
  Firebase.set(firebaseData, "FAlarm", 0);
  Firebase.set(firebaseData, "GAlarm", 0);


}


void BTswitching(String readData, String dataStringON, String dataStringOFF , int relay) {
  delay(10);
  if (readData == dataStringON) {
    digitalWrite(relay, LOW);
  }

  else if (readData == dataStringOFF) {
    digitalWrite(relay, HIGH);
  }
}



void WiFiswitching(String req , String onData, String offData, int relay   ) {
  if (req.indexOf(onData) != -1)
  {
    digitalWrite(relay, LOW);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("" + onData + "");
    client.println("</html>");
    client.stop();
    delay(1);
    //Serial.println("ON");
  }
  else if (req.indexOf(offData) != -1)
  {
    digitalWrite(relay, HIGH);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("" + offData + "");
    client.println("</html>");
    client.stop();
    delay(1);
    //Serial.println("OFF");
  }
}


void FireBase_Switching(FirebaseData & data, String SwitchTag, int Relay ) {

  Firebase.getString(data, SwitchTag );//Reading the value of the varialble Status from the firebase
  int relayValue = data.stringData().toInt();
  if (relayValue == 1)
  {
    digitalWrite(Relay, LOW);
    //Serial.println(" ON");
  }
  if (relayValue == 0)
  {
    digitalWrite(Relay, HIGH);
    //Serial.println(" OFF");
  }
}


int printResult(FirebaseData & data)
{

  if (data.dataType() == "string") {
    //Serial.println(data.stringData());
    return data.stringData().toInt();
  }
  else
  {
    //Serial.println(data.payload());
    return data.stringData().toInt();
  }
}

void DoorOpenNotification() {
  int mF = digitalRead(MegnaticSwitch);
  //Serial.println("Theft detected: "+String(mF));
  //delay(2000);
  if (mF == 1 ) {
    //
    voice.say(sp2_DANGER);
    voice.say(sp4_DANGER);
    //delay(100);
    voice.say(sp2_INTRUDER);
    //delay(100);
    voice.say(sp2_CALL);
    voice.say(sp4_SECURITY);
  }
  
}


void flamewarning() {
  int fF = analogRead(flameSensor);
  //Serial.println("flame detected: "+String(fF));
  //delay(2000);
  if (fF > 10 && fF < 4000  ) {
    //
    voice.say(sp2_DANGER);
    voice.say(sp4_DANGER);
    voice.say(sp2_FIRE);
    voice.say(sp2_ALERT);
    //delay(100);

  }
}


void Gaswarning() {
  int gF = analogRead(GasSwitch);
  //Serial.println("Gas detected: "+String(gF));
  //delay(2000);
  if (gF > 2220) {
    //Serial.println("Gas detected > 2220");
    voice.say(sp2_DANGER);
    voice.say(sp4_DANGER);
    //delay(100);
    voice.say(sp3_SMOKE);
    //delay(100);
    voice.say(sp2_ALERT);
    //delay(100);
  }
}
