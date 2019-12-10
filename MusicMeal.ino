/*
 * MusicMeal
 * Final Project of DXARTS 490 - E-textile & Wearable in Art and Design
 * 
 * Created: Dec 2019
 * Author: Caspian Coberly & Jason Gao
 */ 

#include <MIDIUSB.h>
#include <CapacitiveSensor.h>

#define SNARE_DRUM A6
#define KICK_DRUM A7
#define HAT_DRUM A8

#define CHEESE A0

#define SEND_PIN 9
const byte receivers[] = {10, 16, 14, 15, 5, 19, 20, 21};
CapacitiveSensor notes[] = {
  CapacitiveSensor(9, 10),
  CapacitiveSensor(9, 16),
  CapacitiveSensor(9, 14),
  CapacitiveSensor(9, 15),
  CapacitiveSensor(9, 5),
  CapacitiveSensor(9, 19),
  CapacitiveSensor(9, 20),
  CapacitiveSensor(9, 21)
};
bool notesStatus[] = {false, false, false, false, false, false, false, false};

const long debounceDuration = 100;
const long noteDuration = 100;

unsigned long snareTime = 0;
unsigned long hatTime = 0;
unsigned long kickTime = 0;

bool onSnare = false;
bool onHat = false;
bool onKick = false;

void setup() {
  Serial.begin(9600);

  // initial eight capacitive sensors
  for (int i = 0; i < 8; i++) {
//    notes[i].set_CS_AutocaL_Millis(0xFFFFFFFF);
  }
}

void loop() {
  // Get drums' value
  int snareValue = analogRead(SNARE_DRUM);
  int kickValue = analogRead(KICK_DRUM);
  int hatValue = analogRead(HAT_DRUM);
  int cheeseValue = analogRead(CHEESE);

  // Debug
  Serial.print("snare:");
  Serial.print(snareValue);
  Serial.print(" kick:");
  Serial.print(kickValue);
  Serial.print(" hat:");
  Serial.print(hatValue);
  Serial.print(" cheese:");
  Serial.print(cheeseValue);
  Serial.println();

  unsigned long currentTime = millis();

  // Cheese
  int sliderValue = map(cheeseValue, 550, 1023, 0, 127);
  controlChange(0, 12, sliderValue);

  // Snare
  if (!onSnare && snareValue > 300 && currentTime - snareTime > debounceDuration) {
    noteOn(0, 44, 64);
    snareTime = currentTime;
    onSnare = true;
  }
  if (onSnare && currentTime - snareTime > noteDuration) {
    noteOff(0, 44, 64);
    onSnare = false;
  }

  // Hat
  if (!onHat && hatValue > 160 && currentTime - hatTime > debounceDuration) {
    noteOn(0, 45, 64);
    hatTime = currentTime;
    onHat = true;
  }
  if (onHat && currentTime - hatTime > noteDuration) {
    noteOff(0, 45, 64);
    onHat = false;
  }

  // Kick
  if (!onKick && kickValue > 260 && currentTime - kickTime > debounceDuration) {
    noteOn(0, 46, 64);
    kickTime = currentTime;
    onKick = true;
  }
  if (onKick && currentTime - kickTime > noteDuration) {
    noteOff(0, 46, 64);
    onKick = false;
  }

  // Get note value
  for (int i = 0; i < 8; i++) {
    int value = notes[i].capacitiveSensor(30);

    if (value >= 500 && !notesStatus[i]) {
      noteOn(0, 36 + i, 64);
      notesStatus[i] = true;
    }
    if (value < 500 && notesStatus[i]) {
      noteOff(0, 36 + i, 64);
      notesStatus[i] = false;
    }
  }

  MidiUSB.flush();
  
  // controlChange(0, 10, 65); // Set the value of controller 10 on channel 0 to 65
  delay(10);
}


// Functions for MIDI output

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
