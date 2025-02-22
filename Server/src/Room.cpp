#include "Room.hpp"
#include "Player.hpp"
#include <QJsonObject>
#include <QJsonArray>

#define PLAYING_MSG() {{"source", MessageSourceRoom}, {"type", RoomMessageIsPlaying}}
#define FULL_MSG() {{"source", MessageSourceRoom}, {"type", RoomMessageIsFull}}
#define ENTERED_MSG() {{"source", MessageSourceRoom}, {"type", RoomMessageEntered}}
#define EXITED_MSG() {{"source", MessageSourceRoom}, {"type", RoomMessageExited}}
#define INFO_MSG(Id, Name, MaxPlayers, PlayersCount, Stage) {{"source", MessageSourceRoom}, {"type", RoomMessageInfo}, \
    {"id", Id}, {"name", Name}, {"maxPlayers", MaxPlayers}, {"playersCount", PlayersCount}, {"stage", Stage}}
#define STAGE_MSG(Stage, Dur) QJsonObject({{"source", MessageSourceRoom}, {"type", RoomMessageStageChanged}, {"stage", Stage}, {"duration", Dur}})
#define PLAYERS_MSG(Players) {{"source", MessageSourceRoom}, {"type", RoomMessagePlayersList}, {"players", playersList}}
#define PLAYER_NAME_MSG(Id, Name) {{"source", MessageSourceRoom}, {"type", RoomMessagePlayerReadyChanged}, {"playerId", Id}, {"name", Name}}
#define PLAYER_READY_MSG(Id, Ready) {{"source", MessageSourceRoom}, {"type", RoomMessagePlayerReadyChanged}, {"playerId", Id}, {"ready", Ready}}
#define SIGNS_LIST_MSG(Players) \
{{"source", MessageSourceRoom}, {"type", RoomMessagePlayerSignsList}, {"players", Players}}

