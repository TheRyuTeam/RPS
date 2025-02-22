import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import RPS as RPS

Item {
    Pane {
        anchors.fill: parent

        Material.elevation: 6

        RowLayout {
            anchors.fill: parent

            Label {
                text: RPS.Client.playerName + "  @" + RPS.Client.playerId
                elide: Text.ElideRight
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                property var names: ["", "Waiting ", "Ready confirmation ", "Pick ", "Result "]
                visible: RPS.Client.roomStageRemaining != -1
                text: names[RPS.Client.roomStage] + RPS.Client.roomStageRemaining / 1000
                elide: Text.ElideRight
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                enabled: RPS.Client.connected
                text: RPS.Client.inRoom ? "Room: " + RPS.Client.roomName : RPS.Client.serverAddress
                elide: Text.ElideRight
            }

            Item {
                visible: RPS.Client.inRoom
                implicitWidth: 25
                implicitHeight: 25

                Image {
                    source: "qrc:/qt/qml/RPS/rc/img/exit"
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: RPS.Client.requestLeaveRoom()
                }
            }
        }
    }
}
