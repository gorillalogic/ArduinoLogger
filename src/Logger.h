/**
 * Copyright (c) 2020 Farès Chati
 * This file id part of the ArduinoLogger library.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <TimeLib.h>
#include <ostream>

// If you enable these features, you need to have SdCard library installed
#if defined(LOGGER_SD_FEATURES)
# include "SdCard.h"
#endif

#define LOG_LEVEL_SILENT  0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_TRACE   4
#define LOG_LEVEL_VERBOSE 5

typedef struct LogOutput
{
	Print * stream;
	uint8_t level;
	bool    prefixOnNextPrint;
	bool    prefixEnabled;
	bool    dateEnabled;
	bool    levelNameEnabled;
	bool    enabled;
} LogOutput;

class Logger : public ostream
{
public:
	Logger();
	Logger(uint8_t levelToOutput);

	// Add an output
	void add (Print & stream, uint8_t level,
	  bool prefixEnabled    = true,
	  bool dateEnabled      = true,
	  bool levelNameEnabled = true) const;

	// Modify the properties of an output
	void edit (Print & stream, uint8_t level,
	  bool prefixEnabled    = true,
	  bool dateEnabled      = true,
	  bool levelNameEnabled = true) const;

	void enable (Print & stream) const;           // Enable an output
	void enablePrefix (Print & stream) const;     // Enable prefix
	void enableDate (Print & stream) const;       // Enable date in prefix
	void enableLevelName (Print & stream) const;  // Enable level name in prefix
	void disable (Print & stream) const;          // Disable an output
	void disablePrefix (Print & stream) const;    // Disable prefix
	void disableDate (Print & stream) const;      // Disable date in prefix
	void disableLevelName (Print & stream) const; // Disable level name in prefix

	// Is the output enabled for the specified log level ?
	bool isEnabled (Print & stream, int level = LOG_LEVEL_SILENT) const;

	friend Logger &operator << (ostream & s, Logger& (*pf)(Logger & logger));
	friend inline Logger& endl (Logger& logger);  /* End of line */
	friend inline Logger& dendl (Logger& logger); /* Double end of line */
	friend inline Logger& np (Logger& logger);    /* Do not display prefix for the current line */

private:
	void putch (char c);
	void putstr (const char * str);
	bool seekoff (off_type off, seekdir way);
	bool seekpos (pos_type pos);
	bool sync ();
	pos_type tellpos ();
	void setflags ();

	String clock ();
	LogOutput * getLogOutputFromStream (Print & stream) const;
	void initLogOutput (LogOutput * output, Print & stream, uint8_t level,
	  bool prefixEnabled,
	  bool dateEnabled,
	  bool levelNameEnabled) const;

	const char * debugLevelName (uint8_t debugLevel);
	void printPrefix (uint8_t index);
	void setPrefixOnNextPrint (bool prefixOnNextPrint) const;

	const uint8_t _levelToOutput; // The level that needs to be output by the instance

	static LogOutput * _outputs; // Ouputs array
	static uint8_t _nOutputs;    // Outputs counter
};

inline Logger& endl (Logger& logger)
{
	logger.put ('\n');
	logger.setPrefixOnNextPrint (true);
	logger.setflags();

	return logger;
}

inline Logger& dendl (Logger& logger)
{
	logger.put ('\n');
	logger.put ('\n');
	logger.setPrefixOnNextPrint (true);
	logger.setflags();

	return logger;
}

/*
 * Do not display prefix for the current line
 * Must be put in second position
 */
inline Logger& np (Logger& logger)
{
	logger.setPrefixOnNextPrint (false);

	return logger;
}

extern Logger err;   // Error level logging
extern Logger warn;  // Warning level logging
extern Logger inf;   // Info level logging
extern Logger trace; // Trace level logging
extern Logger verb;  // Verbose level logging
extern const Logger logger;

#endif // ifndef LOGGER_H