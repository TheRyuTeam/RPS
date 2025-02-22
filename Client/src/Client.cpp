#include "Client.hpp"

Client::Client(QObject* parent)
    : QObject{ parent }
    , _serverAddress{}
    , _socket{ new QTcpSocket(this) }
    , _connected{ false }
    , _playerId{}
    , _playerName{ "Player" }
    , _inRoom{ false }
    , _roomName{}
    , _roomStage{ RPS::GameStageNotInRoom }
    , _roomStageRemaining{ -1 }
    , _reconnectTimer{}
    , _stageTimer{}
{
    connect(this, &Client::serverAddressChanged, this, &Client::reconnect);

    _reconnectTimer.setInterval(3000);
    connect(&_reconnectTimer, &QTimer::timeout, this, &Client::reconnect);

    _stageTimer.setSingleShot(true);
    _stageTimer.setInterval(1000);
    connect(&_stageTimer, &QTimer::timeout, this, &Client::onStageTimer);

    connect(_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(_socket, &QTcpSocket::stateChanged, this, &Client::onStateChanged);
    connect(_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

QString Client::serverAddress() const
{
    return _serverAddress;
}

void Client::setServerAddress(const QString& serverAddress)
{
    if (_serverAddress != serverAddress) {
        _serverAddress = serverAddress;
        emit serverAddressChanged(_serverAddress);
    }
}

bool Client::connected() const noexcept
{
    return _connected;
}

QString Client::playerId() const
{
    return _playerId;
}

QString Client::playerName() const
{
    return _playerName;
}

void Client::setPlayerName(const QString& playerName)
{
    sendCommand({{"type", RPS::RequestMessageSetName}, {"name", playerName}});
}

bool Client::inRoom() const
{
    return _inRoom;
}

QString Client::roomName() const
{
    return _roomName;
}

RPS::GameStage Client::roomStage() const
{
    return _roomStage;
}

qint64 Client::roomStageRemaining() const
{
    return _roomStageRemaining;
}

bool Client::ready() const
{
    return _ready;
}

void Client::pushConnected(bool connected)
{
    if (_connected != connected) {
        _connected = connected;
        emit connectedChanged(_connected);
    }
}

void Client::pushPlayerId(const QString& playerId)
{
    if (_playerId != playerId) {
        _playerId = playerId;
        emit playerIdChanged(_playerId);
    }
}

void Client::pushPlayerName(const QString& playerName)
{
    if (_playerName != playerName) {
        _playerName = playerName;
        emit playerNameChanged(_playerName);
    }
}

void Client::pushInRoom(bool inRoom)
{
    if (_inRoom != inRoom) {
        _inRoom = inRoom;
        emit inRoomChanged(_inRoom);
    }
}

void Client::pushRoomName(const QString& roomName)
{
    if (_roomName != roomName) {
        _roomName = roomName;
        emit roomNameChanged(_roomName);
    }
}

void Client::pushStage(RPS::GameStage roomStage)
{
    if (_roomStage != roomStage) {
        _roomStage = roomStage;
        emit roomStageChanged(_roomStage);
    }
}

void Client::pushRoomStageRemaining(qint64 roomStageRemaining)
{
    if (_roomStageRemaining != roomStageRemaining) {
        _roomStageRemaining = roomStageRemaining;
        emit roomStageRemainingChanged(_roomStageRemaining);
    }
}

void Client::pushReady(bool ready)
{
    if (_ready != ready) {
        _ready = ready;
        emit readyChanged(_ready);
    }
}

void Client::sendCommand(const QJsonObject& obj)
{
    _socket->write(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact) + '\n');
}

void Client::onStageTimer()
{
    if (roomStageRemaining() - 1000 > 0) {
        pushRoomStageRemaining(roomStageRemaining() - 1000);
        _stageTimer.start();
    } else {
        pushRoomStageRemaining(-1);
        _stageTimer.stop();
    }
}

void Client::reconnect()
{
    auto url = QUrl::fromUserInput(_serverAddress);
    _socket->connectToHost(url.host(), url.port());
}

void Client::onConnected()
{
    pushConnected(true);
    setPlayerName(playerName());
}

void Client::onDisconnected()
{
    pushConnected(false);
    pushInRoom(false);
    pushStage(RPS::GameStageNotInRoom);
}

void Client::onStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::SocketState::UnconnectedState) {
        _reconnectTimer.start();
    } else {
        _reconnectTimer.stop();
    }
}

