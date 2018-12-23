void calibration() {
  #define TIMEOUT 5000  // Set timout(ms) for auto-accept calibration data
  unsigned long currentTime = millis();

  // Keep the leds on during calibration mode
  digitalWrite(13, HIGH);
  
  for (byte i=0;i<2;i++) {
    if (rc_values[i] < 2500 && rc_values[i] > rc_max_values[i]) {  // ...looking for the MAX's,
      rc_max_values[i] = rc_values[i];
      calTimer = currentTime + TIMEOUT;
    }
    else if (rc_values[i] > 0 && rc_values[i] < rc_min_values[i]) {  // ...then for the MIN's
      rc_min_values[i] = rc_values[i];
      calTimer = currentTime + TIMEOUT;
    }
  }
  
  if (calTimer && calTimer <= currentTime) {
    uint16_t rc_diff_values[2];
    
    #ifdef DEBUG_ENABLED
      static char str[64];

      Serial.println("\tCH1\tCH2");
      sprintf(str,"MAX:\t%d\t%d\n",rc_max_values[0],rc_max_values[1]);
      Serial.print(str);
      sprintf(str,"MIN:\t%d\t%d\n",rc_min_values[0],rc_min_values[1]);
      Serial.print(str);
    #endif
    
    for (byte x=0;x<2;x++) {
      rc_diff_values[x] = rc_max_values[x] - rc_min_values[x];
      if (rc_diff_values[x] < 360) {
        rc_diff_values[x] = 0;
        #ifdef DEBUG_ENABLED
          Serial.print("\t X ");
        #endif
      }
      #ifdef DEBUG_ENABLED
        else Serial.print("\tOK ");
      #endif
    }
    
    if (rc_diff_values[0] && rc_diff_values[1] || digitalRead(A1) == LOW) {
      ledBlink(4);  // Led blinks when calibration succeeded
      writeMem();  // Store cal.values in the eeprom
      
      #ifdef DEBUG_ENABLED
        Serial.print("\n\nCALIBRATION FINISHED\n");
      #else
        //Joystick.begin();
      #endif
      
      calMode = false;  // reset the flag
    }
    else {
      #ifdef DEBUG_ENABLED
        Serial.print("\n\nALL CHANNEL MUST BE \"OK\" TO AUTOSAVE!\nOTHERWISE PRESS BUTTON TO SAVE CAL.DATA.\n\n");
      #endif
      
      calTimer = 0;  // Reset timeout
    }
  }
}

void ledBlink(byte n) {
  for (n; n>0; n--) {
    digitalWrite(13, HIGH);
    delay(80);
    digitalWrite(13, LOW);    
delay(80);
  }
}
