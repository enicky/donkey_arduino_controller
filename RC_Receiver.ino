#include <EEPROM.h>
#include <PinChangeInterrupt.h>

#define PWM_RECEIVER
//#define DEBUG_ENABLED 

uint16_t rc_values[2] = {0,0}; // channel values
uint16_t rc_min_values[2], rc_max_values[2]; //min - max 

#ifdef PWM_RECEIVER
uint8_t rc_pins[6] = {8,9};
uint8_t rc_flags[6] = {1,2};

volatile uint8_t rc_shared_flags;
volatile uint16_t rc_shared_values[6];
volatile uint32_t rc_shared_ts[6];
#endif

unsigned long calTimer, ledTimer;
boolean calMode;

#define LED 13

void setup() {
  // put your setup code here, to run once:
  pinMode(A1, INPUT_PULLUP); // button pin for calibration
  pinMode(LED, OUTPUT);

  Serial.begin(9600);
  while(!Serial){
    digitalWrite(LED, LOW);

    delay(250);
    digitalWrite(LED, HIGH);
    delay(250);
  }
  //Serial.println("Start");
  //Serial.println("Arduino Wireless Adapter (DEBUG MODE)");

  
  
  readMem();
  //rc_min_values[0] = 0;
  //rc_min_values[1] = 0;

  //printMemoryValues();
  //Serial.println("Memory Read ... Setup interrupts");
  rc_setup_interrupts();
  //Serial.println("Interrupts setup");
  if (rc_min_values[0,1] < 360 || rc_max_values[0,1] > 3000 || digitalRead(A1) == LOW) {
    calMode = true;
    //Serial.println("Calibration mode on");
    // Turn leds on during calibration
    digitalWrite(LED, HIGH);
    

    //Serial.println("CALIBRATION ACTIVE");
    
    // Set initial minimum values (very?) high for calibration
    for (byte i=0;i<2;i++) {
        rc_min_values[i] = 2500;
        rc_max_values[i] = 0;
    }
  }
  else {
    #ifdef DEBUG_ENABLED
      Serial.println("CALIBRATION DATA LOADED FROM EEPROM.");
    #endif
  }
}

int clamp(int x, int low, int high) {
  if (x < low) return low;
  if (x > high) return high;
  return x;
}



void loop() {
  // put your main code here, to run repeatedly:
  rc_process_channels();  // Measure channels pwm timing values.
  
  if(!calMode){
    int mappedValueSteering = map(rc_values[0] *1.0,rc_min_values[0]*1.0,rc_max_values[0]*1.0, -100.0, 100.0) * 1.0;
    int mappedValueThrottle = map(rc_values[1] *1.0,rc_min_values[1]*1.0,rc_max_values[1]*1.0, -100.0, 100.0) * 1.0;
    if((mappedValueSteering > -100 && mappedValueSteering < 100)&&( mappedValueThrottle > -100 && mappedValueThrottle < 100)){
      rc_print_channels(mappedValueSteering, mappedValueThrottle);
    }
    
    
  }else {
    calibration();  // Do calibration if flag active.
  }
  // Observed ranges:
  // - steering: min = 1175, center = 1475, max = 1840
  // - throttle: min = 940, center = 1488, max = 2004 
  delay(50);
}

void rc_print_channels(int s, int t) {
  static char str[70];
  sprintf(str, "B %d %d", s,t);
  Serial.println(str);
}
