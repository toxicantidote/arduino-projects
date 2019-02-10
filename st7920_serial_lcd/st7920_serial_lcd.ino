/* ST7920 serial interface

   Toxicantidote - Feb 2019

   Lets you draw stuff on an ST7920 LCD using serial commands. This is
   set up for a 128x64 LCD like the XC4617 from Jaycar.
   
   Serial interface is 115200 baud, 8 data bits, no parity, 1 stop bit.
   Enter HELP for command help.   
   
   Depends on the u8g2lib from https://github.com/olikraus/u8g2
   
   This is currently setup for a display connected using a parallel interface.
   SPI is possible by changing the arguments to the U8G2_ST7920_128X64_F_8080
   u8g2 constructor. See u8g2 docs/examples for details.
*/   

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Command prompt
#define CMDPROMPT "ST7920 LCD> "

// cmd line state vars
char cmdBuffer[32];
int cmdBufferPosition = 0;
char cmdByte;
int cmdStatus = 0;


// cmd buffers
char command[32];
char argument[32];

// initialise extra functions
void handleCommand(char*);
void showHelp(void);
void resetBuffers(void);
void init_screen(void);
void clear_screen(void);
void commit_screen(void);
void draw_box(uint8_t, uint8_t, uint8_t, uint8_t);
void draw_frame(uint8_t, uint8_t, uint8_t, uint8_t);
void draw_disc(uint8_t, uint8_t, uint8_t);
void draw_circle(uint8_t, uint8_t, uint8_t);
void draw_rounded_box(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void draw_rounded_frame(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void draw_text(uint8_t, uint8_t, uint8_t, char*);
void draw_line(uint8_t, uint8_t, uint8_t, uint8_t);
void draw_triangle(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

// LCD init for 128x64 screen, like Jaycar XC4617
// arguments: U8G2_R0, d0, d1, d2, d3, d4, d5, d6, d7, enable, U8X8_PIN_NONE, rs, reset
U8G2_ST7920_128X64_F_8080 u8g2(U8G2_R0, 30, 31, 32, 33, 34, 35, 36, 37, 40, U8X8_PIN_NONE, 38, 41);

// setup function
void setup() {  
  // start serial. do it at 115.2k to allow sending commands a bit faster
  Serial.begin(115200);
  
  // RW is connected to pin 39, set it low
  pinMode(39, OUTPUT);
  digitalWrite(39, LOW);
  
  // get the LCD ready
  u8g2.begin();
  clear_screen();
  init_screen();
  
  // print a welcome message
  draw_text(10, 10, 0, "Ready");
  commit_screen();
}

// main loop
void loop() {
  // command stuff
  for (int i = 0; i < Serial.available() && cmdStatus == 0; i++) { // if serial data is available to read
      cmdStatus = 0;
      cmdByte = Serial.read(); // read it
      if (cmdByte == 0x0A || cmdByte == 0x0D) { // newline and carriage return
          Serial.flush(); // clear the input buffer
          if (cmdBufferPosition == 0) { // if command is blank
            resetBuffers(); // reset buffers
          } else { // if the command is not blank
            cmdStatus = 1; // flag the command as 'ready for processing'
          }
      } else if (cmdByte == 0x7F) { // backspace
          if (cmdBufferPosition != 0) { // don't backspace further than the prompt
            cmdBufferPosition--;
            // backspace on the client
            Serial.print('\b');
          }
      } else { // other char, add to buffer
        // restrict to printable characters
        if (cmdByte >= 0x20  && cmdByte <= 0x7E) {
          cmdBuffer[cmdBufferPosition] = cmdByte; // append to buffer
          cmdBufferPosition++; // increment buffer position
        
          // echo the character back to the client
          Serial.print(cmdByte);
        
        }
      }
  }
  
  if (cmdStatus == 1) { // cmd received, but not processed
  
    // in case of backspace, truncate command buffer
    for (int i = cmdBufferPosition; i <= strlen(cmdBuffer); i++) {
      cmdBuffer[i] = NULL;
    }
    
    handleCommand(cmdBuffer); // process the command
    resetBuffers(); // clear the command buffers
  }
}

// screen routines
void init_screen(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void clear_screen(void) {
    u8g2.clearBuffer();
    commit_screen();
}

void commit_screen(void) {
    u8g2.sendBuffer();
}

void draw_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    u8g2.drawBox(x1, y1, x2, y2);
}

void draw_frame(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    u8g2.drawFrame(x1, y1, x2, y2);
}

void draw_disc(uint8_t x, uint8_t y, uint8_t radius) {
    u8g2.drawDisc(x, y, radius);
}

void draw_circle(uint8_t x, uint8_t y, uint8_t radius) {
    u8g2.drawCircle(x, y, radius);
}

void draw_rounded_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t radius) {
    u8g2.drawRBox(x1, y1, x2, y2, radius);
}

void draw_rounded_frame(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t radius) {
    u8g2.drawRFrame(x1, y1, x2, y2, radius);
}

void draw_text(uint8_t x, uint8_t y, uint8_t direction, char* text) {
    u8g2.setFontDirection(direction);
    u8g2.drawStr(x, y, text);
}

void draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    u8g2.drawLine(x1, y1, x2, y2);
}

void draw_triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3) {
    u8g2.drawTriangle(x1, y1, x2, y2, x3, y3);
}

