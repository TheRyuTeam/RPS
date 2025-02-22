import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import RPS as RPS

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            columns: 2

            Label {
                text: "Player name"
            }

            TextField {
                id: playerName
                enabled: RPS.Client.connected
                text: RPS.Client.playerName
            }

            Label {
                text: "Server address"
            }

            TextField {
                id: serverAddress
                text: RPS.Client.serverAddress
                validator: RegularExpressionValidator {
                    regularExpression: /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5]):([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$/
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Button {
            Layout.alignment: Qt.AlignRight
            text: "Save"
            onClicked: {
                RPS.Client.playerName = playerName.text
                RPS.Client.serverAddress = serverAddress.text
            }
        }
    }
}
