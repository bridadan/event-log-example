#include "EventLog/EventLogEvent.h"
#include <time.h>
#include <stdio.h>

const unsigned int EventLogEvent::NUM_DATA_POINTS = 2;

// Create an empty event
EventLogEvent::EventLogEvent(): _time(0), _value(0) {
    
}

// Create an event with a known value and use the current time
EventLogEvent::EventLogEvent(int value): _value(value) {
    time(&_time);
}

// Create and event given a known value and time
EventLogEvent::EventLogEvent(time_t time, int value): _time(time), _value(value) {

}

// Create an event from a C string
bool EventLogEvent::loadFromString(char* str) {
    // TODO may need to include the new line character here "\n"
    int result = sscanf(str, "%u,%d", _time, _value);
    return result == 2;
}

// Serialize the event to a C string
int EventLogEvent::toString(char* buffer, size_t size) {
    return snprintf(buffer, size, "%u,%d\n", _time, _value);
}

void EventLogEvent::setTime(time_t time) {
    _time = time;
}

void EventLogEvent::setTimeValid(bool timeValid) {
    _timeValid = timeValid;
}

void EventLogEvent::setValue(int value) {
    _value = value;
}

time_t EventLogEvent::getTime() {
    return _time;
}

bool EventLogEvent::getTimeValid() {
    return _timeValid;
}

int EventLogEvent::getValue() {
    return _value;
}
