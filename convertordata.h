#ifndef CONVERTORDATA_H
#define CONVERTORDATA_H
#include <QByteArray>

class ConvertorData
{
public:
    static int qbytearray_to_data(const QByteArray& str, void* data, qint32 size);
    static int data_to_qbytearray(void* data, QByteArray& str, qint32 size);

};

#endif // CONVERTORDATA_H
