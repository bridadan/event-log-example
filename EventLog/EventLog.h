#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include "EventLog/EventLogEvent.h"
#include <stdio.h>

#define EVENT_LOG_BUFFER_SIZE 64

class EventLog {
public:
    
    enum LogState {
        LOGGED_NO_TIME,
        LOGGED_WITH_TIME,
        SYNCED
    };
    
    // This assumes the filesystem is mounted and ready to go!
    EventLog(const char* eventDir);
    
    // Log the event to the filesystem
    // Note: this is synchronous, so event must not go out of scope until the
    // function returns
    int log(EventLogEvent *event);
    
    bool move(unsigned int eventId, LogState curState, LogState newState);


private:
    const char* _eventDir;
    unsigned int _lastEventId;
    
    // TODO look at tuning these buffers or making them static
    // All directories
    char _logsNoTimeDir[EVENT_LOG_BUFFER_SIZE];
    char _logsWithTimeDir[EVENT_LOG_BUFFER_SIZE];
    char _syncedLogsDir[EVENT_LOG_BUFFER_SIZE];
    
    // All files
    char _lastEventIdFile[EVENT_LOG_BUFFER_SIZE];
    
    // Starting from the last point in the log file, return an event
    // The result will be placed in `event`
    // Will return `true` if the log is unread
    //bool _getEvent(Event *event);
    
    bool _ensureDirectoryExists(const char* dir);
    
    void _syncLastEventId();
    
    const char* _getCorrectDirectory(LogState state);
    
};

#endif