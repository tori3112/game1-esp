//
// Created by wiktoria on 31/01/24.
//

#ifndef GAME_LOGGER_H
#define GAME_LOGGER_H

/**
 * creates a time stamp for the message
 */
 struct tm* getTime();

 /**
  * opens and writes log entries to a log file,
  * supposed to help with debugging
  */
 void logMessage(char* message);

#endif //GAME_LOGGER_H
