import QtQuick 2.12
import QtQuick.Controls 2.12

// backend object created and registered in main.cpp
import BackEnd 1.0

ApplicationWindow {
    id: window
    width: 350
    height: 520
    visible: true
    title: qsTr("Elegram Client")

    onClosing: {
        Qt.quit()
    }

    Item {
        id: chatsState
        property int current_chat
    }

    SimplePopup {
        id: popup
    }

    Item {
        id: notificator

        function operationAborted(errorMessage) {
            console.log("operationAborted(mesg) called")
            popup.contentItem.text = errorMessage
            popup.open()
        }

        function showMessage(message) {
            console.log("showMessage(mesg) called")
            popup.contentItem.text = message
            popup.open()
        }

        Connections {
            target: backend
            onDoOperationAborted: {
                notificator.operationAborted(errorMessage)
            }
            onDoShowMessage: {
                notificator.showMessage(message)
            }
        }
    }

    Component {
        id: login_window
        LoginWindow {
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        // Implements back key navigation
        focus: true
        Keys.onReleased: if (event.key === Qt.Key_Back && stackView.depth > 1) {
                             stackView.pop()
                             event.accepted = true
                         }

        initialItem: login_window

        Connections {
            target: backend
            onDoDisconnected: {
                console.log("onDoDisconnected called")
                stackView.clear()
                stackView.push(login_window)
            }
        }
    }
}
