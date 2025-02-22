import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import RPS as RPS

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480

    Material.theme: Material.Dark

    ColumnLayout {
        anchors.fill: parent

        RPS.Header {
            Layout.fillWidth: true
            Layout.minimumHeight: 50
            Layout.maximumHeight: 50
        }

        RPS.Menu {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Component.onCompleted: {
        RPS.Client.serverAddress = "127.0.0.1:46789"
    }
}
