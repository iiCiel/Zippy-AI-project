import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: contactPage
    background: Rectangle { color: "#f5f5f7" }
    MouseArea {
        anchors.fill: parent
        onClicked: contactPage.forceActiveFocus()
    }
    header: Rectangle {
        width: parent.width
        height: 60
        color: "#070c72"
        Text {
            anchors.centerIn: parent
            text: "Contact Support"
            color: "white"
            font.pixelSize: 20
            font.bold: true
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.9, 400)
        spacing: 20


        Rectangle {
            Layout.fillWidth: true
            height: 150
            radius: 15
            color: "white"
            border.color: "#e0e0e0"

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 10

                Text { text: "College of Business IT"; font.bold: true; font.pixelSize: 18; color: "#070c72" }
                Rectangle { height: 1; width: 100; color: "#eee" } // Separator
                Text { text: "zipAIsupport@uakron.edu"; font.pixelSize: 16 }
                Text { text: "(330) 123-4567"; font.pixelSize: 16 }
                Text { text: "Room 107"; font.pixelSize: 16 }
            }
        }

        Text {
            text: "Send a quick message:"
            font.bold: true
            color: "#555"
            Layout.topMargin: 20
        }

        TextArea {

            id: messageInput

            Layout.fillWidth: true
            Layout.preferredHeight: 100
            placeholderText: "Type your issue here... (Enter to send)"
            wrapMode: Text.Wrap

            background: Rectangle {
                color: "white"
                border.color: "#ccc"
                radius: 8
            }


            Keys.onReturnPressed: (event) => {

                                      if ((event.modifiers & Qt.ShiftModifier) == 0) {
                                          submitButton.clicked()
                                          event.accepted = true
                                      } else {
                                          event.accepted = false
                                      }
                                  }
        }

        Button {
            id: submitButton
            text: "Submit Ticket"
            Layout.fillWidth: true
            Layout.preferredHeight: 45

            onClicked: {
                if (messageInput.text.trim() !== "") {
                    console.log("Message submitted: " + messageInput.text)
                    messageInput.text = ""
                }
            }

            background: Rectangle {
                color: parent.down ? "#050950" : "#070c72"
                radius: 8
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
