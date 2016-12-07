//#########################################################
//###                                                   ###
//###            Saab ParrotStar BT Control             ###
//###                   Version 1.0                     ###
//###                    Rob Lester                     ###
//###             Hardware: Arduino MEGA                ###
//###                                                   ###
//#########################################################

#define box Serial1
#include <EEPROM.h>

// Operational Values
long millis_held;
unsigned long firstTime = 0;
int previous = 0;
int current = 0;
int executed = 0;

// The analog ranges need to be determined. When a button is pressed, 
// the value is printed to the console.

// Below ranges based on a 1K resistor pulling down the button value line.

// Button 1 analog range
int btn1low = 220;
int btn1high = 235;

// Button 2 analog range
int btn2low = 55;
int btn2high = 65;

// Button 3 analog range
int btn3low = 370;
int btn3high = 380;

// Button Press Debounce/Longpress settings
int longpress = 1000; //How long should a long press be
int presstimeout = 75; //Set the firsttime value after this long of a press.

// Comms - don't change these
int conRes = 128;
int stopLoop = 0;
int lastComm = 0;

// PINS
int greenPin = 5;
int redPin = 4;
int buttonPin = 0; // Analog Pin for Control Buttons.
int auxPin1 = 7; // Control an aux relay if included in circuit.
int auxPin2 = 8; // Control an aux relay if included in circuit. 2 seems useful.
int ledPin = 13; // On Board LED

int auxPinMem1 = 0; // Aux 1 setting byte on EEPROM
int auxPinMem2 = 1; // Aux 2 setting byte on EEPROM

// Context Flags
int menu = 0;
int resetConfirm = 0;
int inCall = 0;

