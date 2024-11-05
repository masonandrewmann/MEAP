#ifndef MEVENTDELAYMICROS_H_
#define MEVENTDELAYMICROS_H_

/*
 * mEventDelayMicros.h
 *


/**
mEventDelayMicros can be set() to a number of microseconds, then after calling start(), ready() will return true when the time is up.
Alternatively, start(microseconds) will call set() and start() together.
*/
class mEventDelayMicros
{

public:
    /** Constructor.
    Declare an mEventDelayMicros object.
    @param delay_milliseconds how long until ready() returns true, after calling start().  Defaults to 0 if no parameter is supplied.
    */
    mEventDelayMicros(uint64_t delay_microseconds = 0)
    {
        set(delay_microseconds);
    }

    /** Set the delay time.  This setting is persistent, until you change it by using set() again.
    @param delay_microseconds delay time in microseconds.
    */
    inline void set(uint64_t delay_microseconds)
    {
        delay_time = delay_microseconds;
    }

    /** Start the delay.
     */
    inline void start()
    {
        deadline = micros() + delay_time;
    }

    /** Set the delay time and start the delay.
    @param delay_microseconds delay time in microseconds.
    */
    inline void start(uint64_t delay_microseconds)
    {
        set(delay_microseconds);
        start();
    }

    /** Call this in loop() or updateControl() or updateAudio() to check if the delay time is up.
    @return true if the time is up.
    */
    inline bool ready()
    {
        return (micros() > deadline);
    }

protected:
    uint64_t deadline;
    uint64_t delay_time;
};

#endif /* MEVENTDELAYMICROS_H_ */