#pragma once
#include <QObject>

namespace RPS
{
Q_NAMESPACE

enum GameStage
{
    GameStageNotInRoom,
    GameStageWaiting,
    GameStageReadyConfirmation,
    GameStagePick,
    GameStageResult,
};
Q_ENUM_NS(GameStage);

enum MessageSourceType
{
    MessageSourceInvalid,
    MessageSourceServer,
    MessageSourceRoom
};
Q_ENUM_NS(MessageSourceType);

enum ServerMessageType
{
    ServerMessageInvalid,
    ServerMessagePlayerId,
    ServerMessagePlayerName,
    ServerMessagePlayerReady,
    ServerMessageRoomsList,
    ServerMessageRoomNotExists,
};
Q_ENUM_NS(ServerMessageType);

enum RoomMessageType
{
    RoomMessageInvalid,
    RoomMessageIsFull,
    RoomMessageIsPlaying,
    RoomMessageEntered,
    RoomMessageExited,
    RoomMessageInfo,
    RoomMessageStageChanged,
    RoomMessagePlayerNameChanged,
    RoomMessagePlayerReadyChanged,
    RoomMessagePlayerSignsList,
    RoomMessagePlayersList,
};
Q_ENUM_NS(RoomMessageType);

enum RequestMessageType
{
    RequestMessageInvalid,
    RequestMessageSetName,
    RequestMessageRoomsList,
    RequestMessageCreateRoom,
    RequestMessageEnterRoom,
    RequestMessageLeaveRoom,
    RequestMessageSetReady,
    RequestMessageSetSign
};
Q_ENUM_NS(RequestMessageType);

};
