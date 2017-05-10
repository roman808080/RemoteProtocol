#ifndef REMOTEPROTOCOL_H
#define REMOTEPROTOCOL_H


#include <QObject>

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostInfo>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QHash>
#include <QTimer>
#include <QDataStream>

#include <locale>
#include <codecvt>
#include <string>

#include "connectionhandler.h"

class TcpProtocol : public QObject
{
    Q_OBJECT

    public:
        TcpProtocol();
        virtual ~TcpProtocol();
        void runTcpServer();
        void setPort(qint16 tcp);
        void setPassword(std::vector<char> password);
        void connectToServer(QString ip, int port);

        void startProcessServer(LPWSTR desktopName);
        void startProcessClient(QString ip, int port);

    public slots:
        void newIncomingConnection();
        void connected();
        void closeServer();

    signals:
        void newOutConnection(QSharedPointer<QTcpSocket> mSock);
        void newInConnection(QSharedPointer<QTcpSocket> mSock);
        void closed();

    private:
        QScopedPointer<QTcpServer> mTcpServer; // Socket TCP
        QSharedPointer<QTcpSocket> mCurrentSocket; // Socket TCP current socket
        QList<QSharedPointer<ConnectionHandler>> connectionHandlers; // list all connection
        std::vector<char> password;

        //TCP port
        qint16 mLocalTcpPort;

        DWORD dwParrentId;
};

#endif // REMOTEPROTOCOL_H

