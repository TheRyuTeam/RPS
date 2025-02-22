#include "Player.hpp"
#include "Room.hpp"
#include "Server.hpp"
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace RPS
{

Player::Player(Server* server, QTcpSocket* socket, const QString& id, QObject* parent)
    : QObject{ parent }
    , _socket{ socket }
    , _buffer{}
    , _server{ server }
    , _room{ nullptr }
    , _id{ id }
    , _name{ "Player" }
    , _ready{ false }
    , _sign{}
{
    connect(_socket, &QTcpSocket::disconnected, this, &Player::onDisconnected);
    connect(_socket, &QTcpSocket::readyRead, this, &Player::onReadyRead);

    sendMessage({{"source", MessageSourceServer}, {"type", ServerMessagePlayerId}, {"id", id}});
}

QString Player::id() const
{
    return _id;
}

QString Player::name() const
{
    return _name;
}

void Player::setName(const QString& name)
{
    if (_name != name) {
        _name = name;
        emit nameChanged(_name);
        sendMessage({{"source", MessageSourceServer}, {"type", ServerMessagePlayerName}, {"name", name}});
    }
}

bool Player::ready() const
{
    return _ready;
}

void Player::setReady(bool ready)
{
    _ready = ready;
    emit readyChanged(_ready);
    sendMessage({{"source", MessageSourceServer}, {"type", ServerMessagePlayerReady}, {"ready", _ready}});
}

QString Player::sign() const
{
    return _sign;
}

void Player::setSign(const QString& sign)
{
    _sign = sign;
    emit signChanged(_sign);
}

void Player::setRoom(Room* room)
{
    resetRoom();
    if (!room) {
        sendMessage(QJsonObject({{"source", MessageSourceServer}, {"type", ServerMessageRoomNotExists}}));
    } else if (room->addPlayer(this)) {
        _room = room;
    }
}

void Player::setRoom(const QString& roomId)
{
    setRoom(_server->getRoom(roomId));
}

void Player::resetRoom()
{
    if (_room) {
        _room->removePlayer(this);
        _room = nullptr;
    }
}

void Player::sendMessage(const QJsonObject& obj)
{
    _socket->write(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact) + '\n');
}

void Player::onDisconnected()
{
    resetRoom();
    emit disconnected();
}

void Player::onReadyRead()
{
    _buffer += _socket->readAll();
    while (true) {
        auto pos = _buffer.indexOf('\n');
        if (pos < 0) {
            break;
        }

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(_buffer.left(pos), &error);

        _buffer = _buffer.mid(pos + 1);

        if (error.error != QJsonParseError::NoError || !doc.isObject()) {
            continue;
        }

        processData(doc.object());
    }
}

void Player::onGetRooms()
{
    QJsonArray roomsList;
    for (auto room : _server->rooms()) {
        roomsList.append(QJsonObject({
                                         {"roomId", room->id()},
                                         {"name", room->name()},
                                         {"maxPlayers", room->maxPlayers()},
                                         {"playersCount", room->playersCount()},
                                         {"stage", room->stage() }
                                     }));
    }
    sendMessage({{"source", MessageSourceServer}, {"type", ServerMessageRoomsList}, {"rooms", roomsList}});
}

void Player::processData(const QJsonObject& obj)
{
    auto type = obj["type"].toVariant().value<RequestMessageType>();
    if (type == RequestMessageSetName) {
        setName(obj["name"].toString());
    } else if (type == RequestMessageRoomsList) {
        onGetRooms();
    } else if (type == RequestMessageCreateRoom) {
        setRoom(_server->createRoom(this, obj["name"].toString("Room"), std::min(16, std::max(2, obj["maxPlayers"].toInt(2)))));
    } else if (type == RequestMessageEnterRoom) {
        setRoom(obj["roomId"].toString(""));
    } else if (type == RequestMessageLeaveRoom) {
        resetRoom();
    } if (type == RequestMessageSetReady) {
        setReady(obj["ready"].toBool());
    } else if (type == RequestMessageSetSign) {
        setSign(obj["sign"].toString());
    }
}

}
