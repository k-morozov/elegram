import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Contacts 1.0

ColumnLayout {
    spacing: 3

    Text {
        text: "Contacts"
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
            Layout.margins: 20

            Component {
                id: contactsDelegate
                Rectangle {
                    id: wrapper
                    width: parent.width - 2
                    height: 60

                    RowLayout {
                        width: parent.width

                        Column {
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            Text {
                                text: '<b>Id:</b> ' + user_id
                            }
                            Text {
                                text: '<b>Name:</b> ' + name
                            }
                            Text {
                                text: '<b>Email:</b> ' + email
                            }
                        }

                        Button {
                            id: createChatBtn
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            Layout.rightMargin: 10
                            text: qsTr("Create chat")

                            function createChatReaction(isCreated) {
                                console.log("createChatReaction called")
                                if (isCreated) {
                                    notificator.showMessage("Chat created!")
                                    backend.loadChats()
                                } else {
                                    notificator.showMessage(
                                                "Chat NOT created! Maybe already exist or something else  ¯\_(ツ)_/¯ ")
                                }
                            }

                            Connections {
                                target: backend
                                onDoChatCreatedReaction: {
                                    createChatBtn.createChatReaction(isCreated)
                                }
                            }

                            onClicked: {
                                if (backend.availible()) {
                                    backend.createChat(user_id)
                                }
                            }
                        }
                    }
                }
            }

            model: ContactsModel {
                list: contactsList
            }
            delegate: contactsDelegate
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
            Button {
                id: add_contacts_btn
                text: qsTr("Add new contact")
                Layout.fillWidth: true

                onClicked: {
                    stackView.push("AddContactWindow.qml")
                }
            }

            Button {
                id: reload_contacts_btn
                text: qsTr("Reload сontacts")
                Layout.fillWidth: true

                function loadContactsReaction() {
                    console.log("loadContactsReaction() called")
                }

                Connections {
                    target: backend
                    onDoLoadContactsReaction: {
                        reload_contacts_btn.loadContactsReaction()
                    }
                }

                onClicked: {
                    if (backend.availible()) {
                        backend.loadContacts()
                    }
                }
            }
        }

        BackButton {
            Layout.fillWidth: true
        }
    }

    Component.onCompleted: {
        console.log("Will load contacts, backend will be locked")
        if (backend.availible()) {
            backend.loadContacts()
        }
    }
}
