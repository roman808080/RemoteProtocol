#ifndef INPUTCONSOLE_H
#define INPUTCONSOLE_H
#include <iostream>
#include <windows.h>

INPUT_RECORD *wchars2records(std::__cxx11::wstring str);
int locale_to_unicode(const std::string str, wchar_t* wstr);
INPUT_RECORD wchar2record(wchar_t u_char);
INPUT_RECORD setEnter();

const int CODE_PAGE = 1251;

void wchars2records(std::wstring wstr, INPUT_RECORD *input_records)
{
//    wchar_t *wstr = new wchar_t;
//    int size = locale_to_unicode(str, wstr);
    int size = sizeof(input_records)/sizeof(input_records[0]);///////
//    INPUT_RECORD* input_records = new INPUT_RECORD[size+1];
    for(int i=0; i<wstr.size(); i++)
        input_records[i] = wchar2record(wstr[i]);
    input_records[wstr.size()] = setEnter();

//    return input_records;
}

int locale_to_unicode(const std::string str, wchar_t* wstr)
{
    int result_u;
    result_u = MultiByteToWideChar(CODE_PAGE, 0, str.c_str(), -1, 0, 0);
    if (!result_u)
           return 0;

    delete[] wstr;

    wstr = new wchar_t[result_u];
    if(!MultiByteToWideChar(CODE_PAGE, 0, str.c_str(), -1, wstr, result_u))
       return 0;

    return str.size();
}

INPUT_RECORD wchar2record(wchar_t u_char)
{
    INPUT_RECORD input_record;

    input_record.EventType = KEY_EVENT;
    input_record.Event.KeyEvent.bKeyDown = TRUE;
    input_record.Event.KeyEvent.dwControlKeyState = 0;
    input_record.Event.KeyEvent.uChar.UnicodeChar = u_char;
    input_record.Event.KeyEvent.wRepeatCount = 1;
    input_record.Event.KeyEvent.wVirtualKeyCode = 0;
    input_record.Event.KeyEvent.wVirtualScanCode = MapVirtualKey(0, MAPVK_VK_TO_VSC);

    return input_record;
}

INPUT_RECORD setEnter()
{
    return  wchar2record(VK_RETURN);
}


#endif // INPUTCONSOLE_H
