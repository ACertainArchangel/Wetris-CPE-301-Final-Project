int button1Pin = 10;
int button2Pin = 9;
int button3Pin = 8;

int button1Val;
int button2Val;
int button3Val;

int button1LastVal = LOW;
int button2LastVal = LOW;
int button3LastVal = LOW;

unsigned char* portDDRB = (unsigned char*) 0xF8; // 8-10 set as inputs, rest are outputs
//test
// DDRB = B11111000 // 8-10 as input

void setup() {
  Serial.begin(9600);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
}

void loop() {
  //debounce();
  button1Val = digitalRead(button1Pin);
  button2Val = digitalRead(button2Pin);
  button3Val = digitalRead(button3Pin);
  if (button1Val == LOW && button1LastVal == HIGH) {
    Serial.println("Button 1 pressed");
  }
  if (button2Val == LOW && button2LastVal == HIGH) {
    Serial.println("Button 2 pressed");
  }
  if (button3Val == LOW && button3LastVal == HIGH) {
    Serial.println("Button 3 pressed");
  }
  button1LastVal = button1Val;
  button2LastVal = button2Val;
  button3LastVal = button3Val;
}