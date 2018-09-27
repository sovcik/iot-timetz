#include "TimeTZ.h"

#if DEBUG_TIMETZ != 1
#define NODEBUG_PRINT
#endif

#include <debug_print.h>

#define TIME_STRING_SIZE        30

TimeTZ::TimeTZ(){
    _updateInterval = 30000; // 15min by default
    _lastUpdate = millis();
    _tzSig = '+';
    _tzHr = 0;
    _tzMin = 0;
    _tzSeconds = 0;
    _timeStore = NULL;
}

int TimeTZ::setTimeISO(const char *isoTime, char forceTZ){

    int yy, mm, dd, hr, min, sec;
    char tzSig;
    uint8_t tzHr, tzMin;

    DEBUG_PRINT("[TimeTZ:setTime] input=%s\n",isoTime);

    //int match = sscanf(isoTime,"%4u-%2hhu-%2hhuT%2u:%2hhu:%2hhu%s
    //TODO: not sure why it does not work direct parsing to unsigned char variables...
    int match = sscanf(isoTime,"%4u-%2u-%2uT%2u:%2u:%2u", &yy, &mm, &dd, &hr, &min, &sec);

    if (match < 6) {
        DEBUG_PRINT("[TimeTZ:setTime] failed to parse datetime string: par=%d\n",match+1);
        return 0;
    }

    DEBUG_PRINT("[TimeTZ:setTime] elements: date=%d-%d-%d time=%d:%d:%d\n", yy, mm, dd, hr, min, sec);

    // check if provided time is in the same timezone as set timezone, if not adjust time accordingly
    if (getTZOffset(isoTime, &tzSig, &tzHr, &tzMin) ){

        setTime(hr, min, sec, dd, mm, yy);       
        
        if (forceTZ){ // time zone will be set to provided time zone
            setTimeZone(tzSig, tzHr, tzMin);

        } else { // time zone will be kept unchanged, time will be adjusted accordingly
            long a = (tzSig=='+'?1:-1)*(tzHr*3600+tzMin*60);
            if (a != _tzSeconds) { // if different timezone
                a = _tzSeconds - a;
                adjustTime(a);
            }
        }
        
    } else {
        DEBUG_PRINT("[TimeTZ:setTime] failed to extract timezone\n");
        return 0;
    }

    return 1;

}

int TimeTZ::getTZOffset(const char* isoTime, char *tzSig, uint8_t *tzHr, uint8_t *tzMin){
    
    int hr, min;
    char sig;

    if (!isoTime || strlen(isoTime) < 6) return 0;
    
    if (*(isoTime+strlen(isoTime)-1) == 'Z'){
        *tzSig = '+';
        *tzHr = 0;
        *tzMin = 0;
        return 1;
    } 

    if (sscanf(isoTime+strlen(isoTime)-6,"%c%2u:%2u", &sig, &hr, &min) < 3){
        DEBUG_PRINT("[TimeTZ:getTZOff] failed parsing input string\n");
        return 0;
    }

    *tzSig = sig;
    *tzHr = hr;
    *tzMin = min;

    return 1;
}

void TimeTZ::timeToISO(char *buff, size_t size){
    // buff has to be at least 26 characters long in order to store time + termination zero
    const char* fmt;
    time_t n = now();
    if (_tzHr || _tzMin) {
        fmt = "%4d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d";
    } else {
        fmt = "%4d-%02d-%02dT%02d:%02d:%02dZ";
    }
    snprintf(buff, size, fmt, year(n), month(n), day(n), hour(n), minute(n), second(n), _tzSig, _tzHr, _tzMin);
    buff[size-1] = 0;

}

void TimeTZ::setTimeZone(const char* tzOffset) {
    char sig = '+';
    int hr=0, min=0;

    if (!tzOffset) return;
    if (*tzOffset != 'Z' && sscanf(tzOffset,"%c%2u:%2u", &sig, &hr, &min) < 3){
        DEBUG_PRINT("[TimeTZ:setTZ1] failed parsing input string\n");
        return;
    }
    setTimeZone(sig, hr, min);
}

void TimeTZ::setTimeZone(char tzSig, uint8_t tzHr, uint8_t tzMin){
    _tzSig = tzSig;
    _tzHr = (uint8_t)tzHr;
    _tzMin = (uint8_t)tzMin;
    _tzSeconds = (tzSig=='+'?1:-1)*(tzHr*3600+tzMin*60);
    DEBUG_PRINT("[TimeTZ:setTZ2] timezone set seconds=%d\n",_tzSeconds);

}

void TimeTZ::setTimeStore(TimeStore *ts){
    DEBUG_PRINT("[TimeTZ:setStore] store=%lux\n",ts);
    _timeStore = ts;
}

uint8_t TimeTZ::writeTime(){
    if (!_timeStore) {
        DEBUG_PRINT("[TimeTZ:write] no time store set\n");
        return 0;
    }
    char buf[TIME_STRING_SIZE];
    memset(buf,0,TIME_STRING_SIZE);
    timeToISO(&buf[0], TIME_STRING_SIZE);
    DEBUG_PRINT("[TimeTZ:write] writing time=%s\n",&buf[0]);
    return _timeStore->write(&buf[0],TIME_STRING_SIZE);
}

uint8_t TimeTZ::loadTime(char forceTZ){
    if (!_timeStore) {
        DEBUG_PRINT("[TimeTZ:load] no time store set\n");
        return 0;
    }
    char buf[TIME_STRING_SIZE];
    memset(buf,0,TIME_STRING_SIZE);
    uint8_t ret = _timeStore->read(&buf[0],TIME_STRING_SIZE);
    DEBUG_PRINT("[TimeTZ:load] loaded ret=%d, time=%s\n",ret, &buf[0]);
    if (ret)
        setTimeISO(&buf[0], forceTZ);
    return ret;
}

void TimeTZ::loop(){
    if (millis()-_lastUpdate > _updateInterval){
        DEBUG_PRINT("[TimeTZ:loop] alive\n");
        _lastUpdate = millis();
        if (!writeTime())
            DEBUG_PRINT("[TimeTZ:loop] writing time failed");
    }
};