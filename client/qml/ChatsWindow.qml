import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// chatsList object created and registered in main.cpp
import Chats 1.0

ColumnLayout {
    spacing: 5
    Layout.margins: 5

    Text {
        text: "Chats"
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
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.interactive: true

        ListView {
            Layout.margins: 10

            Component {
                id: chatsDelegate
                Rectangle {
                    id: wrapper
                    width: parent.width - 2
                    height: 60

                    Column {
                        Text {
                            text: ' <b>Chat title:</b> ' + chat_title
                            font.pixelSize: 16
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            chatsState.current_chat = chat_id
                            console.log("current_chat_id = " + chatsState.current_chat)
                            stackView.push("MessagesWindow.qml")
                        }
                    }
                }
            }

            model: ChatsModel {
                list: chatsList
            }
            delegate: chatsDelegate
            highlight: Rectangle {
                color: "lightsteelblue"
                radius: 5
            }
            focus: true
        }
    }

    ColumnLayout {
        // buttons
        Layout.alignment: Qt.AlignHCenter
        Layout.margins: 5
        spacing: 5

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            Button {
                id: contacts_btn
                text: qsTr("Contacts")
                Layout.fillWidth: true

                onClicked: {
                    stackView.push("ContactsWindow.qml")
                }
            }

            Button {
                id: reaload_chats_btn
                text: qsTr("Reload chats")
                Layout.fillWidth: true

                function loadChatsReaction() {
                    console.log("loadChatsReaction() called")
                }

                Connections {
                    target: backend
                    onDoLoadChatsReaction: {
                        reaload_chats_btn.loadChatsReaction()
                    }
                }

                onClicked: {
                    if (backend.availible()) {
                        backend.loadChats() // todo reload ChatsModel
                    }
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            Button {
                id: settings_btn
                text: qsTr("Settings")
                Layout.fillWidth: true

                onClicked: {
                    stackView.push("SettingsWindow.qml")
                }
            }

            Button {
                id: profile_btn
                text: qsTr("Profile")
                Layout.fillWidth: true

                onClicked: {
                    stackView.push("ProfileWindow.qml")
                }
            }
        }
    }

    Item {
        // preload
        id: pageUtils
        function loadDataReaction() {
            console.log("loadDataReaction() called")
        }

        Connections {
            target: backend
            onDoLoadDataReaction: {
                pageUtils.loadDataReaction()
            }
        }

        Component.onCompleted: {
            console.log("Will load data (contacts and chats), backend will be locked")
            if (backend.availible()) {
                backend.loadData()
            }
        }
    }
}
