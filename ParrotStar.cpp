//#########################################################
//###                                                   ###
//###            Saab ParrotStar BT Control             ###
//###                   Version 1.0                     ###
//###                    Rob Lester                     ###
//###             Hardware: Arduino MEGA                ###
//###                                                   ###
//#########################################################

#define box Serial1

int buttonPin = 0; //Analog Pin for Control Buttons.

long millis_held;
unsigned long firstTime = 0;
int previous = 0;
int current = 0;
int executed = 0;

// Button 1 analog range
int btn1low = 200;
int btn1high = 300;

// Button 2 analog range
int btn2low = 500;
int btn2high = 600;

// Button 3 analog range
int btn3low = 700;
int btn3high = 800;

// Button 2+3 analog range
int btn23low = 600;
int btn23high = 700;

// Button 1+2 analog range
int btn12low = 300;
int btn12high = 400;

int longpress = 50; //How long should a long press be
int presstimeout = 3; //Set the firsttime value after this long of a press.

int conRes = 128;
int stopLoop = 0;
int lastComm = 0;

// OUTPUT PINS
int greenPin = 6;

void setup() {
  
	Serial.begin(115200);
	Serial.println("Ready.");
	pinMode(13, OUTPUT);
	pinMode(greenPin, OUTPUT);
	digitalWrite(13, HIGH);
	box.begin(9600);
  
}

void loop() {
  
	readAnalogController(); //Listen for analog button commands.
	
	// Tell Box that we are waiting.
	if (!stopLoop) {
		box.write(160);
		box.write(128);
		delay(500);
	}
  
}

void serialEvent1() {
	
	if (stopLoop == 0) stopLoop = 1; // Stop the waiting loop, because the box has come online.
	
	int v = box.read(); // Read the command from the box.
	
	// Process specific commands from box
	
	if (v == 34) digitalWrite(greenPin, LOW);
	if (v == 17) digitalWrite(greenPin, HIGH);
	
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
	
	String c = "";
	while(Serial.available() > 0) {
		int in = Serial.read();
		c = c + in;
	}


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
		box.write(c.toInt());
		delay(250);
		box.write(160);
		box.write(130);
		box.write(0);
	}
	Serial.print("Rec: ");
	Serial.println(c.toInt());
	lastSerial = c.toInt();
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
  
  if (value > btn1low && value < btn1high) return 1;
  else if (value > btn2low && value < btn2high) return 2;
  else if (value > btn3low && value < btn3high) return 3;
  else if (value > btn12low && value < btn12high) return 12;
  else if (value > btn23low && value < btn23high) return 23;
  
  return 0;
  
}

void executeButton(int button, int press) {
	if (press) {
		longPressButton(button);
	} else {
		Serial.print("Momentary Press ");
		switch (button) {
			case 1:
				Serial.println(button);
				break;
			case 2:
				Serial.println(button);
				break;
			case 3:
				Serial.println(button);
				break;
			default:
				Serial.println(button);
				break;
		}
	}
  
}

void longPressButton(int button) {
	Serial.print("Long Press ");
	switch (button) {
		case 1:
			Serial.println(button);
			break;
		case 2:
			Serial.println(button);
			break;
		case 3:
			Serial.println(button);
			break;
		default:
			Serial.println(button);
			break;
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
	command();
	box.write(1);
	releaseButton();
}

void menuSelect() {
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
	command();
	box.write(3);
	delay(1850);
	releaseButton();
}