void setup() { // Initial Boot Sequence
  
	Serial.begin(9600);
	Serial.println("Ready.");
	pinMode(greenPin, OUTPUT);
	pinMode(redPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	box.begin(9600);
	digitalWrite(auxPin1, EEPROM.read(auxPinMem1));
	digitalWrite(auxPin2, EEPROM.read(auxPinMem2));
	
}

void loop() { // 
  
	readAnalogController(); //Listen for analog button commands.
	
	// Tell brain Box that we are waiting.
	if (!stopLoop) {
		box.write(160);
		box.write(128);
		delay(250);
		int bl = digitalRead(greenPin);
		digitalWrite(greenPin, !bl);
		//int dp = digitalRead(ledPin);
		//digitalWrite(ledPin, !dp);
	}
  
}

void serialEvent1() { // Receive commands from the brain box
	
	if (stopLoop == 0) {
		stopLoop = 1; // Stop the waiting loop, because the box has come online.
		digitalWrite(ledPin, LOW);
	}
	
	int v = box.read(); // Read the command from the box.
	
	// Process specific commands from box
	
	if (v == 34) { // At rest
		digitalWrite(greenPin, LOW);
	}
	if (v == 17) { // Phone is engaged..?
		digitalWrite(greenPin, HIGH);
		digitalWrite(redPin, LOW);
	}
	
	// End specific commands
	
	// HANDSHAKE PROCESSING CODE BELOW
	
	// Part of the handshake
	if (lastComm == 160 && v == 1) {
		box.write(160);
		box.write(129);
		box.write(2);
	}
	
	// Part of the handshake -- Complete
	if (lastComm == 160 && v == 4) {
		box.write(160);
		box.write(132);
	}
	
	// Part of the handshake
	if (lastComm == 160 && conRes == 128) {
		box.write(160);
		box.write(131);
		conRes = 131;
	}
	
	// Print non-handshake/status messages to the console.
	if (v != 4 && v != 160 && v != 1) {
		Serial.print("Box: ");
		Serial.println(v, DEC);
	}
	
	lastComm = v; // Set our last command
}

void serialEvent() {
	
	// The below commands are strictly for sending 
	// commands to box via the command line.
	
	/*while(Serial.available() > 0) {
		int in = Serial.read();
		c = c + in;
	}*/

	int c = Serial.read();

	if (c == 56 || c == 55 || c == 52) {
		box.write(160);
		box.write(130);
		if (c == 56) box.write(8);
		if (c == 55) box.write(248);
		if (c == 52) {
			box.write(4);
			delay(250);
			box.write(160);
			box.write(130);
			box.write(0);
		}
	} else {
		box.write(160);
		box.write(130);
		box.write(c);
		delay(250);
		box.write(160);
		box.write(130);
		box.write(0);
	}
	Serial.print("Rec: ");
	Serial.println(c);
}

void readAnalogController() {
	current = analogRead(buttonPin);
	//longpress = analogRead(1); // Use if there's a pot to set longpress time.
	//if (current > 0) Serial.println(current); // Use this line to determine actual analog values initially.
	
	if (previous > 0 && current == 0) {
		executed = 0;
	}
	
	if (previous > 0 && current == 0 && millis_held < longpress) {
		int btn = decodeButton(previous);
		executeButton(btn, 0);
	}
	
	if (current > 0 && previous == 0 && (millis() - firstTime) > presstimeout) {
		firstTime = millis();
	}
	
	if (current > 0) millis_held = (millis() - firstTime);
	else {
		millis_held = 0;
		firstTime = 0;
	}
	
	if (millis_held > longpress && executed == 0) {
	
		int btn = decodeButton(current);
		executeButton(btn, 1);
		executed = 1;
	
	}
	//Serial.println(millis_held);
	previous = current;
}

int decodeButton(int value) {

	Serial.println("Analog Value: " + String(value));
	if (value > btn1low && value < btn1high) return 1;
	else if (value > btn2low && value < btn2high) return 2;
	else if (value > btn3low && value < btn3high) return 3;
	else if (value > btn12low && value < btn12high) return 12;
	else if (value > btn23low && value < btn23high) return 23;
	else if (value > btn13low && value < btn13high) return 13;
	
	return 0;
  
}

void executeButton(int button, int press) {
	if (press) {
		longPressButton(button);
	} else {
		Serial.print("Momentary Press ");
		switch (button) {
			case 1:
				if (menu) {
				  menuSelect();
				  menu = 0;
				  digitalWrite(redPin, LOW);
				}
				else callButton();
				break;
			case 2:
				if (menu || inCall) down();
				else menuSelect();
				break;
			case 3:
				if (menu || inCall) up();
				else endButton();
				break;
			default:
				break;
		}
	}
	Serial.println(button);
}

void longPressButton(int button) {
	Serial.print("Long Press ");
	switch (button) {
		case 1:
			redial();
			break;
		case 2:
			if (!menu && !inCall) auxEnable1();
			break;
		case 3:
			if (menu || inCall) endButton();
			else auxEnable2();
			break;
		default:
			break;
	}
	Serial.println(button);
}

void auxEnable1() {
	digitalWrite(auxPin1, !digitalRead(auxPin1));
	EEPROM.write(auxPinMem1, digitalRead(auxPin1));
	if (digitalRead(auxPin1) == 1) blinkLed(greenPin, 3);
	else blinkLed(redPin, 3);
}

void auxEnable2() {
	digitalWrite(auxPin2, !digitalRead(auxPin2));
	EEPROM.write(auxPinMem2, digitalRead(auxPin2));
	if (digitalRead(auxPin2) == 1) blinkLed(greenPin, 3);
	else blinkLed(redPin, 3);
}

void blinkLed(int led, int times) {
	int counter = 0;
	while (counter < times) {
		digitalWrite(led, HIGH);
		delay(50);
		digitalWrite(led, LOW);
		delay(50);
		counter++;
	}
}

// Box specific functions
void command() {
	box.write(160);
	box.write(130);
}

void releaseButton() {
	delay(150);
	command();
	box.write(0);
}

void callButton() {
	command();
	box.write(2);
	releaseButton();
}

void redial() {
	command();
	box.write(2);
	delay(850);
	releaseButton();
}

void endButton() {
	digitalWrite(redPin, LOW);
	menu = 0;
	command();
	box.write(1);
	releaseButton();
}

void menuSelect() {
	digitalWrite(redPin, HIGH);
	menu = 1;
	command();
	box.write(4);
	releaseButton();
}

void up() {
	command();
	box.write(8);
}

void down() {
	command();
	box.write(248);
}

void reset() {
	digitalWrite(redPin, HIGH);
	resetConfirm = 0;
	command();
	box.write(3);
	delay(1850);
	releaseButton();
	digitalWrite(redPin, LOW);
}
