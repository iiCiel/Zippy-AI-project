import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

Window {
    id: window
    width: 750
    height: 550
    visible: true
    title: qsTr("Zippy AI")

    // Data model to store chat messages
    // Kept at Window level so chat persists when navigating between tabs
    ListModel {
        id: chatModel
    }

    Rectangle {
        anchors.fill: parent
        color: "#070c72"
    }

    Component.onCompleted: {
        // Safe check ensures UI loads even if C++ controller isn't ready
        if (typeof controller !== "undefined") {
            controller.pingOllama();
        }
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

        // ===== HEADER BAR (Always Visible) =====
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#070c72"
            z: 10
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

        // ===== CONTENT AREA (StackView) =====
        // This handles switching between Chat, Maps, Events, etc.
        StackView {
            id: contentStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            initialItem: homePage // Default to the Chat Component

            replaceEnter: Transition { PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 200 } }
            replaceExit: Transition { PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 200 } }
        }

        // ===== FOOTER NAV BAR (Always Visible) =====
        Rectangle {
            id: navigationBar
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: "#070c72"
            z: 10

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12

                // Reusable Nav Button Component
                component NavButton: Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    font.pixelSize: 14
                    background: Rectangle {
                        color: parent.down ? "#4040ff" : (parent.hovered ? "#2323ff" : "#1a1f6b")
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

                NavButton {
                    text: "Home Page"
                    onClicked: contentStack.replace(homePage)
                }

                NavButton {
                    text: "Building Maps"
                    // Loads external BuildingMaps.qml
                    onClicked: contentStack.replace("BuildingMaps.qml")
                }

                NavButton {
                    text: "Events"
                    onClicked: contentStack.replace("Events.qml")
                }

                NavButton {
                    text: "Contact"
                    onClicked: contentStack.replace("Contact.qml")
                }
            }
        }
    }

    // =============================================
    // ===== COMPONENT: HOME PAGE (Chat Logic) =====
    // =============================================
    Component {
        id: homePage

        ColumnLayout {
            spacing: 0

            // Chat Area
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#fffaa0" }
                    GradientStop { position: 1.0; color: "#2323ff" }
                }

                ListView {
                    id: chatListView
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 12
                    clip: true
                    model: chatModel

                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                    delegate: Item {
                        width: chatListView.width
                        height: Math.max(messageBubble.height + 8, 50 + 8)

                        property real avatarTopY: height - 8 - 50

                        Row {
                            anchors.right: model.isUser ? parent.right : undefined
                            anchors.left: model.isUser ? undefined : parent.left
                            spacing: 8

                            // Avatar
                            Item {
                                visible: !model.isUser
                                width: 50; height: 50
                                anchors.bottom: parent.bottom
                                Rectangle {
                                    anchors.fill: parent
                                    radius: 20
                                    color: "white"
                                    clip: true
                                    border.color: "#e0e0e0"
                                    border.width: 1

                                    Image {
                                        source: "qrc:/images/ZippyAvatar.png"
                                        anchors.centerIn: parent
                                        width: parent.width - 4; height: parent.height - 4
                                        fillMode: Image.PreserveAspectFit

                                        // High Quality Settings
                                        smooth: true
                                        mipmap: true
                                        antialiasing: true
                                    }
                                }
                            }

                            // Message Bubble
                            Rectangle {
                                id: messageBubble
                                width: Math.min(messageText.implicitWidth + 24, chatListView.width * 0.75)
                                height: messageText.implicitHeight + 20
                                radius: 18
                                color: model.isUser ? "#80007AFF" : "#803a3a3c"
                                border.color: model.isUser ? "#99FFFFFF" : "#77FFFFFF"
                                border.width: 1.5

                                // Glass effects
                                Rectangle {
                                    anchors.fill: parent; anchors.margins: 1; radius: parent.radius - 1
                                    color: "transparent"; border.color: "#33FFFFFF"; border.width: 1
                                }
                                Rectangle {
                                    width: parent.width - 4; height: parent.height * 0.5
                                    anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter
                                    radius: parent.radius - 2
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: "#50FFFFFF" }
                                        GradientStop { position: 1.0; color: "#00FFFFFF" }
                                    }
                                }

                                Text {
                                    id: messageText
                                    text: model.message
                                    textFormat: Text.MarkdownText
                                    color: "white"
                                    wrapMode: Text.Wrap
                                    anchors.fill: parent; anchors.margins: 12
                                    font.pixelSize: 16
                                    onLinkActivated: (link) => Qt.openUrlExternally(link)
                                }
                            }
                        }

                        // Thinking Indicator Bubble
                        Item {
                            id: zippyThinkingIndicator
                            // Visible if: Not User AND System Generating AND Last Message AND Message Empty
                            visible: !model.isUser && mainLayout.isGenerating && index === chatModel.count - 1 && model.message === ""

                            width: 30
                            height: 25
                            z: 100

                            x: 15 + 50 - 30 - 5 // Position adjustment based on avatar size
                            y: avatarTopY - 15

                            Rectangle {
                                anchors.centerIn: parent
                                width: 25; height: 25; radius: 12.5
                                color: "white"
                                border.color: "#888"; border.width: 1

                                RowLayout {
                                    anchors.centerIn: parent
                                    spacing: 2
                                    Repeater {
                                        model: 3
                                        delegate: Rectangle {
                                            width: 4; height: 4; radius: 2
                                            color: "#3a3a3c"
                                        }
                                    }
                                }
                            }
                        }
                    } // End Delegate

                    onCountChanged: Qt.callLater(positionViewAtEnd)
                }

                // Hero Screen (Only Shows When Chat Is Empty)
                RowLayout {
                    visible: chatModel.count === 0
                    anchors.centerIn: parent
                    spacing: 40
                    width: parent.width * 0.8
                    height: 250

                    Image {
                        source: "qrc:/images/ZippyAILogo.png"
                        Layout.preferredWidth: 300; Layout.preferredHeight: 300
                        fillMode: Image.PreserveAspectFit

                        // High Quality Settings
                        smooth: true
                        mipmap: true
                        antialiasing: true
                    }

                    Text {
                        text: "ZIPPY AI\nCOLLEGE OF BUSINESS"
                        color: "#070c72"
                        font.pixelSize: 32; font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        lineHeight: 1.2
                        Layout.fillWidth: true
                    }
                }
            }

            // Input Bar
            Rectangle {
                id: inputBar
                Layout.fillWidth: true
                Layout.preferredHeight: 85
                color: "#070c72"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 12

                    Button {
                        id: clearChatButton
                        text: "Clear Chat"
                        Layout.preferredWidth: 110; Layout.preferredHeight: 55
                        font.bold: true
                        enabled: chatModel.count > 0
                        onClicked: chatModel.clear()
                        background: Rectangle {
                            radius: 27.5
                            color: clearChatButton.enabled ? "#8B0000" : "#5a5a5a"
                        }
                        contentItem: Text {
                            text: clearChatButton.text
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 55
                        radius: 27.5
                        color: "#1a1f6b"
                        border.color: mainLayout.isGenerating ? "#666" : "#4a4f9b"
                        border.width: 2

                        TextField {
                            id: inputField
                            anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20
                            enabled: !mainLayout.isGenerating
                            color: "white"
                            placeholderText: "Ask Zippy anything..."
                            placeholderTextColor: "#ffffff66"
                            verticalAlignment: TextInput.AlignVCenter
                            background: Rectangle { color: "transparent" }
                            onAccepted: sendButton.clicked()

                            Connections {
                                target: (typeof controller !== "undefined") ? controller : null
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
                        Layout.preferredWidth: 55; Layout.preferredHeight: 55
                        font.pixelSize: 24
                        onClicked: {
                            if (inputField.text.trim() !== "") {
                                mainLayout.isGenerating = true
                                chatModel.append({ message: inputField.text, isUser: true })
                                chatModel.append({ message: "", isUser: false })
                                if (typeof controller !== "undefined") controller.generate(inputField.text)
                                inputField.text = ""
                                chatListView.forceActiveFocus()
                            }
                        }
                        background: Rectangle {
                            radius: 27.5
                            color: sendButton.enabled ? "#007AFF" : "#3a3a3c"
                        }
                        contentItem: Text {
                            text: sendButton.text; color: "white"
                            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }
    }

    // ===== MOBILE KEYBOARD HANDLING =====
    InputPanel {
        id: inputPanel
        z: 99
        anchors.horizontalCenter: parent.horizontalCenter
        y: window.height
        width: window.width * .90

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
