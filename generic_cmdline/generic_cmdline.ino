// Command prompt
#define CMDPROMPT "Prompt> "

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

// setup function
void setup() {	
  Serial.begin(9600);
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
		if (cmdByte >= 0x20	 && cmdByte <= 0x7E) {
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

// serial command interpreter
void handleCommand(char* cmdBuffer) { // handles commands
  
  // init vars
  char *command = NULL;
  char *argument = NULL;
  // Repeat to initialise for extra arguments
  /*
  char *argument2 = NULL;
  char *argument3 = NULL;
  char *argument4 = NULL;
  char *argument5 = NULL;
  */

  char *buf = cmdBuffer;
  
  command = strtok(buf, " "); // first word (command, before space)
  argument = strtok(NULL, " "); // second word (argument, after space)
  // we can keep repeating the same command to get additional arguments
  /*
  argument2 = strtok(NULL, " "); // third word (argument, after space)
  argument3 = strtok(NULL, " "); // fourth word (argument, after space)
  argument4 = strtok(NULL, " "); // fifth word (argument, after space)
  argument5 = strtok(NULL, " "); // sixth word (argument, after space)
  */
  
  Serial.println(""); // newline
  
  // process command
  if (strcasecmp(command, "HELP") == 0) {
	showHelp();	   
  } else if (strcasecmp(command, "EXAMPLE1") == 0) {
	Serial.println("Example one");
  } else if (strcasecmp(command, "EXAMPLE2") == 0) {
	Serial.println("Example two");
	if (strcasecmp(argument, "arg1") == 0) {
		Serial.println("..with argument one");
	} else if (strcasecmp(argument, "arg2") == 0) {
		Serial.println("..with argument two");
	} else {
		Serial.println("..with an unknown argument");
	}		
  }
  
  Serial.println("Command completed!");
}

// print help information
void showHelp() { // shows command help

	Serial.println("##################");
	Serial.println("## Command help ##");
	Serial.println("##################");
	
	Serial.println("\nGeneral commands:");
	Serial.println("\tHELP - Show this help");
	Serial.println("\tEXAMPLE1 - Example command");
	Serial.println("\tEXAMPLE2 <arg1/arg2> - Example command with arguments");
	
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
  if (echoInput == 1) { // if echo is enabled, print a prompt
	Serial.println("");
	Serial.print(CMDPROMPT);
  }
}
