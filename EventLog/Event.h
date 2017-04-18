#ifndef EVENT_H
#define EVENT_H

#include <time.h>

class Event {
public:
    // Get the time and log the data;
    Event();
    Event(int value);
    Event(time_t time, int value);
    
    bool loadFromString(char* str);
    int toString(char* buffer, size_t size);
    
    void setTime(time_t time);
    void setTimeValid(bool timeValid);
    void setValue(int value);
    
    time_t getTime();
    bool getTimeValid();
    int getValue();
    
    static const unsigned int NUM_DATA_POINTS;

private:
    time_t _time;
    bool _timeValid;
    int _value;    
};

#endif