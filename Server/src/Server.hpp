#pragma once
#include <QObject>
#include <QMap>

class QTcpServer;
class QTcpSocket;

namespace RPS
{

class Player;
class Room;

/*!
 * @class Server
 * @brief Tcp server that just stored players and rooms. Also responsible for deleting empty rooms
 * @see @ref Player
 * @see @ref Room
 */
class Server
    : public QObject
{
    Q_OBJECT

public:
    explicit Server(const QString& address, quint64 port, QObject* parent = nullptr);

    Player* createPlayer(QTcpSocket* socket);
    Room* createRoom(Player* player, const QString& name, qsizetype maxPlayers);
    Room* getRoom(const QString& id);
    QList<Room*> rooms() const;

private slots:
    void onNewConnection();
    void onDisconnected();

    void onRoomEmpty();

private:
    QTcpServer* _server;
    QMap<QString, Player*> _players;
    QMap<QString, Room*> _rooms;
};

}
