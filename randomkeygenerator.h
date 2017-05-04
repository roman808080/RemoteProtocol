#ifndef RANDOMKEYGENERATOR_H
#define RANDOMKEYGENERATOR_H
#include <QDebug>
#include "windows.h"
#include "wincrypt.h"
#include "tchar.h"
#include "vector"

#define COUNT_SYMBOLS 62

const char symbols[] = {
    '0','1','2','3','4','5','6','7','8','9',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','a','b','c','d',
    'e','f','g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v','w','x',
    'y','z'
};

bool generateRandomKey(std::vector<char>& keyBuff, int size)
{
    HCRYPTPROV provider;
    if (!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, NULL))
    {
        if (!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
        {
            return false;
        }
    }

    keyBuff.clear();
    keyBuff.resize(size);
    if (!CryptGenRandom(provider, size, (BYTE*)&keyBuff.front()))
    {
        return false;
    }

    for(int i=0; i<size; i++)
    {
        if(keyBuff[i] < 0)
        {
            keyBuff[i] *= -1;
        }

        keyBuff[i] = symbols[keyBuff[i]%COUNT_SYMBOLS];
    }
    return true;
}

#endif // RANDOMKEYGENERATOR_H
