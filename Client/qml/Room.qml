import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects
import RPS as RPS
import Ryu

Item {
    id: root
    property string roomId
    property string name
    property int maxPlayers
    property int playersCount
    property var stage: 0

    function getPlayerIndex(playerId) {
        for (var i = 0; i < view.model.count; ++i) {
            var player = view.model.get(i)
            if (player.playerId === playerId) {
                return i
            }
        }

        return -1
    }

    ColumnLayout {
        anchors.fill: parent

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: RPS.Client.roomStage !== Enums.GameStagePick ? 0 : 1

            ListView {
                id: view
                spacing: 5

                model: ListModel {}

                delegate: Item {
                    id: delegate
                    required property string playerId
                    required property string name
                    required property bool ready
                    required property int winlose
                    required property string sign

                    width: view.width
                    height: 60

                    Pane {
                        anchors.fill: parent
                        Material.elevation: 4

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.topMargin: 5
                            anchors.rightMargin: 20
                            anchors.bottomMargin: 5

                            Label {
                                Layout.fillHeight: true
                                text: delegate.name + "  @" + delegate.playerId
                                color: delegate.winlose
                                       == 0 ? Material.foreground : delegate.winlose
                                              == 1 ? "green" : "red"
                                font.underline: RPS.Client.playerId == delegate.playerId
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            Image {
                                id: rightImg
                                property var readyIcons: ["qrc:/qt/qml/RPS/rc/img/minus", "qrc:/qt/qml/RPS/rc/img/checked"]
                                property bool isWaitingStage: RPS.Client.roomStage
                                                              == Enums.GameStageWaiting
                                                              || RPS.Client.roomStage
                                                              == Enums.GameStageReadyConfirmation

                                source: (isWaitingStage ? readyIcons[delegate.ready ? 1 : 0] : delegate.sign)
                                sourceSize.width: 25
                                sourceSize.height: 25
                                layer.enabled: true
                                layer.effect: MultiEffect {
                                    colorization: 1
                                    colorizationColor: rightImg.isWaitingStage ? delegate.ready ? "green" : "red" : delegate.winlose == 0 ? Material.foreground : delegate.winlose == 1 ? "green" : "red"
                                }
                            }
                        }
                    }
                }
            }

            Item {
                RowLayout {
                    anchors.centerIn: parent

                    Repeater {
                        model: ["rock", "scissors", "paper"]

                        delegate: Item {
                            id: pickDelegate
                            required property string modelData
                            width: 75
                            height: 75

                            Image {
                                source: "qrc:/qt/qml/RPS/rc/img/" + pickDelegate.modelData
                                sourceSize.width: parent.width
                                sourceSize.height: parent.height
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    RPS.Client.requestSetSign(
                                                pickDelegate.modelData)
                                }
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            visible: RPS.Client.roomStage === Enums.GameStageWaiting
                     || RPS.Client.roomStage === Enums.GameStageReadyConfirmation
            Button {
                id: readyButton
                text: RPS.Client.ready ? "Not ready" : "Ready"
                onClicked: RPS.Client.requestSetReady(!RPS.Client.ready)
            }
        }
    }

    Connections {
        target: RPS.Client

        function onRoomInfo(info) {
            root.roomId = info.id
            root.name = info.name
            root.maxPlayers = info.maxPlayers
            root.playersCount = info.playersCount
            root.stage = info.stage
        }

        function onRoomPlayersList(players) {
            view.model.clear()
            for (var i = 0; i < players.length; ++i) {
                var obj = {
                    "playerId": players[i].playerId,
                    "name": players[i].name,
                    "ready": players[i].ready,
                    "winlose": 0,
                    "sign": ""
                }
                view.model.append(obj)
            }
        }

        function onRoomPlayerPropertyChanged(playerId, prop, value) {
            let i = root.getPlayerIndex(playerId)
            if (i !== -1) {
                view.model.setProperty(i, prop, value)
            }
        }

        function onRoomSignsList(players) {
            for (let player of players) {
                let pi = root.getPlayerIndex(player.playerId)
                if (pi != -1) {
                    view.model.setProperty(
                                pi, "sign",
                                "qrc:/qt/qml/RPS/rc/img/" + player.sign)
                    view.model.setProperty(pi, "winlose", player.winlose)
                }
            }
        }
    }
}
