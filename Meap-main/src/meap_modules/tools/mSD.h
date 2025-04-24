#ifndef MSD_H_
#define MSD_H_

#include "FS.h"
#include "SD.h"
#include "SPI.h"

class mSD
{
public:
    mSD() {

    };

    void init(int32_t SD_SCK, int32_t SD_CIPO, int32_t SD_COPI, int32_t SD_CS)
    {
        SPI.begin(SD_SCK, SD_CIPO, SD_COPI, SD_CS);

        if (!SD.begin(SD_CS))
        {
            Serial.println("Card Mount Failed");
            return;
        }
        uint8_t cardType = SD.cardType();

        if (cardType == CARD_NONE)
        {
            Serial.println("No SD card attached");
            return;
        }

        Serial.print("SD Card Type: ");
        if (cardType == CARD_MMC)
        {
            Serial.println("MMC");
        }
        else if (cardType == CARD_SD)
        {
            Serial.println("SDSC");
        }
        else if (cardType == CARD_SDHC)
        {
            Serial.println("SDHC");
        }
        else
        {
            Serial.println("UNKNOWN");
        }
    };

    void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
    {
        Serial.printf("Listing directory: %s\n", dirname);

        File root = fs.open(dirname);
        if (!root)
        {
            Serial.println("Failed to open directory");
            return;
        }
        if (!root.isDirectory())
        {
            Serial.println("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file)
        {
            if (file.isDirectory())
            {
                Serial.print("  DIR : ");
                Serial.println(file.name());
                if (levels)
                {
                    listDir(fs, file.path(), levels - 1);
                }
            }
            else
            {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("  SIZE: ");
                Serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }

    void createDir(fs::FS &fs, const char *path)
    {
        Serial.printf("Creating Dir: %s\n", path);
        if (fs.mkdir(path))
        {
            Serial.println("Dir created");
        }
        else
        {
            Serial.println("mkdir failed");
        }
    }

    void removeDir(fs::FS &fs, const char *path)
    {
        Serial.printf("Removing Dir: %s\n", path);
        if (fs.rmdir(path))
        {
            Serial.println("Dir removed");
        }
        else
        {
            Serial.println("rmdir failed");
        }
    }

    void readFile(fs::FS &fs, const char *path)
    {
        Serial.printf("Reading file: %s\n", path);

        File file = fs.open(path);
        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return;
        }

        Serial.print("Read from file: ");
        while (file.available())
        {
            Serial.write(file.read());
        }
        file.close();
    }

    void writeFile(fs::FS &fs, const char *path, const char *message)
    {
        Serial.printf("Writing file: %s\n", path);

        File file = fs.open(path, FILE_WRITE);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            return;
        }
        if (file.print(message))
        {
            Serial.println("File written");
        }
        else
        {
            Serial.println("Write failed");
        }
        file.close();
    }

    void appendFile(fs::FS &fs, const char *path, const char *message)
    {
        Serial.printf("Appending to file: %s\n", path);

        File file = fs.open(path, FILE_APPEND);
        if (!file)
        {
            Serial.println("Failed to open file for appending");
            return;
        }
        if (file.print(message))
        {
            Serial.println("Message appended");
        }
        else
        {
            Serial.println("Append failed");
        }
        file.close();
    }

    void renameFile(fs::FS &fs, const char *path1, const char *path2)
    {
        Serial.printf("Renaming file %s to %s\n", path1, path2);
        if (fs.rename(path1, path2))
        {
            Serial.println("File renamed");
        }
        else
        {
            Serial.println("Rename failed");
        }
    }

    void deleteFile(fs::FS &fs, const char *path)
    {
        Serial.printf("Deleting file: %s\n", path);
        if (fs.remove(path))
        {
            Serial.println("File deleted");
        }
        else
        {
            Serial.println("Delete failed");
        }
    }

    // stores a 16-bit buffer to sd card
    void storeBuffer16(int32_t *buffer_address, int32_t buffer_size, const char *path)
    {
        SD.remove(path);

        File file = SD.open(path, FILE_WRITE);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            return;
        }
        int32_t i = 0;
        int8_t low_byte;
        int8_t high_byte;
        while (i < buffer_size)
        {
            low_byte = buffer_address[i] & 0xFF;
            high_byte = (buffer_address[i++] >> 8) & 0xFF;
            file.write(low_byte);
            file.write(high_byte);
        }
        file.close();
    }

    // loads a 16-bit buffer from sd card
    void loadBuffer16(int32_t *buffer_destination, int32_t buffer_size, const char *path)
    {

        File file = SD.open(path);
        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return;
        }
        int i = 0;
        int8_t low_byte;
        int8_t high_byte;
        while (i < buffer_size)
        {
            low_byte = file.read();
            high_byte = file.read();
            buffer_destination[i++] = low_byte + (high_byte << 8);
        }
        file.close();
    }

    void clearBuffer(const char *path)
    {
        SD.remove(path);
    }
};

#endif // MSD_H_