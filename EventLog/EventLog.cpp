#include "mbed.h"
#include "EventLog/EventLog.h"
#include "EventLog/EventLogEvent.h"
#include <sys/stat.h>
#include <stdio.h>

// eventDir should not have a trailling slash
EventLog::EventLog(const char* eventDir): _eventDir(eventDir), _lastEventId(0) {
    struct stat statBuff;
    FILE* fd;
    
    _ensureDirectoryExists(eventDir);
    
    // TODO error handling for all these sprintfs
    sprintf(_logsNoTimeDir, "%s/logged_no_time", eventDir);
    if (_ensureDirectoryExists(_logsNoTimeDir)) {
        printf("[EventLog::EventLog] Found %s, no need to create\r\n", _logsNoTimeDir);
    } else {
        printf("[EventLog::EventLog] Creating %s\r\n", _logsNoTimeDir);
    }
    
    sprintf(_logsWithTimeDir, "%s/logged_with_time", eventDir);
    if (_ensureDirectoryExists(_logsWithTimeDir)) {
        printf("[EventLog::EventLog] Found %s, no need to create\r\n", _logsWithTimeDir);
    } else {
        printf("[EventLog::EventLog] Creating %s\r\n", _logsWithTimeDir);
    }
    
    sprintf(_syncedLogsDir, "%s/synced", eventDir);
    if (_ensureDirectoryExists(_syncedLogsDir)) {
        printf("[EventLog::EventLog] Found %s, no need to create\r\n", _syncedLogsDir);
    } else {
        printf("[EventLog::EventLog] Creating %s\r\n", _syncedLogsDir);
    }
    
    sprintf(_lastEventIdFile, "%s/last_event_id.txt", eventDir);
    if (stat(_lastEventIdFile, &statBuff) != 0) {
        printf("[EventLog::EventLog] Creating %s\r\n", _lastEventIdFile);
        _syncLastEventId();
        fd = NULL;
    } else {
        printf("[EventLog::EventLog] Found %s, no need to create\r\n", _lastEventIdFile);
        fd = fopen(_lastEventIdFile, "r");
        char eventIdFileBuffer[16];
        size_t bytesRead = fread(eventIdFileBuffer, 1, 16, fd);
        
        if (bytesRead > 0) {
            if (bytesRead < 16) {
                eventIdFileBuffer[bytesRead] = '\0';
            } else {
                printf("[EventLog::EventLog] Not enough room to put in null character\r\n");
            }
            
            sscanf(eventIdFileBuffer, "%u\r\n", &_lastEventId);
            printf("[EventLog::EventLog] Loaded last event ID as %d\r\n", _lastEventId);
        } else {
            printf("[EventLog::EventLog] last event ID appears to be corrupted, sticking with 0\r\n");
        }
        
        fclose(fd);
        fd = NULL;
    }
}

int EventLog::log(EventLogEvent *event) {
    FILE *fd;
    char logBuffer[EVENT_LOG_BUFFER_SIZE];
    int dataLength = event->toString(logBuffer, EVENT_LOG_BUFFER_SIZE);
    
    if (dataLength > EVENT_LOG_BUFFER_SIZE) {
        printf("[EventLog::log] Couldn't fit event into buffer\r\n");
        return -1;
    } else if (dataLength < 0){
        printf("[EventLog::log] Failed to serialize event with error %d\r\n", dataLength);
        return -2;
    }
    
    const char* logDir;
    
    if (event->getTimeValid()) {
        logDir = _logsWithTimeDir;
    } else {
        logDir = _logsNoTimeDir;
    }
    
    _lastEventId++;
    _syncLastEventId();
    
    char logFile[EVENT_LOG_BUFFER_SIZE];
    snprintf(logFile, EVENT_LOG_BUFFER_SIZE, "%s/%u.txt\r\n", logDir, _lastEventId);
    fd = fopen(logFile, "w");
    int dataWritten = fwrite(logBuffer, 1, dataLength, fd);
    fclose(fd);
    
    if (dataWritten != dataLength) {
        printf("[EventLog::log] Failed to write all event data to file\r\n");
        return -3;
    }
    
    return 0;
}

bool EventLog::move(unsigned int eventId, LogState curState, LogState newState) {
    const char* curLogDir;
    const char* newLogDir;
    
    curLogDir = _getCorrectDirectory(curState);
    newLogDir = _getCorrectDirectory(newState);
    
    char curLogFile[EVENT_LOG_BUFFER_SIZE];
    snprintf(curLogFile, EVENT_LOG_BUFFER_SIZE, "%s/%u.txt\r\n", curLogDir, eventId);
    struct stat statBuff;
    if (stat(curLogFile, &statBuff) != 0) {
        printf("[EventLog::move] File %s does not exist\r\n", curLogFile);
        return false;
    }
    
    char newLogFile[EVENT_LOG_BUFFER_SIZE];
    snprintf(newLogFile, EVENT_LOG_BUFFER_SIZE, "%s/%u.txt\r\n", newLogDir, eventId);
    struct stat statBuff;
    if (stat(newLogFile, &statBuff) == 0) {
        printf("[EventLog::move] File %s exists\r\n", newLogFile);
        return false;
    }
    
    int result = rename(curLogFile, newLogFile);
    return result == 0;
}

bool EventLog::_ensureDirectoryExists(const char* dir) {
    struct stat statBuff;
    if (stat(dir, &statBuff) != 0) {
        mkdir(dir, 0777); // TODO assign correct permissions
        return false;
    } else {
        return true;
    }
}

void EventLog::_syncLastEventId() {
    // TODO error handling
    char tempBuff[16];
    FILE *fd;
    int bytesToWrite = snprintf(tempBuff, 16, "%u\r\n", _lastEventId);
    fd = fopen(_lastEventIdFile, "w");
    fwrite(tempBuff, 1, bytesToWrite, fd);
    fclose(fd);
}

const char* EventLog::_getCorrectDirectory(LogState state) {
    switch (state) {
        case LOGGED_NO_TIME:
            return _logsNoTimeDir;
        break;
        
        case LOGGED_WITH_TIME:
            return _logsWithTimeDir;
        break;
        
        case SYNCED:
            return _syncedLogsDir;
        break;
    }
}