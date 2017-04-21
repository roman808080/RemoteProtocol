#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <windows.h>

#define ARRAY_CHAR 10
#define SIZE_CHAR_INFO_WIDTH 100
#define SIZE_CHAR_INFO_LENGTH 100
#define SIZE_INPUT_RECORDS 40

struct DataOut
{
    char array[ARRAY_CHAR] = {};
    SMALL_RECT srctReadRect = {0};
    COORD position = {0};
    CHAR_INFO charInfos[SIZE_CHAR_INFO_LENGTH * SIZE_CHAR_INFO_WIDTH] = {};
};

struct DataIn
{
    SMALL_RECT srctWriteRect = {0};
    INPUT_RECORD inputRecords[SIZE_INPUT_RECORDS] = {};
    char array[ARRAY_CHAR] = {};
};

#endif // DATASTRUCT_H
