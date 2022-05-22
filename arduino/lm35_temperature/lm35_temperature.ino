

void setup()
{
  Serial.begin(9600);
}
void loop()
{
  Serial.print("TEMPRATURE = ");
  Serial.print(get_temperature(A0));
  Serial.print("*C");
  Serial.println();
  delay(1000);
}


String get_temperature(int temp_pin){
  int val = analogRead(temp_pin);
  float mv = ( val/1024.0)*5000;
  float cel = mv/10;
  String temp = String(cel, 2);
  return temp;
  }
