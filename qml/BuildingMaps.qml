import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    background: Rectangle { color: "#f5f5f7" }

    header: Rectangle {
        width: parent.width
        height: 60
        color: "#070c72"
        Text {
            anchors.centerIn: parent
            text: "Building Maps"
            color: "white"
            font.pixelSize: 20
            font.bold: true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        TabBar {
            id: mapTabs
            Layout.fillWidth: true
            background: Rectangle { color: "transparent" }

            TabButton { text: "Floor 1" }
            TabButton { text: "Floor 2" }
            TabButton { text: "Floor 3" }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"
            radius: 15
            border.color: "#d0d0d0"

            Image {
                anchors.centerIn: parent
                // source: "qrc:/images/example.png"
                source: ""
                fillMode: Image.PreserveAspectFit

                // Fallback text if no image
                Text {
                    anchors.centerIn: parent
                    visible: parent.status !== Image.Ready
                    text: "Map Image Placeholder\n" + mapTabs.currentItem.text
                    horizontalAlignment: Text.AlignHCenter
                    color: "#888"
                }
            }
        }
    }
}
