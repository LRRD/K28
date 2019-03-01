//Mason Parrone 4/2018 Code for digital flow controller redesign for use with new Brushless submersible pump (0-5v speed control signal)
//Stripped down encoder code to bare bones and moved all inputs to digital pins that aren't PWM. This previously used the analog pins that we may need for the DAC in the future.
//Added in LCD code from RS-232 display code
//Added in DAC code for MCP 4725 Adafruit version
//Added maxFlow and startFlow to make encoder behavior better and replace constant numbers further down in code with a magic ones
//Stripped down K500 digital flow controller code to get K28 code
//Added lines of code to control on board LED with PWM from pin D3
//Removed knob acceleration

#include <Wire.h>//Include the Wire library to talk I2C
#define MCP4725_ADDR 0x62 //Unique address for MCP4725 Adafruit version 


////////////////////////////////////////////////////////////////////////////
// Magic Numbers & Global Variables ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
const byte led = 3;           //LED pin
const byte led2 = 5;
float encoder_count = 0.0;
int released;
int lowFlow = 1500;         //~Encoder count below lowest flow setting

////////////////////////////////////////////////////////////////////////////
// DAC Controls ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void writePWM() //Function to control analog voltage out from DAC
{
  int pwm2 = round((encoder_count / 4095) * 255); //Relate encoder count to value being sent to LED 
  int pwm = encoder_count; //Relate current encoder count to value being sent to DAC.
  Wire.beginTransmission(MCP4725_ADDR);
  Wire.write(64);                     // cmd to update the DAC
  Wire.write(pwm >> 4);        //Write PWM Value with the 8 most significant bits...
  Wire.write((pwm & 15) << 4); //Add the 4 least significant bits...
  Wire.endTransmission();
  analogWrite(led, pwm2); //Write to LED with encoder count value
}


////////////////////////////////////////////////////////////////////////////
// Encoder handling  ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Taken From Alix controller code.
// The encoder count value.
// The sequence of gray codes in the increasing and decreasing directions.
static uint8_t cw_gray_codes[4] = { 2, 0, 3, 1 };
static uint8_t ccw_gray_codes[4] = { 1, 3, 0, 2 };
// The gray code we last read from the encoder.
static uint8_t previous_gray_code = 0;
#define encoderswitch 7
#define channelB 4
#define channelA 2


// Reset the encoder to zero.
static void reset_encoder()
{
  encoder_count = 0;
}


// Look for encoder rotation, updating encoder_count as necessary.
static void check_encoder()
{
  // Get the Gray-code state of the encoder.
  // A line is "low" if <= 512; "high" > 512 or above.
  int gray_code = ((digitalRead(channelA) == HIGH) << 1) | (digitalRead(channelB) == HIGH);

  // If the gray code has changed, adjust the intermediate delta.
  if (gray_code != previous_gray_code)
  {
    if (encoder_count < lowFlow)  //Coarse adjustment
    {
      //Knob twist CW
      if (gray_code == cw_gray_codes[previous_gray_code])
      {
        encoder_count += 50;
      }
  
      //Knob twist CCW
      else if (gray_code == ccw_gray_codes[previous_gray_code])
      {
        encoder_count -= 50;
      }
    }
    
    else     //Fine adjustment
    {
      //Knob twist CW
      if (gray_code == cw_gray_codes[previous_gray_code])
      {
        encoder_count += 16;     //Allows for 256 values (4096/16 = 256) good enough for fine controls, but not so much that you need to turn the knob a lot
      }
  
      //Knob twist CCW
      else if (gray_code == ccw_gray_codes[previous_gray_code])
      {
        encoder_count -= 16;
      }
    }
    previous_gray_code = gray_code; //Stores current gray code for future comparison
    encoder_count = constrain(encoder_count, 0, 4095);
  }
}



////////////////////////////////////////////////////////////////////////////
// Switch handling  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

static unsigned int switch_was_down = 0;
static unsigned int switch_released()
{
  // Becuase it's an input pullup switch reads high when pressed.
  int switch_is_down = (digitalRead(encoderswitch) == HIGH);

  // The action takes place when the switch is released.
  released = (switch_was_down && !switch_is_down);

  // Remember the state of the switch.
  switch_was_down = switch_is_down;

  // Was the switch just released?
  return released;
}



//////////////////////////////////////////////////////////////////////////////
/// Screen Refresh   /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

long elapsed_ms; //used to refresh the lcd Screen
static long last_display_update_ms = 0; //used to refresh the lcd Screen

void refresh_lcd()
{
  elapsed_ms = millis() - last_display_update_ms;
  writePWM();
  Serial.println(encoder_count);
}


//////////////////////////////////////////////////////////////////////////////////
// Setup   ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  Wire.begin();           //Initialize wire library for DAC
  pinMode(encoderswitch, INPUT_PULLUP); //Set the encoder button to input pullup. Most buttons work best as input pullup.
  pinMode(channelA, INPUT_PULLUP); //For grayhill 62AG11-L5-060C reverse compatible with older grayhill op encoder as well
  pinMode(channelB, INPUT_PULLUP); //For grayhill 62AG11-L5-060C reverse compatible with older gryahill op encoder as well
  pinMode(led, OUTPUT);   //LED setup as output
  pinMode(led2, OUTPUT);   //LED setup as output
  reset_encoder();        //Initialize the encoder by setting encoder count value to 0.
}

//////////////////////////////////////////////////////////////////////////////////
// Loop  /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void loop()
{
  int switch_event;
  check_encoder();
  switch_event = switch_released();  //Run routine that checks switch
  refresh_lcd(); //Periodically update the display.
  //digitalWrite(led, HIGH);
  //digitalWrite(led2, HIGH);
  if (digitalRead(encoderswitch) == LOW) //Encoder Pressed
  {
    reset_encoder();
    delay (200);
  }
}
