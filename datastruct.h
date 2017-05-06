#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <windows.h>
#include <vector>

#define ARRAY_CHAR 10
#define SIZE_CHAR_INFO_WIDTH 100
#define SIZE_CHAR_INFO_LENGTH 100
#define SIZE_INPUT_RECORDS 40

struct DataOut
{
    SMALL_RECT srctReadRect = {0};
    COORD position = {0};
    COORD size = {0};
    std::vector<CHAR_INFO> charInfos;
};

struct DataIn
{
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = {};
    bool windowChanged;
    INPUT_RECORD inputRecord;
};

#endif // DATASTRUCT_H
