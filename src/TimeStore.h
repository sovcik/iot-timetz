#ifndef __TIMESTORE_H__
#define __TIMESTORE_H__

#include <Arduino.h>
#include <Looper.h>

class TimeStore {
    protected:
        uint8_t _available = 0;

    public:
        virtual void begin(){_available=1;};
        virtual uint8_t write(void *buff, size_t size)=0;
        virtual uint8_t read(void *buff, size_t size)=0;
        uint8_t available() {return _available;};
        virtual void stop(){_available=0;};
};

#endif