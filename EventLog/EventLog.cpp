#include "mbed.h"
#include "EventLog.h"
#include "Event.h"
#include <sys/stat.h>

EventLog::EventLog(const char* eventDir): _eventDir(eventDir) {
    struct stat statBuff;
    if (stat(_eventDir, &statBuff) != 0) {
        mkdir(eventDir, 0777); // TODO assign correct permissions
    }
    
    if (stat)
}

bool EventLog::log(Event *event) {
    bool success = true;
    int length = event->toString(_logFile, EVENT_LOG_BUFFER_SIZE);
    
    if (length > EVENT_LOG_BUFFER_SIZE) {
        printf("[EventLog] Dropping event due to size. Size: %d, Max: %d\r\n", length, EVENT_LOG_BUFFER_SIZE);
        success = false;
    } else {
        _fd = fopen(_logFile, "a");
        
        if (_fd) {
            int writtenBytes = fwrite(_logBuffer, 1, length, _fd);
            
            if (writtenBytes != stringLength) {
                printf("Expected to write %d bytes, but actually wrote %d bytes\r\n", stringLength, writtenBytes);
                success = false;
            }
            
            if (fclose(_fd) < 0) {
                printf("Failed to close '/fs/events.txt'\r\n");
                success = false;
            } else {
                _fd = NULL;
            }
        } else {
            printf("Failed to open '/fs/events.txt'\r\n");
            success = false;
        }
    }
    
    return success;
}

bool EventLog::read(Event *event) {
    
}

bool EventLog::_getEvent(Event *event) {
    // Open the log file
    _fd = fopen(_logFile, "r");
    
    if (_fd) {
        // Resume our place in the logfile
        fsetpos(_fd, _readPosition);
        int writtenBytes = fwrite(_logBuffer, 1, length, _fd);
        
        if (writtenBytes != stringLength) {
            printf("[EventLog] Expected to write %d bytes, but actually wrote %d bytes\r\n", stringLength, writtenBytes);
            success = false;
        }
        
        if (fclose(_fd) < 0) {
            printf("[EventLog] Failed to close logFile\r\n");
            success = false;
        }
    } else {
        printf("[EventLog] Failed to open logFile\r\n");
        success = false;
    }
}