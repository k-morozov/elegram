import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Messages 1.0

ColumnLayout {
    spacing: 3

    Text {
        id: chat_title
        text: "Chat title"
        font.pixelSize: 24
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        Layout.margins: 3
    }

    ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignHCenter
        Layout.margins: 10
        Layout.bottomMargin: 30
        Layout.topMargin: 30
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.interactive: true

        ListView {
            id: messages
            height: parent.height
            width: parent.width

            Component {
                id: chatDelegate
                Rectangle {
                    id: wrapper
                    width: parent.width
                    height: mesg_text.contentHeight + sender_text.contentHeight + 20

                    Column {
                        Text {
                            id: sender_text
                            text: '<b>Sender:</b> ' + backend.get_contact_name(
                                      sender_id)
                        }

                        Text {
                            id: mesg_text
                            text: '<b>Text:</b> ' + mesg
                            width: wrapper.width - 10
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }

            model: MessagesModel {
                list: messagesList
            }
            delegate: chatDelegate
        }
    }

    RowLayout {
        Layout.margins: 5
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: parent.width

        TextField {
            id: mesg_input
            placeholderText: qsTr("Write your message here")
            Layout.margins: 3
            focus: true
        }

        Button {
            id: send_mesg_btn
            text: qsTr("Send")
            Layout.margins: 3

            function sendMessageReaction(status) {
                console.log("sendMessageReaction called with result = " + status)

                if (status) {
                    reload_messages_btn.clicked()
                }
                // todo add notification about send message error
            }

            Connections {
                target: backend
                onDoSendMessageReaction: {
                    send_mesg_btn.sendMessageReaction(status)
                }
            }

            onClicked: {
                if (mesg_input.text === "") {
                    notificator.showMessage("Empty message!")
                } else if (backend.availible()) {
                    // todo change chatsState to messagesList
                    backend.sendMessage(chatsState.current_chat,
                                        mesg_input.text)
                    mesg_input.clear()
                }
            }
        }
    }

    RowLayout {
        // buttons
        Layout.alignment: Qt.AlignHCenter
        Layout.margins: 5
        spacing: 5

        BackButton {
            Layout.fillWidth: true
        }
        Button {
            id: reload_messages_btn
            text: qsTr("Reload messages")
            Layout.fillWidth: true

            function loadMessagesReaction() {
                console.log("loadMessagesReaction called")
            }

            Connections {
                target: backend
                onDoLoadMessagesReaction: {
                    reload_messages_btn.loadMessagesReaction()
                }
            }

            onClicked: {
                if (backend.availible()) {
                    messagesList.setCurrentChatId(chatsState.current_chat)

                    backend.loadMessages(chatsState.current_chat)
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Will load messages first time with chat_id = " + chatsState.current_chat)
        if (backend.availible()) {
            messagesList.setCurrentChatId(chatsState.current_chat)

            backend.loadMessages(chatsState.current_chat)
        }
    }
}
