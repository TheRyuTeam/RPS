#pragma once
#include <RPS/common.hpp>
#include <QObject>
#include <QtQmlIntegration>
#include <QJsonObject>
#include <QTcpSocket>

/*!
 * @class Client
 * @brief Big and scarry all in one class for handling connection, reconnection, communication with remote server
 */
class Client
    : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

    Q_PROPERTY(QString playerId READ playerId NOTIFY playerIdChanged)
    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName NOTIFY playerNameChanged)

    Q_PROPERTY(bool inRoom READ inRoom NOTIFY inRoomChanged)
    Q_PROPERTY(QString roomName READ roomName NOTIFY roomNameChanged)
    Q_PROPERTY(RPS::GameStage roomStage READ roomStage NOTIFY roomStageChanged)
    Q_PROPERTY(qint64 roomStageRemaining READ roomStageRemaining NOTIFY roomStageRemainingChanged)

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
    explicit Client(QObject* parent = nullptr);

    QString serverAddress() const;
    void setServerAddress(const QString& serverAddress);
    bool connected() const noexcept;

    QString playerId() const;
    QString playerName() const;
    void setPlayerName(const QString& playerName);

    bool inRoom() const;
    QString roomName() const;
    RPS::GameStage roomStage() const;
    qint64 roomStageRemaining() const;

    bool ready() const;

    Q_INVOKABLE void requestRoomsList();
    Q_INVOKABLE void requestCreateRoom(const QString& name, int maxPlayers);
    Q_INVOKABLE void requestEnterRoom(const QString& roomId);
    Q_INVOKABLE void requestLeaveRoom();

    Q_INVOKABLE void requestSetReady(bool ready);
    Q_INVOKABLE void requestSetSign(const QString& choice);

private:
    void pushConnected(bool connected);
    void pushPlayerId(const QString& playerId);
    void pushPlayerName(const QString& playerName);
    void pushInRoom(bool inRoom);
    void pushRoomName(const QString& roomName);
    void pushStage(RPS::GameStage roomStage);
    void pushRoomStageRemaining(qint64 roomStageRemaining);
    void pushReady(bool ready);

signals:
    void serverAddressChanged(const QString& serverAddress);
    void connectedChanged(bool connected);

    void playerIdChanged(const QString& playerId);
    void playerNameChanged(const QString& playerName);

    void inRoomChanged(bool inRoom);
    void roomNameChanged(const QString& roomName);
    void roomStageChanged(RPS::GameStage roomStage);
    void roomStageRemainingChanged(qint64 roomStageRemaining);

    void readyChanged(bool ready);

    void roomsList(const QJsonArray& rooms);
    void roomIsNotExists();
    void roomIsFull();
    void roomIsPlaying();

    void roomInfo(const QJsonObject& info);
    void roomPlayersList(const QJsonArray& players);
    void roomPlayerPropertyChanged(const QString& id, const QString& property, const QVariant& value);
    void stageRemainingChanged(qint64 remaining);
    void roomSignsList(const QJsonArray& players);

private slots:
    void sendCommand(const QJsonObject& obj);
    void onStageTimer();

    void reconnect();
    void onConnected();
    void onDisconnected();
    void onStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();

    void processMessage(const QJsonObject& obj);
    void processServerMessage(const QJsonObject& obj);
    void processRoomMessage(const QJsonObject& obj);

private:
    QString _serverAddress;
    QTcpSocket* _socket;
    bool _connected;

    QString _playerId;
    QString _playerName;

    bool _inRoom;
    QString _roomName;
    RPS::GameStage _roomStage;
    qint64 _roomStageRemaining;

    bool _ready;

    QTimer _reconnectTimer;
    QTimer _stageTimer;
};

inline void Client::requestRoomsList()
{
    sendCommand({{"type", RPS::RequestMessageRoomsList}});
}

inline void Client::requestCreateRoom(const QString& name, int maxPlayers)
{
    sendCommand({{"type", RPS::RequestMessageCreateRoom}, {"name", name}, {"maxPlayers", maxPlayers}});
}

inline void Client::requestEnterRoom(const QString& roomId)
{
    sendCommand({{"type", RPS::RequestMessageEnterRoom}, {"roomId", roomId}});
}

inline void Client::requestLeaveRoom()
{
    sendCommand({{"type", RPS::RequestMessageLeaveRoom}});
}

inline void Client::requestSetReady(bool ready)
{
    QJsonObject obj({{"type", RPS::RequestMessageSetReady}, {"ready", ready}});
    sendCommand(obj);
}

inline void Client::requestSetSign(const QString& choice)
{
    sendCommand({{"type", RPS::RequestMessageSetSign}, {"sign", choice}});
}
