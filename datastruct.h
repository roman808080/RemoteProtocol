#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <windows.h>
#include <vector>

#define ARRAY_CHAR 10
#define SIZE_CHAR_INFO_WIDTH 100
#define SIZE_CHAR_INFO_LENGTH 100
#define SIZE_INPUT_RECORDS 40

struct DataStaticOut
{
    SMALL_RECT srctReadRect = {};
    COORD position = {};
    COORD size = {};

    COORD writeFrom = {};
    COORD writeTo {};
    bool changedCharInfos;
};

struct DataStaticIn
{
    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = {};
    bool windowChanged;
};

struct DataOut
{
    DataStaticOut st;
    std::vector<CHAR_INFO> charInfos;
};

struct DataIn
{
    DataStaticIn st;
    std::vector<INPUT_RECORD> inputRecords;
};

#endif // DATASTRUCT_H
