#include "arduino_secrets.h"
// TODO
// Pouziti wifi manager k nastaveni SSDI a Password Wifiny.

/* 
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/ac57ef01-9b95-4ee3-8936-45ca18d52a0b 

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float room1Temp;
  float room2Temp;
  float targetTemp1;
  float targetTemp2;
  bool antiFreeze;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"

struct keyvalue_pair_t {
  char    key[5];
  int     value;
};
const byte numChars = 32; // buffer size for serial reader
char receivedChars[numChars]; // input buffer for receiving from serial input
char tempChars[numChars];  

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  room1Temp = 0;
  room2Temp = 0;
}

void loop() {
  ArduinoCloud.update();
  // Your code here 
  boolean newData = false; 

    newData = recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseKeyValuePair() replaces the commas with \0
       
        keyvalue_pair_t kv; 

        kv = parseKeyValuePair();
        processParsedData(kv);
        newData = false;
    }
}

void onTargetTemp1Change()  {
  // Add your code here to act upon TargetTemp1 change
  Serial.printf("<TT1, %d>", (int)(targetTemp1 * 100));
  Serial.println("");
}

void onTargetTemp2Change()  {
  // Add your code here to act upon TargetTemp2 change
  Serial.printf("<TT2, %d>", (int)(targetTemp2 * 100));
  Serial.println("");
}

void onAntiFreezeChange()  {
  // Add your code here to act upon AntiFreeze change
  Serial.printf("<AF, %d>", (int)antiFreeze);
  Serial.println("");
}

bool recvWithStartEndMarkers() {
    bool newData = false;
    static boolean recvInProgress = false;
    static byte indx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[indx] = rc;
                indx++;
                if (indx >= numChars) {
                    indx = numChars - 1;
                }
            }
            else {
                receivedChars[indx] = '\0'; // terminate the string
                recvInProgress = false;
                indx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
    return newData;
}

keyvalue_pair_t parseKeyValuePair() {      // split the data into its parts
    char *strtokIndx; // this is used by strtok() as an index
    keyvalue_pair_t kv;

    strtokIndx = strtok(tempChars,",");      // get the first part - the tag
    strcpy(kv.key, strtokIndx); // copy it to KEY 
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    kv.value = atoi(strtokIndx);     // convert this part to an integer

    return kv;
}

void processParsedData(keyvalue_pair_t &kv) {
  // take appropriate actions according to the tag
  if (strcmp(kv.key, "T1") == 0){
    room1Temp = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "T2") == 0) {
    room2Temp = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "TT1") == 0){
    targetTemp1 = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "TT2") == 0) {
    targetTemp2 = kv.value / 100.0;
  }
  else if (strcmp(kv.key, "AF") == 0) {
    antiFreeze = (bool)kv.value;
  }
  else {
    // do some error handling here. Ingoring it now.
  }
}