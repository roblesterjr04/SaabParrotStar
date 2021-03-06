//#########################################################
//###                                                   ###
//###            Saab ParrotStar BT Control             ###
//###                  Version 1.2.4                    ###
//###                    Rob Lester                     ###
//###               Hardware: Atmega328p                ###
//###                                                   ###
//#########################################################

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
int btn1low = 200;
int btn1high = 300;

// Button 2 analog range
int btn2low = 50;
int btn2high = 200;

// Button 3 analog range
int btn3low = 350;
int btn3high = 500;

// Button Press Debounce/Longpress settings
int longpress = 1000; //How long should a long press be
int presstimeout = 200; //Set the firsttime value after this long of a press.
int low = 5; //Set the low threshold of the analog input. Anything below this will count as null/0/zilch. 

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
int ringing = 0;

// Debug data
int debug = false;

int at328 = true;

#define box Serial

void setup() { // Initial Boot Sequence
	
	pinMode(greenPin, OUTPUT);
	pinMode(redPin, OUTPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(auxPin1, OUTPUT);
	pinMode(auxPin2, OUTPUT);
	box.begin(9600);
	digitalWrite(auxPin1, EEPROM.read(auxPinMem1));
	digitalWrite(auxPin2, EEPROM.read(auxPinMem2));
	digitalWrite(ledPin, 1);
}
void(* resetFunc) (void) = 0;

void loop() { // 
	readAnalogController(); //Listen for analog button commands.
	
	// Tell parrot brain Box that we are waiting.
	if (!stopLoop) {
		box.write(160);
		box.write(128);
		delay(250);
		int bl = digitalRead(greenPin);
		digitalWrite(greenPin, !bl);
    //digitalWrite(ledPin, !bl);
	}
}

void debugPrint(String message) {
	if (!debug) return;
	if (at328) {
		box.println(message);
	} else {
		Serial.println(message);
	}
}

void serialEvent() { // Receive commands from the brain box
	
	if (stopLoop == 0) {
		stopLoop = 1; // Stop the waiting loop, because the box has come online.
		digitalWrite(ledPin, LOW);
	}

	int v = box.read();
	
	// Process specific commands from box
	
	if (v == 34) { 
		// At rest
		digitalWrite(greenPin, LOW);
		digitalWrite(redPin, LOW);
		inCall = 0;
		menu = 0;
		ringing = 0;
	}
	if (v == 17) { 
		// Phone is engaged
		digitalWrite(greenPin, HIGH);
		digitalWrite(redPin, LOW);
		inCall = 1;
		menu = 0;
		ringing = 0;
	}
	if (v == 2 || v == 32) {
		// Menu State
		digitalWrite(greenPin, HIGH);
		digitalWrite(redPin, LOW);
		menu = 1;
		inCall = 0;
		ringing = 0;
	}
	if (v == 119) {
		int ledState = digitalRead(greenPin);
		digitalWrite(greenPin, !ledState);
		menu = 0;
		inCall = 0;
		ringing = 1;
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

  readAnalogController();
	
	lastComm = v; // Set our last command
}

void readAnalogController() {
	delay(10);
	current = analogRead(buttonPin);
		
	if (previous > low && current <= low) {
		executed = 0;
	}
	
	if (previous > low && current <= low && millis_held < longpress) {
		int btn = decodeButton(previous);
		executeButton(btn, 0);
		debugPrint("Voltage: " + String(current));
		debugPrint("Button: " + String(btn));
	}
	
	if (current > low && previous <= low && (millis() - firstTime) > presstimeout) {
		firstTime = millis();
	}
	
	if (current > low) millis_held = (millis() - firstTime);
	else {
		millis_held = 0;
		firstTime = 0;
	}
	
	if (millis_held > longpress && executed == 0) {
	
		int btn = decodeButton(current);
		executeButton(btn, 1);
		executed = 1;
		debugPrint("Voltage: " + String(current));
		debugPrint("Button: " + String(btn));
				
	}
	
	if (millis_held > longpress * 10) {
		
		int btn = decodeButton(current);
		executeButton(btn, 2);
		
		debugPrint("Voltage: " + String(current));
		debugPrint("Button: " + String(btn));
		
	}
	
	previous = current;
}

int decodeButton(int value) {
	
	if (value > btn1low && value < btn1high) return 1;
	else if (value > btn2low && value < btn2high) return 2;
	else if (value > btn3low && value < btn3high) return 3;
	
	return 0;
  
}

void executeButton(int button, int press) {
	if (press == 1) {
		longPressButton(button);
	} else if (press == 2) {
		longHoldButton(button);
	} else {
		shortPressButton(button);
	}
}

void shortPressButton(int button) {
	switch (button) {
		case 1:
			if (menu) menuSelect();
			//else if (ringing) redial();
			else callButton();
			break;
		case 2:
			if (menu) down();
			else if (inCall) up();
			else menuSelect();
			break;
		case 3:
			if (menu) up();
			else endButton();
			break;
		default:
			break;
	}
}

void longHoldButton(int button) {
	switch (button) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			reset();
			break;
		default:
			break;
	}
}

void longPressButton(int button) {
	switch (button) {
		case 1:
			redial();
			break;
		case 2:
			if (!menu && !inCall) auxEnable1();
			if (inCall) down();
			break;
		case 3:
			if (menu || inCall) endButton();
			else auxEnable2();
			break;
		default:
			break;
	}
}

void auxEnable1() {
	digitalWrite(auxPin1, !digitalRead(auxPin1));
	EEPROM.write(auxPinMem1, digitalRead(auxPin1));
	blinkLed(greenPin, 3);
}

void auxEnable2() {
	digitalWrite(auxPin2, !digitalRead(auxPin2));
	EEPROM.write(auxPinMem2, digitalRead(auxPin2));
	blinkLed(greenPin, 3);
}

// Blink an LED. Do not use inside the main loop, or the serial interrupts.
void blinkLed(int led, int times) {
	int counter = 0;
	while (counter < times) {
		digitalWrite(led, HIGH);
		delay(100);
		digitalWrite(led, LOW);
		delay(100);
		counter++;
	}
}

// Box specific functions
void command(int command) {
	box.write(160);
	box.write(130);
	box.write(command);
}

void releaseButton() {
	delay(150);
	command(0);
}

void callButton() {
	blinkLed(greenPin, 2);
	command(2);
	releaseButton();
}

void redial() {
	command(2);
}

void endButton() {
	command(1);
	releaseButton();
}

void menuSelect() {
	command(4);
	releaseButton();
}

void up() {
	command(8);
}

void down() {
	command(248);
}

void reset() {
	debugPrint("Resetting Device. Goodbye.");
	digitalWrite(greenPin, HIGH);
	resetConfirm = 0;
	command(3);
	delay(2000);
	digitalWrite(greenPin, LOW);
	EEPROM.update(auxPinMem1, LOW);
	EEPROM.update(auxPinMem2, LOW);
	resetFunc();
}
