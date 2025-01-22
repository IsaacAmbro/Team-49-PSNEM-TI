const int Out1 = 3; // Outout pin to control the relay connected to the voltage regulator
const int Out2 = 4; // Outout pin to control the relay connected to the power supply
const int Out3 = 5; // Outout pin to control the relay connected to the LDO

void setup() {

  Serial.begin(9600);  

  pinMode(Out1, OUTPUT); //setting pin as output
  pinMode(Out2, OUTPUT); //setting pin as output
  pinMode(Out3, OUTPUT); //setting pin as output

}


void loop() {

  if (Serial.available() > 0) {

    String command = Serial.readStringUntil('\n'); 


    if (command == "ON1") { //turning on votage regulator relay

      digitalWrite(Out1, HIGH); // maximum DUE voltage output is 3.3, so setting this to HIGH

    } 
    
    else if (command == "ON2") { //turning on power supply relay

      digitalWrite(Out2, HIGH);

    } 

    else if (command == "ON3") { // turning on LDO relay

      digitalWrite(Out3, HIGH);

    } 
    else if (command == "OFF") { // turning off all relays

      digitalWrite(Out1, LOW);
      digitalWrite(Out2, LOW);
      digitalWrite(Out3, LOW); 

    }

  }

}
