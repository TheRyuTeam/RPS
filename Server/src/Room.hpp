#pragma once
#include <RPS/common.hpp>
#include <QObject>
#include <QSet>
#include <QTimer>

namespace RPS
{

class Player;

/*!
 * @class Room
 * @brief All game magic is here. Responsible for rock paper scissors game session
 * @details Room stores players, manage game stages, player in game actions, i.e whole gameplay
 * @see @ref Player
 */
class Room
    : public QObject
{
    Q_OBJECT
public:
    explicit Room(const QString& id, const QString& name, qsizetype maxPlayers, QObject* parent = nullptr);
    ~Room() override;

    QString id() const { return _id; }
    QString name() const { return _name; }
    qsizetype maxPlayers() const noexcept { return _maxPlayers; }
    qsizetype playersCount() const noexcept { return _players.size(); }
    RPS::GameStage stage() const noexcept { return _stage; }

public slots:
    bool addPlayer(Player* player);
    void removePlayer(Player* player);

signals:
    void empty();

private slots:
    void onNameChanged(const QString& name);
    void onReadyChanged(bool ready);
    void onSignChanged(const QString& sign);

    QJsonObject makeStateObj() const;

    void sendAll(const QJsonObject& obj);

    void setStagePrivate(GameStage stage, qint64 duration);

    void startWaiting();
    void startReadyConfirm();
    void startPick();
    void startResult();

private:
    void checkReady();
    void checkSign();

private:
    QString _id;
    QString _name;
    qsizetype _maxPlayers;
    QList<Player*> _players;
    RPS::GameStage _stage;

    QSet<Player*> _ready;
    QSet<Player*> _sign;

    QTimer _readyComnfirmTimer;
    QTimer _pickTimer;
    QTimer _resultTimer;
};

}
