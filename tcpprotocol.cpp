#include "tcpprotocol.h"

#define DEFAULT_TCP_PORT 4644

TcpProtocol::TcpProtocol()
    : mTcpServer(NULL), mCurrentSocket(NULL)
{
    setPort(DEFAULT_TCP_PORT);
    dwParrentId = GetCurrentProcessId();
}

TcpProtocol::~TcpProtocol()
{
    mTcpServer->close();
}

void TcpProtocol::runTcpServer()
{
    mTcpServer.reset(new QTcpServer);
    mTcpServer->listen(QHostAddress::Any, mLocalTcpPort);
    connect(mTcpServer.data(), SIGNAL(newConnection()), this, SLOT(newIncomingConnection()));
}

void TcpProtocol::setPort(qint16 tcp)
{
    mLocalTcpPort = tcp;
}

void TcpProtocol::setPassword(std::vector<char> password)
{
    this->password = password;
}

void TcpProtocol::connectToServer(QString ip, int port)
{
    mCurrentSocket = QSharedPointer<QTcpSocket>(new QTcpSocket);
    connect(mCurrentSocket.data(), SIGNAL(connected()), this, SLOT(connected()));
    mCurrentSocket->connectToHost(ip, port);
}

void TcpProtocol::newIncomingConnection()
{
    if (!mTcpServer->hasPendingConnections()) return;

    mCurrentSocket = QSharedPointer<QTcpSocket>(mTcpServer->nextPendingConnection());

//    startProcessServer(mCurrentSocket->socketDescriptor(), NULL);

    connectionHandlers.append(QSharedPointer<ConnectionHandler>(new ConnectionHandler));
    connectionHandlers.at(connectionHandlers.size() - 1)->setSocket(mCurrentSocket);
    connectionHandlers.at(connectionHandlers.size() - 1)->setPassword(password);
    connect(connectionHandlers.at(connectionHandlers.size() - 1).data(), SIGNAL(closed()), this, SLOT(closedProcess()));
    connectionHandlers.at(connectionHandlers.size() - 1)->startServer();

    emit newInConnection(mCurrentSocket);
}

void TcpProtocol::connected()
{
    connectionHandlers.append(QSharedPointer<ConnectionHandler>(new ConnectionHandler));
    connectionHandlers.at(connectionHandlers.size() - 1)->setSocket(mCurrentSocket);
    connect(connectionHandlers.at(connectionHandlers.size() - 1).data(), SIGNAL(closed()), this, SLOT(closedProcess()));

    emit newOutConnection(mCurrentSocket);
}

void TcpProtocol::startProcessServer(qintptr descriptor, LPWSTR desktopName)
{
    qDebug() << descriptor;

    if(desktopName)
        desktop = CreateDesktopW(desktopName, 0, 0, 0, GENERIC_ALL, 0);

    std::wstring passwordServer(password.begin(), password.end());

    wchar_t temp[10];
    _ltow(descriptor, temp, 10);

    std::wstring path = L"remoteprotocol.exe";
    path += L" " + passwordServer;
    path += L" ";
    path += temp;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    if(desktopName)
        si.lpDesktop = desktopName;

    SECURITY_ATTRIBUTES security = {
       sizeof(security), NULL, TRUE
     };

    if(CreateProcess(NULL, (LPWSTR)path.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        dwProcessIds.push_back(pi.dwProcessId);
    }
    else
    {
        DWORD dwErrorId = GetLastError();
        printf("CreateProcess failed (%d).\n", dwErrorId);
        return;
    }

    Sleep(500);
}

void TcpProtocol::startProcessClient(QString ip, int port)
{

}

void TcpProtocol::closedProcess()
{
    int countClosed = 0;
    for(auto connectionHandler: connectionHandlers)
    {
        if(!connectionHandler.data()->alive())
        {
            countClosed++;
        }
    }
    if(connectionHandlers.size() == countClosed)
    {
        killSelf();
    }
}

void TcpProtocol::killSelf()
{
    HANDLE killed = OpenProcess(PROCESS_TERMINATE, false, dwParrentId);
    if (killed)
        TerminateProcess(killed, 0);
}
