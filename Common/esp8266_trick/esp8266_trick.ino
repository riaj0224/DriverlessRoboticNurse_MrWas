#include <SoftwareSerial.h>
SoftwareSerial ESP(3, 2); // RX | TX

void setup()
  {  Serial.begin(9600);
     ESP.begin(9600);
  }

void loop()
  {
     // Repetir lo recibido por el ESP8266 hacia el monitor serial
     if (ESP.available())
         { char c = ESP.read() ;
           Serial.print(c);
         }

     // Repetir lo recibido por el monitor serial hacia el ESP8266
     if (Serial.available())
         {  char c = Serial.read();
            ESP.print(c);
         }
   }
