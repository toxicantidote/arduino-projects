/*
  This is a quick demo showing how you can index and get messages from
  a file on an SD card with an Arduino. Useful for when you have a lot
  of static strings to display, but don't have the memory to store it.
  
  Assumes that the SD card is FAT16 or FAT32 formatted, and that the
  file is in the root directory of the SD card and is named
  STRINGS.TXT.
  
  An example STRINGS.TXT looks like:
01First message
02Something else
03Blah blah
  
  Message numbers don't have to be sequential. There is no error
  handling.
  
  Toxicantidote.net - January 2021
*/

void getMessage(String search, char* target) {
  // open the file from the SD card. assumes the filename is STRINGS.TXT
  File dataFile = SD.open("STRINGS.TXT");
  // allow up to two characters for the message identifier
  char code[2];
  // allow up to 32 characters for the message
  char message[32];

  // if the file is available...
  if (dataFile) {
    // while data is available from the file
    while (dataFile.available()) {
      // get data until a newline character is encountered. note this is a LINUX newline (\n)
      // not a Windows newline (\r\n)
      String str = dataFile.readStringUntil('\n');

      // Get the first two characters on the line
      str.substring(0, 3).toCharArray(code, 3);

      // if those two characters match our target string
      if (search == code) {
        // get the remaing text on the line, up to 32 characters long
        str.substring(2).toCharArray(message, 32);
        // break out of the read loop - we have found what we need
        break;
      }
      
    }
    // close the file
    dataFile.close();
  }
  // return the message
  return message;
}
