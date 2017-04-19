#include "convertordata.h"

int ConvertorData::qbytearray_to_data(const QByteArray& str, void* data, qint32 size)
{
    if (str.size() < size - 1)
        return -1;
    for (int i = 0; i < size; i++)
        ((char*)data)[i] = str[i];
    return 0;
}

int ConvertorData::data_to_qbytearray(void* data, QByteArray& str, qint32 size)
{
    str.resize(size);
    for (int i = 0; i < size; i++)
        str[i] = ((char*)data)[i];
    return 0;
}
