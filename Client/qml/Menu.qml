import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import RPS as RPS

Item {
    StackLayout {
        anchors.fill: parent
        currentIndex: RPS.Client.inRoom

        RowLayout {
            clip: true

            Column {
                id: menuBar
                property int currentIndex: 0
                Layout.fillHeight: true
                Layout.minimumWidth: 150

                Repeater {
                    model: ["Create room", "Rooms", "Settings"]
                    property int currentIndex: 0

                    delegate: ItemDelegate {
                        required property string modelData
                        required property int index
                        width: parent.width
                        text: modelData
                        onClicked: menuBar.currentIndex = index
                        highlighted: menuBar.currentIndex == index
                    }
                }
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: menuBar.currentIndex

                RPS.CreateRoomMenu {}

                RPS.RoomsMenu {}

                RPS.SettingsMenu {}
            }
        }

        RPS.Room {}
    }
}
