//#########################################################
//###                                                   ###
//###            Saab ParrotStar BT Control             ###
//###                   Version 1.0                     ###
//###                    Rob Lester                     ###
//###                                                   ###
//#########################################################

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

void setup() {
  Serial.begin(9600);
  Serial.println("Ready.");
  pinMode(13, OUTPUT);
}

void loop() {
  
  readAnalogController(); //Listen for analog button commands.
  
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
        digitalWrite(11, HIGH);
        break;
      case 2:
        Serial.println(button);
        digitalWrite(12, HIGH);
        break;
      case 3:
        Serial.println(button);
        digitalWrite(13, HIGH);
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
      digitalWrite(11, LOW);
      break;
    case 2:
      Serial.println(button);
      digitalWrite(12, LOW);
      break;
    case 3:
      Serial.println(button);
      digitalWrite(13, LOW);
      break;
    default:
      Serial.println(button);
      break;
  }
}
