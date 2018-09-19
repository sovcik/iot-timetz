#ifndef __TIMESPIFFS_H__
#define __TIMESPIFFS_H__

#include <FS.h>
#include <TimeStore.h>
#include <Looper.h>

class TimeStoreSPIFFS : public TimeStore {
    protected:
        File _timeFile;
    
    public:
        ~TimeStoreSPIFFS();
        void begin(const char *fileName);
        uint8_t write(void *buff, size_t size) override;
        uint8_t read(void *buff, size_t size) override;
        void stop() override;

};


#endif