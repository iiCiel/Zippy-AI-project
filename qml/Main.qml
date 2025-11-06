import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window
    width: 640
    height: 550
    visible: true
    title: qsTr("Zippy AI")

    // Data model to store chat messages
    ListModel {
        id: chatModel
    }

    Rectangle {
        anchors.fill: parent
        color: "#070c72"
    }

    Component.onCompleted: {
        controller.pingOllama();
    }

    ColumnLayout {
        id: mainLayout
        spacing: 0
        anchors.fill: parent
        anchors.bottomMargin: inputPanel.active ? inputPanel.height : 0
        property bool isGenerating: false
        Behavior on anchors.bottomMargin {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#05094d"
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    text: "💬 Zippy AI — College of Business Assistant"
                    color: "white"
                    font.pixelSize: 18
                    font.bold: true
                    Layout.fillWidth: true
                }
                Button {
                    id: configButton
                    text: "⚙"
                    font.pixelSize: 20
                    Layout.preferredWidth: 45
                    Layout.preferredHeight: 40
                    onClicked: {
                        const component = Qt.createComponent("OllamaConfig.qml")
                        const win = component.createObject()
                        if (win) win.show()
                    }
                    background: Rectangle {
                        color: configButton.hovered ? "#0a0f8f" : "transparent"
                        radius: 8
                    }
                }
            }
        }

        // Chat area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "white"

            ListView {
                id: chatListView
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12
                clip: true
                model: chatModel
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
                delegate: Item {
                    width: chatListView.width
                    height: messageBubble.height + 8
                    Row {
                        anchors.right: model.isUser ? parent.right : undefined
                        anchors.left: model.isUser ? undefined : parent.left
                        spacing: 8
                        Rectangle {
                            id: messageBubble
                            width: Math.min(messageText.implicitWidth + 24, chatListView.width * 0.75)
                            height: messageText.implicitHeight + 20
                            radius: 18
                            color: model.isUser ? "#007AFF" : "#3a3a3c"
                            Text {
                                id: messageText
                                text: model.message
                                color: "white"
                                wrapMode: Text.Wrap
                                anchors.fill: parent
                                anchors.margins: 12
                                font.pixelSize: 16
                            }
                        }
                    }
                }
                onCountChanged: {
                    Qt.callLater(positionViewAtEnd)
                }
            }


            RowLayout {
                visible: chatModel.count === 0
                anchors.centerIn: parent
                spacing: 40
                width: parent.width * 0.8
                height: 250
                Layout.alignment: Qt.AlignHCenter

                Image {
                    id: zippyLogo
                    source: "qrc:/images/ZippyAILogo.png"
                    Layout.preferredWidth: 300
                    Layout.preferredHeight: 300
                    fillMode: Image.PreserveAspectFit
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    smooth: true
                    mipmap: true
                    antialiasing: true
                }

                Text {
                    text: "ZIPPY AI\nCOLLEGE OF BUSINESS"
                    color: "#070c72"
                    font.pixelSize: 32
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    lineHeight: 1.2
                    Layout.fillWidth: true

                }
            }
        }

        // Input area
        Rectangle {
            id: inputBar
            Layout.fillWidth: true
            Layout.preferredHeight: 85
            color: "#1a1f6b"
            RowLayout {

                anchors.fill: parent
                anchors.margins: 15
                spacing: 12
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 55
                    radius: 27.5
                    color: "#1a1f6b"
                    border.color: mainLayout.isGenerating ? "#666" : "#4a4f9b"
                    border.width: 2
                    TextField {
                        id: inputField
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        enabled: !mainLayout.isGenerating
                        color: "white"
                        placeholderText: "Ask Zippy anything..."
                        font.pixelSize: 16
                        placeholderTextColor: "#ffffff66"
                        verticalAlignment: TextInput.AlignVCenter
                        background: Rectangle {
                            color: "transparent"
                        }
                        onAccepted: {
                            sendButton.clicked()
                        }
                        Connections {
                            target: controller
                            function onGenerateFinished(response) {
                                if (chatModel.count > 0) {
                                    var lastIndex = chatModel.count - 1
                                    var lastMsg = chatModel.get(lastIndex)
                                    if (!lastMsg.isUser) {
                                        chatModel.setProperty(lastIndex, "message", lastMsg.message + response)
                                    }
                                }
                            }
                            function onStreamFinished() {
                                mainLayout.isGenerating = false
                            }
                        }
                    }
                }
                Button {
                    id: sendButton
                    text: "↑"
                    enabled: !mainLayout.isGenerating && inputField.text.trim() !== ""
                    Layout.preferredWidth: 55
                    Layout.preferredHeight: 55
                    font.pixelSize: 24
                    font.bold: true
                    onClicked: {
                        if (inputField.text.trim() !== "") {
                            mainLayout.isGenerating = true
                            chatModel.append({
                                message: inputField.text,
                                isUser: true
                            })
                            chatModel.append({
                                message: "",
                                isUser: false
                            })
                            controller.generate(inputField.text)
                            inputField.text = ""
                            chatListView.forceActiveFocus()
                        }
                    }
                    background: Rectangle {
                        radius: 27.5
                        color: sendButton.enabled ? "#007AFF" : "#3a3a3c"
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    contentItem: Text {
                        text: sendButton.text
                        font: sendButton.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // Navigation Bar
        Rectangle {
            id: navigationBar
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: "#05094d"

            RowLayout { //ROW FOR THE BUTTONS. THESE ARE THE SPACING BETWEEN BUTTONS
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12

                Button {
                    text: "Home Page"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    font.pixelSize: 14
                    background: Rectangle {
                        color: parent.down ? "#0056b3" : (parent.hovered ? "#0069d9" : "#007AFF") // Blue
                        radius: 8
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    text: "Building Maps"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    font.pixelSize: 14
                    background: Rectangle {
                        color: parent.down ? "#0056b3" : (parent.hovered ? "#0069d9" : "#007AFF") // Blue
                        radius: 8
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    text: "Events"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    font.pixelSize: 14
                    background: Rectangle {
                        color: parent.down ? "#0056b3" : (parent.hovered ? "#0069d9" : "#007AFF") // Blue
                        radius: 8
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    text: "Contact"
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    font.pixelSize: 14
                    background: Rectangle {
                        color: parent.down ? "#0056b3" : (parent.hovered ? "#0069d9" : "#007AFF") // Blue
                        radius: 8
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }


    }

    InputPanel {
        id: inputPanel
        z: 99
        x: 0
        y: window.height
        width: window.width
        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: window.height - inputPanel.height
            }
        }
        transitions: Transition {
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
}
