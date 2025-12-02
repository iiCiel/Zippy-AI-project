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
            text: "Upcoming Events"
            color: "white"
            font.pixelSize: 20
            font.bold: true
        }
    }


    ListModel {
        id: eventModel
        ListElement { title: "Career Fair 2024"; date: "OCT 12"; time: "10:00 AM"; location: "Grand Hall" }
        ListElement { title: "AI in Business Talk"; date: "OCT 15"; time: "2:00 PM"; location: "Room 304" }
        ListElement { title: "Alumni Networking"; date: "NOV 01"; time: "6:00 PM"; location: "Student Union" }
    }

    ListView {
        id: eventList
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        model: eventModel
        clip: true

        delegate: Rectangle {
            width: eventList.width
            height: 100
            radius: 12
            color: "white"
            border.color: "#e0e0e0"

            RowLayout {
                anchors.fill: parent
                spacing: 0

                // Date Box
                Rectangle {
                    Layout.preferredWidth: 80
                    Layout.fillHeight: true
                    color: "#070c72"


                    radius: 12
                    Rectangle {
                        width: 12; height: parent.height
                        anchors.right: parent.right
                        color: "#070c72"
                    }

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 2
                        Text {
                            text: model.date.split(" ")[0]
                            color: "white"; font.pixelSize: 12
                            Layout.alignment: Qt.AlignHCenter
                        }
                        Text {
                            text: model.date.split(" ")[1]
                            color: "white"; font.pixelSize: 22; font.bold: true
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }


                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.margins: 15
                    spacing: 5

                    Text {
                        text: model.title
                        font.bold: true; font.pixelSize: 18
                        color: "#333"
                    }
                    Text {
                        text: "🕒 " + model.time + "  📍 " + model.location
                        color: "#666"; font.pixelSize: 14
                    }
                }
            }
        }
    }
}
