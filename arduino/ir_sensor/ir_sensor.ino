                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  // Library Used 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Objects Creation
Servo servo1;
LiquidCrystal_I2C lcd(0x27,16,2);

// Inputs
#define ir_slot1 2
#define ir_slot2 3
#define ir_in 5

// Outputs
#define led1 8
#define led2 9
#define in_servo 13

// Slots available counter
int count_slots = 0;

void setup(){
  // Serial begin
  Serial.begin(9600);
  
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
}

 
void loop(){
  
  // Read Slots Statut if it is not available 0 else 1
  int ir1_statut = ir_statut(ir_slot1);
  int ir2_statut = ir_statut(ir_slot2);
  int ir_in_statut = ir_statut(ir_in);

  // Enter Vehicule verification
  if ((ir_in_statut==1)&&(count_slots>0)){
      servo1.write(180);
      lcd.setCursor(0,1);
      lcd.print("Access Accepted");
      delay(3000);
      lcd.print("                       ");
    }else if ((ir_in_statut==1)&&(count_slots == 0)){
      servo1.write(0);
      lcd.setCursor(0,1);
      lcd.print("No Slot Available");
      delay(3000);
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

  // Slots Flags LEDs
  // Slot 1 flag
  if (ir1_statut==1){
      digitalWrite(led1, HIGH);
    }else{
      digitalWrite(led1, LOW);
      }
  // Slot 2 flag
  if (ir2_statut==1){
      digitalWrite(led2, HIGH);
    }else{
      digitalWrite(led2, LOW);
      }

  delay(500);
}


// IR Statu function
int ir_statut(int ir_pin){
    if (digitalRead(ir_pin) == 0){
        return 1;
      }return 0;
}
