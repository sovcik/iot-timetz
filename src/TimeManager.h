#ifndef __TIMEMANAGER_H__
#define __TIMEMANAGER_H__

class TimeManager {
    public:
        virtual int setTimeISO(const char* isoTime)=0;
        virtual void setTimeZone(const char *tzOffset)=0;  // offset as e.g. "+01:00" or just "Z"
        virtual void setTimeZone(char tzSig, unsigned char tzHr, unsigned char tzMin)=0;
};

#endif