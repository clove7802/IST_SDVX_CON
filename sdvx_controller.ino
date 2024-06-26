#include <Joystick.h>
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD, 7, 0,
 true, true, false, false, false, false, false, false, false, false, false);

boolean hidMode, state[2]={false}, set[4]={false};
int encL=0, encR=0;
const int PULSE = 50;
byte EncPins[]    = {1, 0, 3, 2}; //엔코더핀 LBA RBA
byte ButtonPins[] = {11, 9, 7, 5,8,6,12}; //버튼연결된 핀번호 조이스틱버튼 순차적용
unsigned long ReactiveTimeoutMax = 1000;
 
byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
//byte SingleCount = sizeof(SinglePins) / sizeof(SinglePins[0]);
byte EncPinCount = sizeof(EncPins) / sizeof(EncPins[0]);
unsigned long ReactiveTimeoutCount = ReactiveTimeoutMax;

int ReportDelay = 700;
unsigned long ReportRate ;

void setup() {
  Serial.begin(9600);
  Joystick.begin(false);
  Joystick.setXAxisRange(-PULSE/2, PULSE/2-1);
  Joystick.setYAxisRange(-PULSE/2, PULSE/2-1);
  
  // setup I/O for pins
  for(int i=0;i<ButtonCount;i++) {
    pinMode(ButtonPins[i],INPUT_PULLUP);
  }
  // for(int i=0;i<SingleCount;i++) {
  //   pinMode(SinglePins[i],OUTPUT);
  // }
  for(int i=0;i<EncPinCount;i++) {
    pinMode(EncPins[i],INPUT_PULLUP);
  }

  //setup interrupts
  attachInterrupt(digitalPinToInterrupt(EncPins[0]), doEncoder0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(EncPins[2]), doEncoder1, CHANGE);
  
  // light mode detection
  hidMode = digitalRead(ButtonPins[0]);
  while(digitalRead(ButtonPins[0])==LOW) {
    if ( (millis() % 1000) < 500) {
      // for(int i=0;i<SingleCount;i++) {
      //   digitalWrite(SinglePins[i],HIGH);
      // }
    }
    else if ( (millis() % 1000) > 500) {
      // for(int i=0;i<SingleCount;i++) {
      //   digitalWrite(SinglePins[i],LOW);
      // }
    }
  }
  // for(int i=0;i<SingleCount;i++) {
  //   digitalWrite(SinglePins[i],LOW);
  // }

  //boot light
  for(int i=0; i<ButtonCount; i++) {
    // digitalWrite(SinglePins[i],HIGH);
    // delay(80);
    // digitalWrite(SinglePins[i],LOW);
  }
  for(int i=ButtonCount-2; i>=0; i--) {
    // digitalWrite(SinglePins[i],HIGH);
    // delay(80);
    // digitalWrite(SinglePins[i],LOW);
  }
  for(int i=0;i<ButtonCount ;i++) {
    // digitalWrite(SinglePins[i],HIGH);
  }
    delay(500);
  for(int i=0;i<ButtonCount ;i++) {
    // digitalWrite(SinglePins[i],LOW);
  }
} //end setup

void loop() {
  ReportRate = micros() ;
  
  // read buttons
  for(int i=0;i<ButtonCount;i++) {
    Joystick.setButton (i,!(digitalRead(ButtonPins[i])));
  }

  if(hidMode==false || (hidMode==true && ReactiveTimeoutCount>=ReactiveTimeoutMax)){
    for(int i=0;i<ButtonCount;i++) {
      // digitalWrite (SinglePins[i],!(digitalRead(ButtonPins[i])));
    }
  }
  else if(hidMode==true) {
    ReactiveTimeoutCount++;
  }

  //read encoders, detect overflow and rollover
  if(encL < -PULSE/2 || encL > PULSE/2-1)
  encL = constrain (encL*-1 , -PULSE/2, PULSE/2-1);
  if(encR < -PULSE/2 || encR > PULSE/2-1)
  encR = constrain (encR*-1 , -PULSE/2, PULSE/2-1);
  Joystick.setXAxis(encL);
  Joystick.setYAxis(encR);

  //report
  Joystick.sendState();
  delayMicroseconds(ReportDelay);
  //ReportRate Display
  Serial.print(micros() - ReportRate) ;
  Serial.println(" micro sec per loop") ;
}//end loop

//Interrupts
void doEncoder0() {
  if(state[0] == false && digitalRead(EncPins[0]) == LOW) {
    set[0] = digitalRead(EncPins[1]);
    state[0] = true;
  }
  if(state[0] == true && digitalRead(EncPins[0]) == HIGH) {
    set[1] = !digitalRead(EncPins[1]);
    if(set[0] == true && set[1] == true) {
      encL++;
    }
    if(set[0] == false && set[1] == false) {
      encL--;
    }
    state[0] = false;
  }
}

void doEncoder1() {
  if(state[1] == false && digitalRead(EncPins[2]) == LOW) {
    set[2] = digitalRead(EncPins[3]);
    state[1] = true;
  }
  if(state[1] == true && digitalRead(EncPins[2]) == HIGH) {
    set[3] = !digitalRead(EncPins[3]);
    if(set[2] == true && set[3] == true) {
      encR++;
    }
    if(set[2] == false && set[3] == false) {
      encR--;
    }
    state[1] = false;
  }
}
