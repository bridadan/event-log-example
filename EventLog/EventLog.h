#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <stdio.h>

#define EVENT_LOG_BUFFER_SIZE 64

struct_t EventLogEntry {
    Event event;
};

class EventLog {
public:
    // This assumes the filesystem is mounted and ready to go!
    EventLog(const char* eventDir);
    
    // Log the event to the filesystem
    // Note: this is synchronous, so event must not go out of scope until the
    // function returns
    int log(Event *event);


private:
    fpos_t _readPosition;
    const char* _logFile;
    onst char* _syncFile;
    FILE* _fd;
    
    char _logBuffer[EVENT_LOG_BUFFER_SIZE];
    
    // Starting from the last point in the log file, return an event
    // The result will be placed in `event`
    // Will return `true` if the log is unread
    bool _getEvent(Event *event);
    
};

#endif