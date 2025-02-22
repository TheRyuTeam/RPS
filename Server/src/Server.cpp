#include "Server.hpp"
#include "Player.hpp"
#include "Room.hpp"
#include <QTcpServer>
#include <QRandomGenerator>

namespace RPS
{

Server::Server(const QString& address, quint64 port, QObject* parent)
    : QObject{ parent }
    , _server{ new QTcpServer(this) }
    , _players{}
    , _rooms{}
{
    connect(_server, &QTcpServer::newConnection, this, &Server::onNewConnection);
    _server->listen(QHostAddress(address), port);
}

Player* Server::createPlayer(QTcpSocket* socket)
{
    static int numId = 0;
    auto player = new Player(this, socket, QString::number(numId++), this);
    connect(player, &Player::disconnected, this, &Server::onDisconnected);
    return player;
}

Room* Server::createRoom(Player* player, const QString& name, qsizetype maxPlayers)
{
    static int numId = 0;
    auto room = new Room(QString::number(numId++), name, maxPlayers, this);
    connect(room, &Room::empty, this, &Server::onRoomEmpty);
    _rooms[room->id()] = room;
    return room;
}

Room* Server::getRoom(const QString& id)
{
    return _rooms.value(id, nullptr);
}

QList<Room*> Server::rooms() const
{
    return _rooms.values();
}

void Server::onNewConnection()
{
    auto player = createPlayer(_server->nextPendingConnection());
    _players[player->id()] = player;
}

void Server::onDisconnected()
{
    _players.remove(qobject_cast<Player*>(sender())->id());
    sender()->deleteLater();
}

void Server::onRoomEmpty()
{
    _rooms.remove(qobject_cast<Room*>(sender())->id());
    sender()->deleteLater();
}

}