void Client::onReadyRead()
{
    while (_socket->canReadLine()) {
        QJsonDocument doc = QJsonDocument::fromJson(_socket->readLine());
        auto obj = doc.object();
        processMessage(obj);
    }
}

void Client::processMessage(const QJsonObject& obj)
{
    auto source = obj["source"].toVariant().value<RPS::MessageSourceType>();
    switch (source) {
    case RPS::MessageSourceServer:
        processServerMessage(obj);
        break;
    case RPS::MessageSourceRoom:
        processRoomMessage(obj);
        break;
    case RPS::MessageSourceInvalid:
    default:
        break;
    }
}

void Client::processServerMessage(const QJsonObject& obj)
{
    auto type = obj["type"].toVariant().value<RPS::ServerMessageType>();
    switch (type) {
    case RPS::ServerMessagePlayerId:
        pushPlayerId(obj["id"].toString());
        break;
    case RPS::ServerMessagePlayerName:
        pushPlayerName(obj["name"].toString());
        break;
    case RPS::ServerMessagePlayerReady:
        pushReady(obj["ready"].toBool());
        break;
    case RPS::ServerMessageRoomsList:
        emit roomsList(obj["rooms"].toArray());
        break;
    case RPS::ServerMessageRoomNotExists:
        emit roomIsNotExists();
        break;
    case RPS::ServerMessageInvalid:
    default:
        break;
    }
}

void Client::processRoomMessage(const QJsonObject& obj)
{
    auto type = obj["type"].toVariant().value<RPS::RoomMessageType>();
    switch (type) {
    case RPS::RoomMessageIsFull:
        emit roomIsFull();
        break;
    case RPS::RoomMessageIsPlaying:
        emit roomIsPlaying();
        break;
    case RPS::RoomMessageEntered:
        _inRoom = true;
        emit inRoomChanged(_inRoom);
        break;
    case RPS::RoomMessageExited:
        pushInRoom(false);
        pushStage(RPS::GameStageNotInRoom);
        pushRoomStageRemaining(-1);
        break;
    case RPS::RoomMessageInfo:
        emit roomInfo(obj);
        pushRoomName(obj["name"].toString());
        pushStage(obj["stage"].toVariant().value<RPS::GameStage>());
        break;
    case RPS::RoomMessageStageChanged: {
        pushStage(obj["stage"].toVariant().value<RPS::GameStage>());
        auto dur = obj["duration"].toInteger();
        if (dur > 0) {
            pushRoomStageRemaining(dur);
            _stageTimer.start();
        } else {
            _stageTimer.stop();
            pushRoomStageRemaining(-1);
        }
        break;
    }
    case RPS::RoomMessagePlayerNameChanged:
        emit roomPlayerPropertyChanged(obj["playerId"].toString(), "name", obj["name"].toString());
        break;
    case RPS::RoomMessagePlayerReadyChanged: {
        emit roomPlayerPropertyChanged(obj["playerId"].toString(), "ready", obj["ready"].toBool());
        break;
    }
    case RPS::RoomMessagePlayerSignsList:
        emit roomSignsList(obj["players"].toArray());
        break;
    case RPS::RoomMessagePlayersList:
        emit roomPlayersList(obj["players"].toArray());
        break;
    case RPS::RoomMessageInvalid:
    default:
        break;
    }
}

