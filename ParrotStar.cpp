//#########################################################
//###                                                   ###
//###            Saab ParrotStar BT Control             ###
//###                   Version 1.0                     ###
//###                    Rob Lester                     ###
//###             Hardware: Arduino MEGA                ###
//###                                                   ###
//#########################################################

#define box Serial1

// Operational Values
long millis_held;
unsigned long firstTime = 0;
int previous = 0;
int current = 0;
int executed = 0;

// The analog ranges need to be determined. When a button is pressed, 
// the value is printed to the console.

// Button 1 analog range
int btn1low = 150;
int btn1high = 200;

// Button 2 analog range
int btn2low = 800;
int btn2high = 850;

// Button 3 analog range
int btn3low = 920;
int btn3high = 930;

// Button 2+3 analog range
int btn23low = 910;
int btn23high = 920;

// Button 1+2 analog range
int btn12low = 400;
int btn12high = 500;

// Button 1+3 analog range
int btn13low = 620;
int btn13high = 630;

// Button Press Debounce/Longpress settings
int longpress = 1000; //How long should a long press be
int presstimeout = 200; //Set the firsttime value after this long of a press.

// Comms - don't change these
int conRes = 128;
int stopLoop = 0;
int lastComm = 0;

// PINS
int greenPin = 5;
int redPin = 4;
int buttonPin = 0; //Analog Pin for Control Buttons.

// Context Flags
int menu = 0;
int resetConfirm = 0;

void setup() { // Initial Boot Sequence
  
	Serial.begin(115200);
	Serial.println("Ready.");
	pinMode(greenPin, OUTPUT);
	pinMode(redPin, OUTPUT);
	pinMode(13, OUTPUT);
	box.begin(9600);
  
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
		int dp = digitalRead(13);
		digitalWrite(13, !dp);
	}
  
}

void serialEvent1() { // Receive commands from the brain box
	
	if (stopLoop == 0) {
		stopLoop = 1; // Stop the waiting loop, because the box has come online.
		digitalWrite(13, LOW);
	}
	
	int v = box.read(); // Read the command from the box.
	
	// Process specific commands from box
	
	if (v == 34) {
		digitalWrite(greenPin, LOW);
	}
	if (v == 17) {
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

	//int i = c.toInt();
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
				if (menu) menuSelect();
				else callButton();
				break;
			case 2:
				if (menu) down();
				break;
			case 3:
				if (menu) up();
				else endButton();
				break;
			case 12:
				break;
			case 23:
				break;
			case 13:
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
			if (resetConfirm) reset();
			else redial();
			break;
		case 2:
			menuSelect();
			break;
		case 3:
			if (menu) endButton();
			break;
		case 12:
			break;
		case 23:
			resetConfirm = 1;
			break;
		case 13:
			break;
		default:
			break;
	}
	Serial.println(button);
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
	resetConfirm = 0;
	command();
	box.write(3);
	delay(1850);
	releaseButton();
}
