#include <WiFi.h>
#include <M5StickCPlus.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

const int buttonA = 37;  //Button-B =39, Button-A = 37
const int buttonB = 39;

int connectweb = 0;
int last_valueA = 0;
int last_valueB = 0;
int cur_valueA = 0;
int cur_valueB = 0;


float temp = 0.00;


char MQTTServerip[] = "192.168.23.1";
String HTTPServerip = "192.168.23.1";

WiFiClient espclient;
PubSubClient client(espclient);

HTTPClient http;

const char* ssid = "";
const char* password = "";
const char* devname = "";

int sendtempdata = 0;
char payloaddata[40]="";

void setup() {
  // init lcd
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(40, 15);
  M5.Lcd.println("Temperature MQTT");
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
}

void loop() {
  M5.IMU.getTempData(&temp);
  cur_valueA = digitalRead(buttonA);// read the value of BUTTON_A
  cur_valueB = digitalRead(buttonB);// read the value of BUTTON_B
  if(cur_valueA==0){
    Serial.println("Button A Status: pressed");
    Serial.println("Connecting to Wifi-Network");
    connectToNetwork();
  }
  else{
    Serial.println("Button A Status: released");
  }
  if(cur_valueB==0){
   sendtempdata=1;
  }
  else{
    Serial.println("Button B Status: released");
  }
  Serial.print("Temperature : ");
  Serial.println(temp);
  M5.Lcd.setCursor(30, 95);
  M5.Lcd.printf("Temperature : %.2f C", temp);
  if(connectweb == 1)
  {
    Serial.print("Publishing to test, helloworldPublishing");
    client.publish("test","");
  }
  if(sendtempdata == 1)
  {
    char buffer[40];
    sprintf(buffer, "%.2f", temp);
    Serial.print("Publishing to test/temp1");
    client.publish("test/temp1",buffer);
    if (temp >=60.00){
      client.publish("test/sens1","Temperature Alert!");
    }
    else if (temp < 60.00){
      client.publish("test/sens1","Normal!");
    }
  }
  if (!client.connected() && connectweb == 1) {
    char strbuf[40];
    sprintf(strbuf,"%s",devname);
    if (client.connect(strbuf)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("test","");
      // ... and resubscribe
      client.subscribe("teston");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      //Wait 3 seconds before retrying
      delay(3000);
    }
  }
  else {
    client.loop();
  }
  
  delay(300);
}

void connectToNetwork() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  connectweb = 1;
  Serial.println("Connected to network");
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  client.setServer(MQTTServerip, 1883);
  client.setCallback(callback);
  char strbuf[40];
  sprintf(strbuf,"%s",devname);
  client.connect(strbuf);
  client.subscribe("teston");
  client.publish("test","hello world");
  M5.Lcd.setCursor(30,40,2); 
  M5.Lcd.print(WiFi.localIP());// display the status
  sendhttpget();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    payloaddata[i]=payload[i];
  }
  M5.Lcd.setCursor(30, 115);
  M5.Lcd.printf("Payload : %s", payloaddata);
  Serial.println();
}

void sendhttpget(){
  String serverAPI = String("http://" + HTTPServerip + ":8000/devices");
  String serverPath = String(serverAPI + "/" + devname);
  http.begin(serverPath.c_str());
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
}
