#define box Serial1
#define con Serial2

//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(10, 11);
//SoftwareSerial mySerialTwo(5, 6);

int conRes = 128;
int stopLoop = 0;
int lastComm = 0;

void setup() {
  // put your setup code here, to run once:
  //while(!Serial){}
  Serial.begin(115200);
  Serial.println("Ready.");
  pinMode(13, OUTPUT);
  //digitalWrite(13, HIGH);
  box.begin(9600);
  con.begin(9600);
  
  

}

int counter = 0;

void loop() {
  if (!stopLoop) {
    box.write(160);
    //Serial.println("T: 160");
    box.write(128);
    //Serial.println("T: 128");
    delay(500);
  }
}

void serialEvent1() {
  if (stopLoop == 0) {
    stopLoop = 1;
  }
  int v = box.read();
  if (v == 34) digitalWrite(13, LOW);
  if (v == 17) digitalWrite(13, HIGH);
  if (lastComm == 160 && v == 1) {
    box.write(160);
    box.write(129);
    //Serial.println("T: 129");
    box.write(2);
  }
  if (lastComm == 160 && v == 4) {
    box.write(160);
    box.write(132);
    //Serial.println("T: 132");
  }
  if (lastComm == 160 && conRes == 128) {
    box.write(160);
    box.write(131);
    conRes = 131;
    //Serial.println("T: 131");
  }
  if (v != 4 && v != 160 && v != 1) {
    Serial.print("Box: ");
    Serial.println(v, DEC);
  }
  lastComm = v;
  //con.write(v);
}

void serialEvent2() {
  int v = con.read();
  if (v != 160 && v != 132 && v != 128) {
    Serial.print("Con: ");
    Serial.println(v, DEC);
  }
  //con.write(v);
}

void serialEvent() {
  String c = "";
  while(Serial.available() > 0) {
    int in = Serial.read();
    c = c + in;
  }
  
  //int i = c.toInt();
  if (c == 56 || c == 55 || c == 52 || c == 54) {
    box.write(160);
    box.write(130);
    if (c == 56) box.write(8);
    if (c == 55) box.write(248);
    if (c == 54) {
      box.write(1);
      box.write(2);
      delay(750);
      box.write(160);
      box.write(130);
      box.write(0);
    }
    if (c == 52) {
      box.write(4);
      delay(250);
      box.write(160);
      box.write(130);
      box.write(0);
    }
    Serial.println("knob");
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
}