// serial command interpreter
void handleCommand(char* cmdBuffer) { // handles commands
  
  // init vars
  char *command = NULL;
  char *argument1 = NULL;
  char *argument2 = NULL;
  char *argument3 = NULL;
  char *argument4 = NULL;
  char *argument5 = NULL;
  char *argument6 = NULL;

  char *buf = cmdBuffer;
  
  command = strtok(buf, " "); // first word (command, before space)
  argument1 = strtok(NULL, " "); // second word (argument, after space)
  argument2 = strtok(NULL, " "); // third word (argument, after space)
  argument3 = strtok(NULL, " "); // fourth word (argument, after space)
  argument4 = strtok(NULL, " "); // fifth word (argument, after space)
  argument5 = strtok(NULL, " "); // sixth word (argument, after space)
  argument6 = strtok(NULL, " "); // seventh word (argument, after space)  
  
  // probably a better way to do argument handling so that we can accept
  // multiple words for drawtext, but i'm not that good at c++
  
  Serial.println(""); // newline
  
  // process command
  bool valid = true;
  if (strcasecmp(command, "HELP") == 0) {
    showHelp();    
  } else if (strcasecmp(command, "DRAWBOX") == 0) {
    draw_box(atoi(argument1), atoi(argument2), atoi(argument3), atoi(argument4));
  } else if (strcasecmp(command, "DRAWFRAME") == 0) {
    draw_frame(atoi(argument1), atoi(argument2), atoi(argument3), atoi(argument4));    
  } else if (strcasecmp(command, "DRAWDISC") == 0) {
    draw_disc(atoi(argument1), atoi(argument2), atoi(argument3));
  } else if (strcasecmp(command, "DRAWCIRCLE") == 0) {
    draw_circle(atoi(argument1), atoi(argument2), atoi(argument3));
  } else if (strcasecmp(command, "DRAWROUNDEDBOX") == 0) {
    draw_rounded_box(atoi(argument1), atoi(argument2), atoi(argument3), atoi(argument4), atoi(argument5));
  } else if (strcasecmp(command, "DRAWROUNDEDFRAME") == 0) {
    draw_rounded_frame(atoi(argument1), atoi(argument2), atoi(argument3), atoi(argument4), atoi(argument5));
  } else if (strcasecmp(command, "DRAWLINE") == 0) {
    draw_line(atoi(argument1), atoi(argument2), atoi(argument3), atoi(argument4));
  } else if (strcasecmp(command, "DRAWTRIANGLE") == 0) {
    draw_triangle(atoi(argument1), atoi(argument2), atoi(argument3), atoi(argument4), atoi(argument5), atoi(argument6));
  } else if (strcasecmp(command, "DRAWTEXT") == 0) {
    draw_text(atoi(argument1), atoi(argument2), atoi(argument3), argument4);
    
  } else if (strcasecmp(command, "CLEAR") == 0) {
    clear_screen();
  } else if (strcasecmp(command, "COMMIT") == 0) {
    commit_screen();
    
  } else {
    valid = false;
    Serial.println(F("?"));
  }
  
  if (valid == true) {
    Serial.println(F("-"));
  }

}

// print help information
void showHelp() { // shows command help

    Serial.println(F("##################################"));
    Serial.println(F("## ST7920 LCD drawing interface ##"));
    Serial.println(F("##################################"));
    
    Serial.println(F("\nCcommands:"));
    Serial.println(F("\tHELP - Show this help"));
    Serial.println(F("\tDRAWBOX x1 y1 x2 y2 - Draw filled box"));
    Serial.println(F("\tDRAWFRAME x1 y1 x2 y2 - Draw empty box"));
    Serial.println(F("\tDRAWDISC x y radius - Draw filled cicle"));
    Serial.println(F("\tDRAWCIRCLE x y radius - Draw empty circle"));
    Serial.println(F("\tDRAWROUNDEDBOX x1 y1 x2 y2 radius - Draw filled box with rounded corners"));
    Serial.println(F("\tDRAWROUNDEDFRAME x1 y1 x2 y2 radius - Draw empty box with rounded corners"));
    Serial.println(F("\tDRAWTEXT x y direction text - Draw single word of text. Direction is 0-3, corresponding to 0, 90, 180, 270 degrees"));
    Serial.println(F("\tDRAWLINE x1 y1 x2 y2 - Draw line"));
    Serial.println(F("\tDRAWTRIANGLE x1 y1 x2 y2 x3 y3 - Draw filled triangle"));
    Serial.println(F("\n\tCLEAR - Clear the screen"));
    Serial.println(F("\tCOMMIT - Push drawing to screen"));
}

// reset the serial command buffers
void resetBuffers() { // resets command buffers and command state
  cmdStatus = 0; // reset the processing state
  cmdBufferPosition = 0; // reset the buffer position
  for (int x = 0; x < 32; x++) { // clear the buffer
    cmdBuffer[x] = NULL; // command buffer
    command[x] = NULL; // command
    argument[x] = NULL; // command arguments
  }
  Serial.println("");
  Serial.print(CMDPROMPT);

}
