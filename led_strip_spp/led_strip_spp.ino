//#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(52, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

/*-- Example of commmands:
    HAND:125,56,44,144,153;
    Values from 0 to 180
*/

//Servo servo[NUMBER_OF_SERVOS];

String command;

int strip_Color[3];

void setup()
{
  //-- Attach the servos. Order for right hand, counting from the thumb to the little finger.
  //-- Servos are connected from digital pin #6 to #10. Servo for wrist is connected at #11 digital pin.
  
  //-- Servo setup
  //for (int i=0; i<NUMBER_OF_SERVOS; i++) servo[i].attach(i+6);

  //-- Servo inti position:
  /*for (int i=0; i<NUMBER_OF_SERVOS; i++){
    servo_pos[i]=90;
    servo[i].write(servo_pos[i]);
    } 
*/
  Serial.begin(19200);
  
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

    Serial.println("---- Wellcome to control board ----");
    Serial.println("Available commands:");
    Serial.println("STRIP_COLOR:255,255,255;");
    Serial.println("RAINBOW:ms_delay;");

}

void loop()
{

    if(Serial.available()){
        char c = Serial.read();

        if(c == '\n'){
          parseCommand(command);
          command = "";
        }
        else command += c;
    }

      //for(int i=0; i<NUMBER_OF_SERVOS; i++) servo[i].write(servo_pos[i]);

}

void parseCommand(String com)
{
  String part1;
  String part2;

  part1 = com.substring(0,com.indexOf(":"));
  part2 = com.substring(com.indexOf(":")+1,com.indexOf(";"));

  if(part1.equalsIgnoreCase("STRIP_COLOR"))
  {
    //Serial.print("HAND recognized: ");

    //-- STRIP_COLOR:255,255,255;

    for(int i=0; i<3; i++){

      strip_Color[i] = getValuesFromCommand(part2);
      //Serial.print(strip_Color[i]);
      //Serial.print("\t");
    }

      //Serial.println("_");

    // Move actuators of HAND:
    //for(int i=0; i<HAND_VALUES; i++) servo[i].write(servo_pos[i]);

    //Print colorwipe:
    colorWipe(strip.Color(strip_Color[0], strip_Color[1] , strip_Color[2] ), 0); // Blue


  }

  else if(part1.equalsIgnoreCase("RAINBOW"))
  {
    /*
    RAINBOW:ms_delay;

      int fingerIndex = getValuesFromCommand(part2);
      servo_pos[fingerIndex] = getValuesFromCommand(part2);
      // Move the finguer #fingerIndex:
      servo[fingerIndex].write(servo_pos[fingerIndex]);
      */
      rainbow(getValuesFromCommand(part2));


  }
  else if(part1.equalsIgnoreCase("WRIST"))
  {
    /*servo_pos[5] = getValuesFromCommand(part2);
    servo[5].write(servo_pos[5]);    
    */

  }
  else
  {
    Serial.print("Command not recognized");
   // Serial.println(com);

  }  

}

int getValuesFromCommand (String& command_values){
  int value = command_values.substring(0,command_values.indexOf(",")).toInt();
  command_values = command_values.substring(command_values.indexOf(",")+1);
  return value;
}

//------------------------ Strips functions:


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}