namespace RPS
{

Room::Room(const QString& id, const QString& name, qsizetype maxPlayers, QObject* parent)
    : QObject{ parent }
    , _id{ id }
    , _name{ name }
    , _maxPlayers{ maxPlayers }
    , _players{}
    , _stage{ GameStageWaiting }
    , _ready{}
    , _sign{}
    , _readyComnfirmTimer{}
    , _pickTimer{}
    , _resultTimer{}
{
    _readyComnfirmTimer.setSingleShot(true);
    _readyComnfirmTimer.setInterval(5000);
    connect(&_readyComnfirmTimer, &QTimer::timeout, this, &Room::startPick);

    _pickTimer.setSingleShot(true);
    _pickTimer.setInterval(10000);
    connect(&_pickTimer, &QTimer::timeout, this, &Room::startResult);

    _resultTimer.setSingleShot(true);
    _resultTimer.setInterval(10000);
    connect(&_resultTimer, &QTimer::timeout, this, &Room::startWaiting);
}

Room::~Room()
{
    sendAll({{"source", MessageSourceRoom}, {"type", RoomMessageExited}});
}

bool Room::addPlayer(Player* player)
{
    if (!player) {
        return false;
    }
    if (_stage == GameStagePick || _stage == GameStageResult) {
        player->sendMessage(PLAYING_MSG());
        return false;
    }
    if (_players.size() == _maxPlayers) {
        player->sendMessage(FULL_MSG());
        return false;
    }
    if (_players.contains(player)) {
        return false;
    }

    connect(player, &Player::nameChanged, this, &Room::onNameChanged);
    connect(player, &Player::readyChanged, this, &Room::onReadyChanged);
    connect(player, &Player::signChanged, this, &Room::onSignChanged);

    _players.append(player);

    player->setReady(false);
    player->setSign("");

    player->sendMessage(ENTERED_MSG());
    player->sendMessage(INFO_MSG(_id, _name, _maxPlayers, _players.size(), _stage));
    sendAll(makeStateObj());

    return true;
}

void Room::removePlayer(Player* player)
{
    disconnect(player, &Player::nameChanged, this, &Room::onNameChanged);
    disconnect(player, &Player::readyChanged, this, &Room::onReadyChanged);
    disconnect(player, &Player::signChanged, this, &Room::onSignChanged);

    _players.removeOne(player);
    _ready.remove(player);
    if (_stage == GameStageReadyConfirmation) {
        checkReady();
    }
    if (_stage == GameStagePick) {
        checkSign();
    }

    player->sendMessage(EXITED_MSG());
    sendAll(makeStateObj());
    if (_players.size() == 0) {
        emit empty();
    }
}

void Room::onNameChanged(const QString& name)
{
    sendAll(PLAYER_NAME_MSG(qobject_cast<Player*>(sender())->id(), name));
}

void Room::onReadyChanged(bool ready)
{
    if (_stage == GameStageWaiting || _stage == GameStageReadyConfirmation) {
        auto player = qobject_cast<Player*>(sender());
        sendAll(PLAYER_READY_MSG(player->id(), ready));
        if (ready) {
            _ready.insert(player);
        } else {
            _ready.remove(player);
        }
        checkReady();
    }
}


void Room::onSignChanged(const QString& sign)
{
    if (_stage == GameStagePick) {
        auto player = qobject_cast<Player*>(sender());
        _sign.insert(player);
        checkSign();
    }
}

QJsonObject Room::makeStateObj() const
{
    QJsonArray playersList;
    for (auto player : _players) {
        QJsonObject playerObj;
        playerObj["playerId"] = player->id();
        playerObj["name"] = player->name();
        playerObj["ready"] = player->ready();
        playerObj["sign"] = player->sign();
        playersList.append(playerObj);
    }
    return PLAYERS_MSG(playersList);
}

void Room::sendAll(const QJsonObject& obj)
{
    for (auto player : _players) {
        player->sendMessage(obj);
    }
}

void Room::setStagePrivate(GameStage stage, qint64 duration)
{
    if (_stage != stage) {
        _stage = stage;
        sendAll(STAGE_MSG(_stage, duration));
    }
}

void Room::startWaiting()
{
    setStagePrivate(GameStageWaiting, -1);
    for (auto p : _players) {
        p->setReady(false);
        p->setSign("");
    }
    _ready.clear();
    _sign.clear();
    sendAll(makeStateObj());
    _readyComnfirmTimer.stop();
    _pickTimer.stop();
    _resultTimer.stop();
}

void Room::startReadyConfirm()
{
    setStagePrivate(GameStageReadyConfirmation, _readyComnfirmTimer.interval());
    _readyComnfirmTimer.start();
}

void Room::startPick()
{
    setStagePrivate(GameStagePick, _pickTimer.interval());
    _pickTimer.start();
}

void Room::startResult()
{
    setStagePrivate(GameStageResult, _resultTimer.interval());

    QMap<QString, int> signs;
    for (auto p : _players) {
        signs[p->sign()] += 1;
    }

    QMap<QString, QString> beatBy {{"rock", "paper"}, {"paper", "scissors"}, {"scissors", "rock"}};
    QMap<Player*, int> damageTaken;
    for (auto p : _players) {
        if (!beatBy.contains(p->sign())) {
            damageTaken[p] = std::numeric_limits<int>::max();
        } else {
            damageTaken[p] += signs[beatBy[p->sign()]];
        }
    }

    QList<QPair<Player*, int>> damageTakenList{ damageTaken.keyValueBegin(), damageTaken.keyValueEnd() };
    std::sort(damageTakenList.begin(), damageTakenList.end(), [](const QPair<Player*, int>& a, const QPair<Player*, int>& b) {
        return a.second < b.second;
    });

    QMap<Player*, int> winlose;
    winlose[damageTakenList[0].first] = 1;
    for (qsizetype i = 1; i < damageTakenList.size() && damageTakenList[0].second == damageTakenList[i].second; ++i) {
        winlose[damageTakenList[i].first] = 1;
    }

    winlose[damageTakenList[damageTakenList.size() - 1].first] = 2;
    for (qsizetype i = damageTakenList.size() - 2; i >= 0 &&
        damageTakenList[damageTakenList.size() - 1].second == damageTakenList[i].second; --i) {
        winlose[damageTakenList[i].first] = 2;
    }

    QJsonArray players;
    for (auto p : _players) {
        players.append(QJsonObject({{"playerId", p->id()}, {"sign", p->sign()}, {"winlose", winlose[p]}}));
    }
    sendAll(SIGNS_LIST_MSG(players));
    _resultTimer.start();
}

void Room::checkReady()
{
    if (_ready.size() == _players.size() && _players.size() > 1) {
        startReadyConfirm();
    } else {
        _readyComnfirmTimer.stop();
        setStagePrivate(GameStageWaiting, -1);
    }
}

void Room::checkSign()
{
    if (_sign.size() == _players.size()) {
        _pickTimer.stop();
        startResult();
    }
}

}
