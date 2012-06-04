#include <FF_XCVR.h>

void setup() {
  Serial.begin(9600);
  XCVR_init();
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
}


void loop()
{
  delay(1000);
  String transmission = "Lets transmit something a little bit longer. This is a test of the Firefly board. Callsign: N4LNX";
  for(byte i = transmission.length(); i; i--)
  {
    XCVR_send_char(transmission[i-1]);
   }
   XCVR_send_char(' ');
}
