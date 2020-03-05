#include "TimeStoreSPIFFS.h"

#include <TimeLib.h>

#if DEBUG_TIMETZ != 1
#define NODEBUG_PRINT
#endif

#ifdef ESP32
#include <SPIFFS.h>
#endif

#include <debug_print.h>

TimeStoreSPIFFS::~TimeStoreSPIFFS(){
    stop();
}

void TimeStoreSPIFFS::begin(const char *fileName){
    const char* mode;
    DEBUG_PRINT("[timeSPF:begin] starting SPIFFS time file=%s\n", fileName);

    if (SPIFFS.exists(fileName))
        mode = "r+";
    else 
        mode = "w+";

    _timeFile = SPIFFS.open(fileName, mode);
    if (!_timeFile) {
        DEBUG_PRINT("[timeSPF:begin] failed opening time file\n");
        return;
    }

    _available = 1;

}

void TimeStoreSPIFFS::stop(){
    _timeFile.close();
    _available = 0;
}

uint8_t TimeStoreSPIFFS::read(void *buff, size_t size) {
    DEBUG_PRINT("[timeSPF:read] going to read\n");
    if (!_available) return 0;

    _timeFile.seek(0, SeekSet);
    bool ret = _timeFile.read((uint8_t*)buff, size) == size;
    DEBUG_PRINT("[timeSPF:read] read time ret=%d\n",ret);

    return ret;
}

uint8_t TimeStoreSPIFFS::write(void *buff, size_t size) {
    DEBUG_PRINT("[timeSPF:write] going to write %s\n",(char*)buff);
    if (!_available) return 0;

    _timeFile.seek(0, SeekSet);
    bool ret = _timeFile.write((uint8_t*)buff, size) == size;
    _timeFile.flush();
    DEBUG_PRINT("[timeSPF:write] saved time ret=%d\n",ret);

    return ret;
}