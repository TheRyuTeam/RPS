import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import RPS as RPS
import Ryu

Item {
    id: root
    implicitWidth: 350

    RowLayout {
        anchors.fill: parent

        ListView {
            id: view
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 300

            spacing: 5
            focus: true

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            delegate: Item {
                id: delegate
                required property string roomId
                required property string name
                required property int maxPlayers
                required property int playersCount
                required property int stage
                enabled: maxPlayers != playersCount
                         && (stage == Enums.GameStageWaiting
                             || stage == Enums.GameStageReadyConfirmation)

                width: view.width
                height: 45

                function getStageName(stage) {
                    var names = ["Invalid stage", "Waiting stage", "Ready stage", "Pick stage", "Result stage"]
                    return stage >= names.length ? names[0] : names[stage]
                }

                Pane {
                    anchors.fill: parent
                    Material.elevation: delegateMA.containsMouse ? 8 : 4

                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20

                        Label {
                            width: parent.width * 0.45
                            height: parent.height
                            verticalAlignment: Qt.AlignVCenter
                            text: delegate.name + "  @" + delegate.roomId
                            elide: Text.ElideRight
                        }

                        Label {
                            width: parent.width * 0.45
                            height: parent.height
                            verticalAlignment: Qt.AlignVCenter
                            text: delegate.getStageName(delegate.stage)
                            elide: Text.ElideRight
                        }

                        Label {
                            width: parent.width * 0.10
                            height: parent.height
                            verticalAlignment: Qt.AlignVCenter
                            text: delegate.playersCount + '/' + delegate.maxPlayers
                            elide: Text.ElideRight
                        }
                    }
                }

                MouseArea {
                    id: delegateMA
                    anchors.fill: parent
                    onDoubleClicked: RPS.Client.requestEnterRoom(
                                         delegate.roomId)
                    hoverEnabled: true
                }
            }
        }

        Column {
            Layout.fillHeight: true

            Item {
                enabled: RPS.Client.connected
                width: 25
                height: 25

                Image {
                    source: "qrc:/qt/qml/RPS/rc/img/refresh.png"
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: RPS.Client.requestRoomsList()
                }
            }
        }
    }

    Connections {
        target: RPS.Client

        function onConnectedChanged(connected) {
            if (connected) {
                RPS.Client.requestRoomsList()
            } else {
                view.model = []
            }
        }

        function onRoomsList(rooms) {
            view.model = rooms
        }
    }
}
