/* Desginathon Braille keyboard  */

// Libraries
#include <SoftwareSerial.h>

//Constants for ports:
const int backspace = A0;
const int topButton = A1;
const int middleButton = A2;
const int bottomButton = A3;
const int space = A4; 
const int bluetoothTx = 10;
const int bluetoothRx = 9;

// fixed threshold for force resistive sensor
const int threshold = 200;

// ASCII key is in charKeys[brailleInput - 11
const int charKeys[] = {
  34 ,0  ,0  ,0  ,0  ,44 ,63 ,0  ,0  ,105,   //10
  0  ,106,119,59 ,34 ,33 ,0  ,115,0  ,116,   //20
  0  ,97 ,0  ,101,0  ,99 ,0  ,100,0  ,107,   //30
  117,111,122,109,120,110,121,98 ,0  ,104,   //40
  0  ,102,0  ,103,0  ,108,118,114,0  ,112,   //50
  0,  113
  };
  
// ASCII key for int is in intKeys[brailleInput - 20]
const int intKeys[] = {
  57 ,0  ,48 ,0  ,0  ,0  ,0  ,0  ,0  ,0,     //10
  0  ,0  ,49 ,0  ,53 ,0  ,51 ,0  ,52 ,0,     //20
  0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,50 ,0,     //30
  56 ,0  ,54 ,0  ,55
  };

//Variable to check for caps
// zero corresponds to small letters, 1 corresponds to all caps, 2 corresponds to one capital letter
int capsCount = 0;

SoftwareSerial btSerial(bluetoothTx, bluetoothRx);

void setup(){
  // Initialisation
  Serial.println("Initialised system");
  Serial.begin(9600);       //Begin serial communication
  btSerial.begin(115200);
  
}


void loop(){
  if (analogRead(topButton) > threshold || analogRead(middleButton) > threshold || analogRead(bottomButton) > threshold || analogRead(space) > threshold) {
    calInput();
  }
  else if (analogRead(backspace) > threshold) {
      while (analogRead(backspace) > threshold) {}                       // while loops ensures that backspace button is only pressed once
      Serial.println("Keyboard pressing backspace");
      btSerial.write(8);
      delay(200);
  }
}


// Get converted decimal form of inputs and call the respective methods
void calInput() {
  // 1   4
  // 2   5
  // 3   6   positions
  int inputValue = calInputValue();
  switch(inputValue) {  
    case -1:
      Serial.println("returning backspace");
      while (analogRead(backspace > threshold)) {}
      return;
      break; 
    case 0:     //case 0 for space bar
      Serial.println("Keyboard pressing space");
      if (capsCount == 2 || capsCount == 1) {
        capsCount = 0;
      }
      btSerial.write(32);
      delay(200);
      break;
    case 1:     //case 1 for capital letters
      if (capsCount < 2) {
        capsCount += 1;
      }
      Serial.println(capsCount);
      break;
    case 15:     // case 15 for numeral inputs
      while(true) {
        if (analogRead(topButton) > threshold || analogRead(middleButton) > threshold || analogRead(bottomButton) > threshold || analogRead(space) > threshold) {
          inputValue = calInputValue();
          break;
        }          
      }
      numInput(inputValue);
      break;
    default:    // default for small letter input
      charInput(inputValue);
      break;
  }
}


// Computes and converts input into decimal values
//
int calInputValue() {
  int clicked1[] = {0,0,0,1}; 
  int clicked2[] = {0,0,0,1};
  int tempValue1 = 0;
  int tempValue2 = 0; 
  while (analogRead(topButton) > threshold || analogRead(middleButton) > threshold || analogRead(bottomButton) > threshold || analogRead(space) > threshold) {
    if (analogRead(topButton) > threshold) {
      clicked1[0] = 1;
      }
    if (analogRead(middleButton) > threshold) {
      clicked1[1] = 1;
      }
    if (analogRead(bottomButton) > threshold) {
      clicked1[2] = 1;
      }
    if (analogRead(space) > threshold) {
      clicked1[3] = 0;
      } 
    if (analogRead(backspace) > threshold) {
       Serial.println("backspace entered");
       return -1;         
       }
  }
  tempValue1 = (32 * clicked1[0] + 16 * clicked1[1] + 8 * clicked1[2]) * clicked1[3];
  Serial.print("temp value 1 is ");
  Serial.println(tempValue1);
  delay(200);
  
  while (true) {
    if (analogRead(backspace) > threshold) {
      return -1;
      Serial.println("backspace entered");
     }
    if (analogRead(topButton) > threshold || analogRead(middleButton) > threshold || analogRead(bottomButton) > threshold || analogRead(space) > threshold) {
      while (analogRead(topButton) > threshold || analogRead(middleButton) > threshold || analogRead(bottomButton) > threshold || analogRead(space) > threshold) {
        if (analogRead(topButton) > threshold) {
          clicked2[0] = 1;
         }
        if (analogRead(middleButton) > threshold) {
          clicked2[1] = 1;
         }
        if (analogRead(bottomButton) > threshold) {
          clicked2[2] = 1;
        }
        if (analogRead(space) > threshold) {
          clicked2[3] = 0;
        }
      }  
      tempValue2 = (4 * clicked2[0] + 2 * clicked2[1] + 1 * clicked2[2]) * clicked2[3];
      Serial.print("temp value 2 is ");
      Serial.println(tempValue2);
      break;   
    }
  }
  
  return tempValue1 + tempValue2;
}


void charInput(int input) {
  if (input <= 10) {
    return;
  }
  switch(capsCount) {
    int output;
    case 0:
      output = charKeys[input - 11];
      if (output != 0) {
      Serial.print("char input is : ");
      Serial.println(char(output));
      btSerial.write(output); 
      delay(200);
      break;
    case 1: 
      output = charKeys[input-11] - 32;
      if (output <= 90 && output >= 65) {
      Serial.print("capital char input is : ");
      Serial.println(char(output));
      btSerial.write(output); 
      delay(200);
      capsCount = 0;
      }
      break;
    case 2:
      output = charKeys[input-11] - 32;
      if (output <= 90 && output >= 65) {
      Serial.print("capital char input is : ");
      Serial.println(char(output));
      btSerial.write(output);
      delay(200);
      } else {
        capsCount == 0;
        }
      break;    
      }
  }
  // use charKeys[] to convert into into ASCII and send to device
}


// This method converts input into ASCII for numeral inputs using the int hash table
//
void numInput(int input) {
  if (input <= 19) {
    return;
  }
  int output = intKeys[input - 20];
  if (output != 0) {
  Serial.print("num input is : ");
  Serial.println(char(output));
  btSerial.write(output);
  delay(200);
  }
}


