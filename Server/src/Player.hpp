#pragma once
#include <RPS/common.hpp>
#include <QObject>
#include <QJsonObject>

class QTcpSocket;

namespace RPS
{

class Room;
class Server;

/*!
 * @class Player
 * @brief Kinda remote object for clients
 * @details Handles processing client connection, sending/receiving commands.
 */
class Player
    : public QObject
{
    Q_OBJECT

public:
    explicit Player(Server* server, QTcpSocket* socket, const QString& id, QObject* parent = nullptr);

    QString id() const;
    QString name() const;
    void setName(const QString& name);
    bool ready() const;
    void setReady(bool ready);
    QString sign() const;
    void setSign(const QString& sign);

public slots:
    void setRoom(Room* room);
    void setRoom(const QString& roomId);
    void resetRoom();

    void sendMessage(const QJsonObject& obj);

signals:
    void disconnected();

    void nameChanged(const QString& name);
    void signChanged(const QString& sign);
    void readyChanged(bool ready);

private slots:
    void onDisconnected();
    void onReadyRead();

    void onGetRooms();

    void processData(const QJsonObject& obj);

private:
    QTcpSocket* _socket;
    QByteArray _buffer;

    Server* _server;
    Room* _room;

    QString _id;
    QString _name;
    bool _ready;
    QString _sign;
};

}
