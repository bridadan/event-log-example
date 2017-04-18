#include "Event.h"
#include <time.h>

const unsigned int Event::NUM_DATA_POINTS = 2;

// Create an empty event
Event::Event(): _time(0), _value(0) {
    
}

// Create an event with a known value and use the current time
Event::Event(int value): _value(value) {
    time(&_time);
}

// Create and event given a known value and time
Event::Event(time_t time, int value): _time(time), _value(value) {

}

// Create an event from a C string
bool Event::loadFromString(char* str) {
    // TODO may need to include the new line character here "\n"
    int result = sscanf(str, "%u,%d", _time, _value);
    return result == 2;
}

// Serialize the event to a C string
int Event::toString(char* buffer, size_t size) {
    return snprintf(buffer, size, "%u,%d\n", _time, _value);
}

void Event::setTime(time_t time) {
    _time = time;
}

void Event::setTimeValid(bool timeValid) {
    _timeValid = timeValid;
}

void Event::setValue(int value) {
    _value = value;
}

time_t Event::getTime(time_t baseTime = 0) {
    return _time;
}

bool Event::getTimeValid() {
    return _timeValid;
}

int Event::getValue() {
    return _value;
}
