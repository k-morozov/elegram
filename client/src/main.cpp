#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QNetworkConfigurationManager>
#include <QSettings>
#include <QDebug>
#include <QNetworkSession>

#include "backend_facade.h"

#include "model/chatsmodel.h"
#include "model/chatslist.h"
#include "model/messagesmodel.h"
#include "model/messageslist.h"
#include "model/contactsmodel.h"
#include "model/contactslist.h"

int main(int argc, char *argv[]) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        qDebug() << "Get saved network configuration";
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        QNetworkSession *networkSession = new QNetworkSession(config, &app);
        networkSession->open();
        networkSession->waitForOpened();

        if (networkSession->isOpen()) {
            // Save the used configuration
            qDebug() << "Save the used configuration";
            QNetworkConfiguration config = networkSession->configuration();
            QString id;
            if (config.type() == QNetworkConfiguration::UserChoice) {
                id = networkSession->sessionProperty(
                    QLatin1String("UserChoiceConfiguration")).toString();
            } else {
                id = config.identifier();
            }

            QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
            settings.beginGroup(QLatin1String("QtNetwork"));
            settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
            settings.endGroup();
        }
    }

    qmlRegisterUncreatableType<elegram::BackEndFacade>("BackEnd", 1, 0, "BackEnd",
                                                       QStringLiteral("BackEnd should not be created in QML"));

    qmlRegisterType<elegram::ChatsModel>("Chats", 1, 0, "ChatsModel");
    qmlRegisterUncreatableType<elegram::ChatsList>("Chats", 1, 0, "ChatsList",
                                                   QStringLiteral("ChatsList should not be created in QML"));

    qmlRegisterType<elegram::ContactsModel>("Contacts", 1, 0, "ContactsModel");
    qmlRegisterUncreatableType<elegram::ContactsList>("Contacts", 1, 0, "ContactsList",
                                                      QStringLiteral("ContactsList should not be created in QML"));

    qmlRegisterType<elegram::MessagesModel>("Messages", 1, 0, "MessagesModel");
    qmlRegisterUncreatableType<elegram::MessagesList>("Messages", 1, 0, "MessagesList",
                                                      QStringLiteral("MessagesList should not be created in QML"));

    elegram::ChatsList chatsList;
    elegram::ContactsList contactsList;
    elegram::MessagesList messagesList;
    elegram::BackEndFacade backend(&chatsList, &contactsList, &messagesList);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("chatsList"), &chatsList);
    engine.rootContext()->setContextProperty(QStringLiteral("contactsList"), &contactsList);
    engine.rootContext()->setContextProperty(QStringLiteral("messagesList"), &messagesList);
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "engine.rootObjects().isEmpty() returned true";
        return -1;
    }

    qDebug() << "app.exec()";
    return app.exec();
}
