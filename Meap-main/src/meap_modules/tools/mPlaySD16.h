#ifndef MPLAYSD16_H_
#define MPLAYSD16_H_

#include "FS.h"
#include "SD.h"
#include "SPI.h"

class mPlaySD16
{
public:
    mPlaySD16()
    {

        _file_open = false;
    };

    void open(char *path)
    {
        _path = path;
        file = SD.open(_path);
        _file_open = true;

        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return;
        }
    }

    int16_t next()
    {
        if (file.available())
        {
            // low_byte;
            // high_byte;
            low_byte = file.read();
            high_byte = file.read();
            return low_byte + (high_byte << 8);
        }
        else
        {
            return 0;
        }
    }

    File file;
    bool _file_open;
    int8_t low_byte;
    int8_t high_byte;
    char *_path;
};

#endif // MPLAYSD16_H_