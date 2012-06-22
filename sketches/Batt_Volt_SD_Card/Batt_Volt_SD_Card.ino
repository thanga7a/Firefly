//Twyman Clements
//Written for Firefly

//SD Card library call out
#include <SD.h> 
int CS_pin = 10;                   //Chip Select pin for SPI communciations. When this pin is pulled high the microcontroller writes to the SD card.  
const int ledPin = 3;              //LED pin, visual to let you know it's working
unsigned long time;                //Since this can become a very long # it becomes and unsigned long
float refresh_rate = 0.0;          //Initialize the refresh rate. This will be reset to what you put in the COMMANDS.txt file
long id = 1;                       //Initialize ID 
boolean ledStatus = LOW;           //Switch the LED on and off 
String ledStat;                    //String variable which states "ON" and "OFF" 

void setup() {
  Serial.begin(9600);                          //Serial communciations initialization 
  Serial.println("Initializing Card");       
  
  pinMode(CS_pin, OUTPUT);                     //Set the pins
  pinMode(ledPin, OUTPUT); 
  
  if(!SD.begin(CS_pin)) {                      //Make sure you can talk to the SD card, if not say failure 
    Serial.println("Card Failure"); 
    return; 
  }
  
  Serial.println("Card Ready");                //If it talks to it say so
  
  File commandFile = SD.open("COMMANDS.txt");  //Opends the COMMANDS.txt file
  if(commandFile) {                            //This if statement goes through and reads the text you have written in the COMMANDS.txt
    Serial.println("Reading Command File");    //There is a good explaination of this loop if you search for "SD card Arudino" on youtube it's like a 30 minute video 
    float decade = pow(10, (commandFile.available() -1)); 
    while(commandFile.available()) {
      float temp = (commandFile.read() - '0'); 
      refresh_rate = temp*decade+refresh_rate; 
      decade = decade/10;
    }
    
    Serial.print("Refresh Rate = ");            //Write the refresh rate to the serial dialog box when you open it  
    Serial.print(refresh_rate); 
    Serial.println("ms");
    commandFile.close(); 
  }
  
  else{                                          //If it doesn't work it lets you know
  Serial.println("Could not read command file.");               
  return;
}

File logFile = SD.open("LOG.txt", FILE_WRITE);   //Creates the LOG file and says it can write to it 
if(logFile) {
  logFile.println(", , , ,");                    //Writes ,,,, everytime you restart the program
  String header = ("ID, Time, LED Status, Batt Voltage"); //Header text 
  logFile.println(header); 
  logFile.close();                               //After you open the file, write something you must then close it to do anything else
  Serial.println(header);                        //Prints to serial dialog box
}

else{                                            //Let's you know if it doesn't work
  Serial.println("Coudldn't open log file"); 
}
}

void loop() {
  int batt_volt = analogRead(0);                  //Reads the battery voltage 
  time = millis();                                //Starts your timing 
  digitalWrite(ledPin, ledStatus);                //Toggles the LED based on what ledStatus is 
  
  if(ledStatus == LOW) {                          //If statement to say if the LED is on or off
    ledStat = "ON"; 
  }
  
  else {
    ledStat = "OFF"; 
  }
  
  String dataString = String(id) + " " + String(time) + " " +String(ledStat) + " " + String(batt_volt);    
  //Takes all the data and puts into a variable string called dataString  
  
  File logFile = SD.open("LOG.txt", FILE_WRITE);                  //Opends the text file again
  if (logFile) {                                                  //If it opends correctly writes the dataString
    logFile.println(dataString);
    logFile.close();                                              //Again you have to close them
    Serial.println(dataString);                                   //Prints data if Serial dialog box is open
  }
  else {
    Serial.println("Couldn't open log file");                     //Let's you know if it doesn't work
  }
  
  id++;                                                           //Increment your id #
  ledStatus = !ledStatus;                                         //Since ledStatus is a boolean we reverse it from what it was here. Thus the LED toggle
  
  delay(refresh_rate);                                            //What the refresh_rate, which is from the COMMANDS.txt file 
}
