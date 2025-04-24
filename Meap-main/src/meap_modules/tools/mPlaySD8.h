#ifndef MPLAYSD8_H_
#define MPLAYSD8_H_

#include "FS.h"
#include "SD.h"
#include "SPI.h"

class mPlaySD8
{
public:
    mPlaySD8()
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

    int8_t next()
    {
        if (file.available())
        {
            return file.read();
        }
        else
        {
            return 0;
        }
    }

    File file;
    bool _file_open;
    char *_path;
};

#endif // MPLAYSD8_H_