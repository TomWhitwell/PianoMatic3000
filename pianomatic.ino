/*

 PIANO-MATIC 3000 
 Midi colour generator and note counter 
 
 
 Circuit: 
 
 Midi isolation/inversion circuit from:
 http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1187962258/
 Going into Serial RX pin. 
 
 NB: Add a jumper or switch to disconnect from Serial RX to allow sketches to be uploaded! 
 
 RGB LED: 
 D9 = Red 
 D10 = Green 
 D11 = Blue 
 
 Note count LEDs: 
 D4 = first 
 D5 = second
 D6 = third 
 
 Switch LED = D3 
 
 
 */

// THRESHOLDS FOR THE NOTE LEDs 
int notebonus1=200;
int notebonus2=500;
int notebonus3=2000;

#include <MIDI.h>  // Add Midi Library

// Assign pins to LEDs 
#define redLED 9
#define greenLED 10
#define blueLED 11
#define bonusLED1 4
#define bonusLED2 5
#define bonusLED3 6
#define switchLED 3 


// Set up variables 
int r; // RGB = colour variables for output 
int g;
int b;
int targetr; // target rgb that colours are fading towards 
int targetb;
int targetg;

int refresh; // sets speed of refresh, influences speed of fades etc 
int refreshcount; // counter for refreshes 
int notecount; // overall note counter 
float fade = 254; // brightness of switch LED 
unsigned long timeout = 0; // timeout counter 
unsigned long timeouttime = 20000; // timeout for switch LED - how many millis after last note before it starts to flash 
unsigned long time; // holder for milliseconds since boot 
boolean boot = true; // indicates first run - to allow for LED switch fade 

void MyHandleNoteOn(byte channel, byte pitch, byte velocity) { // Triggered by Note On 
  if (velocity>0){
    notecount++;
  }; // don't add to the note count for note off messages 

  timeout = time;
  boot = false;
  refresh=(127-velocity)*4; // connects velocity to fade speed 

  // COLOUR SETTINGS 
  if (pitch % 12 == 0){ // C
    targetr=254;
    targetg=0;
    targetb=0;
  }
  else if (pitch % 12 == 2){ // D
    targetr=0;
    targetg=254;
    targetb=0;
  }
  else if (pitch % 12 == 4){ // E
    targetr=0;
    targetg=0;
    targetb=254;
  }
  else if (pitch % 12 == 5){ // F
    targetr=254;
    targetg=254;
    targetb=0;
  }
  else if (pitch % 12 == 7){ // G
    targetr=0;
    targetg=254;
    targetb=254;
  }
  else if (pitch % 12 == 9){ // A
    targetr=254;
    targetg=0;
    targetb=254;
  }
  else if (pitch % 12 == 11){ // B
    targetr=200;
    targetg=200;
    targetb=200;
  }
  else     
  {
    targetr=random(254);
    targetg=random(254);
    targetb=random(254);
  }; //black notes are random 


  if (velocity == 0) {//A NOTE ON message with a velocity = Zero is actualy a NOTE OFF
    targetr=0;
    targetg=0;
    targetb=0;

    ;
  }





}

void MyHandleNoteOff(byte channel, byte pitch, byte velocity) { // Some MIDI keyboards use this for note off 
  targetr=0;
  targetg=0;
  targetb=0;

}




void setup() {
  pinMode (bonusLED1, OUTPUT);
  pinMode (bonusLED2, OUTPUT); 
  pinMode(bonusLED3, OUTPUT);
  pinMode(switchLED, OUTPUT);

  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
  // to respond to channel 2 notes only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This is important!! This command
  // tells the Midi Library which function I want called when a Note ON command
  // is received. in this case it's "MyHandleNoteOn".
  MIDI.setHandleNoteOff(MyHandleNoteOff); // This is important!! This command
  // tells the Midi Library which function I want called when a Note ON command
  // is received. in this case it's "MyHandleNoteOn".
}

void loop() { // Main loop
  MIDI.read(); // Continually check what Midi Commands have been received.
  time=millis();

  if (refreshcount>refresh){ // DON'T UPDATE EVERY CYCLE 

    //FADE ROUTINE
    //while((targetr != r) || (targetg != g) || (targetb != b)){
    if (targetr<r){
      r=r-1;
    };
    if (targetg<g){
      g=g-1; 
    };
    if (targetb<b){
      b=b-1; 
    };

    if (targetr>r){
      r=r+2; 
    };
    if (targetg>g){
      g=g+2; 
    };
    if (targetb>b){
      b=b+2; 
    }; 


    // UPDATE LEDS 

    // RGB led     
    analogWrite(redLED,r);
    analogWrite(greenLED, g);
    analogWrite(blueLED,b);

    // note count LEDs
    if (notecount>notebonus1){
      digitalWrite(bonusLED1,HIGH); 
    }
    if (notecount>notebonus2){
      digitalWrite(bonusLED2,HIGH); 
    }
    if (notecount>notebonus3){
      digitalWrite(bonusLED3,HIGH); 
    }

    // switch LED 

    if (time-timeout>timeouttime) // If box has been left switched on, flash battery light 
    {
      if (time % 1000 < 500){
        analogWrite(switchLED,254);
      }
      else { 
        analogWrite(switchLED,0);
      }
    }


    if (boot == true){ // on first run, nicely fade the switch light until the first note is played 
      analogWrite(switchLED,fade);
      fade=fade-0.005;
      if (fade<.02){
        boot=false;
      };
    }



    refreshcount = 0;  

  } 

  refreshcount++;

}



