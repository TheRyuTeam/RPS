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
                text: "Room name"
            }

            TextField {
                id: name
            }

            Label {
                text: "Max players"
            }

            TextField {
                id: maxPlayers
                text: "2"
                validator: IntValidator {
                    bottom: 2
                    top: 16
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Button {
            Layout.alignment: Qt.AlignRight
            text: "Create"
            onClicked: RPS.Client.requestCreateRoom(name.text, maxPlayers.text)
        }
    }
}
