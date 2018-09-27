#ifndef __TIMETZ_H__
#define __TIMETZ_H__

#include <Arduino.h>
#include <TimeLib.h>
#include <TimeStore.h>
#include <TimeManager.h>
#include <Looper.h>

class TimeTZ : public Looper, public TimeManager {
    protected:
        char _tzSig;
        uint8_t _tzHr;
        uint8_t _tzMin;

        long _tzSeconds; // calculated time zone offset in seconds

        TimeStore *_timeStore;
        unsigned long _updateInterval;
        unsigned long _lastUpdate;

    public:
        TimeTZ();
        void setTimeZone(const char *tzOffset) override;  // offset as e.g. "+01:00" or just "Z"
        void setTimeZone(char tzSig, uint8_t tzHr, uint8_t tzMin) override;
        int setTimeISO(const char *isoTime, char forceTZ = 0) override;
        int getTZOffset(const char* isoTime, char *tzSig, uint8_t *tzHr, uint8_t *tzMin);
        void timeToISO(char *buff, size_t size);

        void setTimeStore(TimeStore *ts);

        uint8_t writeTime();
        uint8_t loadTime(char forceTZ = 0);

        void loop() override;

};


#endif