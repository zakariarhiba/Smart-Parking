                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // Library Used 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoMqttClient.h>

// Objects Creation
Servo servo1;
LiquidCrystal_I2C lcd(0x27,16,2);

// Inputs
#define ir_slot1 2
#define ir_slot2 3
#define ir_in 5
#define temp_pin A0
#define ldr_pin A1

// Outputs
#define led1 8
#define led2 9
#define in_servo 13

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Slots available counter
int count_slots = 0;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

EthernetClient ethernetclient;
MqttClient mqttClient(ethernetclient);


const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic[]  = "CoreElectronics/temp";
const char topic2[]  = "CoreElectronics/sun";
const char topic_in1[]  = "CoreElectronics/reserve";
const char topic_in2[]  = "CoreElectronics/unreserve";
const char topic_slot_statut[] = "CoreElectronics/slot_statut";

const long interval = 1000;
unsigned long previousMillis = 0;

void setup(){
  // Serial begin
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

   if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  
  // LCD initalization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Welcome to Smart ");
  lcd.setCursor(5,1);
  lcd.print("Parking");
  delay(2000);
  lcd.clear();

  // Initialize IN OUT pins
  pinMode(ir_slot1, INPUT);
  pinMode(ir_slot2, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(in_servo, OUTPUT);
  
  // Initialization the servo motors
  servo1.attach(in_servo);
  servo1.write(0);


  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  // subscribe to a topic
  mqttClient.subscribe(topic_in1);
  mqttClient.subscribe(topic_in2);


  Serial.print("Waiting messages from the application : ");
  Serial.println();
}

 
void loop(){
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

   // Read Slots Statut if it is not available 0 else 1
  int ir1_statut = ir_statut(ir_slot1);
  int ir2_statut = ir_statut(ir_slot2);
  int ir_in_statut = ir_statut(ir_in);

  // avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    // Send Temperature Sensor Value
    Serial.print("Send the temp : ");
    Serial.println(get_temperature(temp_pin));
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(get_temperature(temp_pin));
    mqttClient.endMessage();

    // Send LDR Sun value
    int ldr_val = get_ldr_statut(ldr_pin);
    if (ldr_val > 600){
        Serial.print("Send the sun statut : ");
        Serial.println("hot");
        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic2);
        mqttClient.print("hot");
        mqttClient.endMessage();
      }else{
        Serial.print("Send the sun statut : ");
        Serial.println("hot");
        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic2);
        mqttClient.print("normal");
        mqttClient.endMessage();
        }


    // sending the slot statut
     // Slot 1 flag
    if (ir1_statut==0){
        digitalWrite(led1, HIGH);
        mqttClient.beginMessage(topic_slot_statut);
        mqttClient.print("slot1_1");
        mqttClient.endMessage();
        Serial.println("Slot1 send on");
      }else{
        digitalWrite(led1, LOW);
        mqttClient.beginMessage(topic_slot_statut);
        mqttClient.print("slot1_0");
        mqttClient.endMessage();
        Serial.println("Slot1 send off");
        }
    // Slot 2 flag
    if (ir2_statut==0){
        digitalWrite(led2, HIGH);
        mqttClient.beginMessage(topic_slot_statut);
        mqttClient.print("slot2_1");
        mqttClient.endMessage();
        Serial.println("Slot 2 send on");
      }else{
        digitalWrite(led2, LOW);
        mqttClient.beginMessage(topic_slot_statut);
        mqttClient.print("slot2_0");
        mqttClient.endMessage();
        Serial.println("Slot2 send off");
        }

    mqttClient.beginMessage(topic_slot_statut);
    mqttClient.print("slot3_0");
    mqttClient.endMessage();

    Serial.println();


      // Enter Vehicule verification
  if ((ir_in_statut==1)&&(count_slots>0)){
      servo1.write(180);
      lcd.setCursor(0,1);
      lcd.print("Access Accepted");
      lcd.print("                       ");
    }else if ((ir_in_statut==1)&&(count_slots == 0)){
      servo1.write(0);
      lcd.setCursor(0,1);
      lcd.print("No Slot Available");
      lcd.setCursor(0,1);
      lcd.print("                       ");
      }else{
        servo1.write(0);
        lcd.setCursor(0,1);
        lcd.print("No car Detected");
        }

  // Set the counter slots availble
  if((ir1_statut == 1) && (ir2_statut == 1)){
    count_slots  = 0;
    lcd.setCursor(0,0);
    lcd.print("Slots Av : 0");
    }else if (((ir1_statut == 0) && (ir2_statut == 1)) || ((ir1_statut == 1) && (ir2_statut == 0))){
      count_slots = 1;
      lcd.setCursor(0,0);
      lcd.print("Slots Av : 1");
      }else if ((ir1_statut == 0) && (ir2_statut == 0)) {
        count_slots = 2;
        lcd.setCursor(0,0);
        lcd.print("Slots Av : 2");
        }Serial.print("Available Slots : ");Serial.println(count_slots);
  }
  
 
}


// on MQTT Function
void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
  Serial.println();
}


// get the temperature value function
String get_temperature(int temp_pin){
  int value = analogRead(temp_pin);
  float mv = ( value/1024.0)*5000;
  float cel = mv/10;
  String temp = String(cel, 2);
  return temp;
  }

// IR Statu function
int ir_statut(int ir_pin){
    if (digitalRead(ir_pin) == 0){
        return 1;
      }return 0;
}

// LDR get value function
int get_ldr_statut(int ldr_pin){
    int value  = analogRead(ldr_pin);
    return value;
}
    
