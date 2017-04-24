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
//    CHAR_INFO charInfos[SIZE_CHAR_INFO_LENGTH * SIZE_CHAR_INFO_WIDTH] = {};
    std::vector<CHAR_INFO> charInfos;
};

struct DataIn
{
    SMALL_RECT srctWriteRect = {0};
    std::vector<INPUT_RECORD> inputRecords;
};

#endif // DATASTRUCT_H
