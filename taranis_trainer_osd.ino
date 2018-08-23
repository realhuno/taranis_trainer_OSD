#define INPUT_FREQUENCY 50

#define CHANNEL_MAX  2000
#define CHANNEL_MIN 1000
#define CHANNEL_MID 1500
#define CHANNEL_NUMBER 8


#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 10  //set PPM signal output pin on the arduino

int team1=1000;
int team2=1000;
int flag1=1000;
int flag2=1000;


int ppm[CHANNEL_NUMBER];

int prevSwitch = LOW;
byte currentState = LOW;

void setup() {
  delay(300);
  Serial.begin(57600);
  
  //initiallize default ppm values
  for (int i=0; i<CHANNEL_NUMBER; i++){
      ppm[i]= CHANNEL_MID;
  }

  ppm[CHANNEL_NUMBER - 1] = 1000;



 
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
  Serial.println("Ready...");
}



void loop() {

if (Serial.available() > 0) {
    const char command = Serial.read();

    switch (command) {
     
      case 'f': {
          uint8_t index = Serial.parseInt();
          uint16_t value = Serial.parseInt();
ppm[index]=value;

      }
      }
    }
  



  
  delay(1000 / INPUT_FREQUENCY);

}

ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if (state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PULSE_LENGTH * 2;
    state = false;
  } else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= CHANNEL_NUMBER){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;// 
      OCR1A = (FRAME_LENGTH - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PULSE_LENGTH) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
