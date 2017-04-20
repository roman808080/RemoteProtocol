#include <QTcpSocket>
#include <QSharedPointer>
#include <QDataStream>


#include "server.h"
#include "convertordata.h"
#include "datastruct.h"

Server::Server(QSharedPointer<QTcpSocket> socket)
{
    this->socket.reset(socket.data());
}

int Server::write(DataOut& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    QByteArray qbytearray;
    qint32 size = sizeof(data);
    ConvertorData::data_to_qbytearray(&data, qbytearray, size);

    out << size;
    if(out.writeRawData(qbytearray.data(), size) == -1)
        return -1;

    socket->write(block);
    socket->waitForBytesWritten();
    return 0;
}

int Server::read(DataIn& data)
{
    socket->waitForReadyRead();
    QDataStream in;
    in.setDevice(socket.data());
    in.setVersion(QDataStream::Qt_4_0);

    qint32 size;
    in >> size;

    QByteArray dataArray;
    dataArray.resize(size);

    if(in.readRawData(dataArray.data(), size) == -1)
        return -1;

    ConvertorData::qbytearray_to_data(dataArray, &data, size);
    return 0;
}

void Server::loop()
{
//    QObject::connect(this, SIGNAL(finished()), this, SLOT(quit()));
//    start();
    run();
}

void Server::run()
{
    while(true){
        // first read
        DataIn nextMessage;
        read(nextMessage);
        writeInputToConsole(nextMessage);

        qDebug() << nextMessage.array << "\n";
        // after write our console answer
        DataOut answer;
        int i = 0;
        for(char c: "Answer"){
            answer.array[i] = c;
            i++;
        }
        write(answer);
   }
}

int Server::writeInputToConsole(DataIn& data)
{
    //temporary
    FreeConsole(); // console lost

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    std::wstring path = L"cmd.exe";

    SECURITY_ATTRIBUTES security = {
       sizeof(security), NULL, TRUE
     };

    // Start the child process.
    if(!CreateProcess(NULL, (LPWSTR)path.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return -1;
    }

    Sleep(2000);

    if(!AttachConsole(pi.dwProcessId)){
        printf( "AttachConsole failed (%d).\n", GetLastError() );
        return -1;
    }

    HANDLE hConIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwTmp;

    dwTmp = 0;
    WriteConsoleInput(hConIn, &data.inputRecords[0], SIZE_INPUT_RECORDS, &dwTmp);
    Sleep(1000);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return 0;
}

int Server::readOutputFromConsole(DataOut& data)
{
    //pass
}
