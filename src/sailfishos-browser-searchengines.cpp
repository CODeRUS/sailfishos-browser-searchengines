#include <QtQuick>
#include <sailfishapp.h>

#include <QTimer>
#include <QDebug>

#include <QtDBus>

#include "dbusadaptor.h"
#include "searchenginesmodel.h"

static const QString s_dbusObject = QStringLiteral("/org/coderus/searchengines");
static const QString s_dbusService = QStringLiteral("org.coderus.searchengines");
static const QString s_dbusInterface = QStringLiteral("org.coderus.searchengines");

void initializeDBus()
{
    new DBusAdaptor(qApp);

    QDBusConnection bus = QDBusConnection::systemBus();
    const bool registerObject = bus.registerObject(s_dbusObject, s_dbusInterface, qApp);
    qDebug() << Q_FUNC_INFO << "registerObject" << registerObject;
    if (!registerObject) {
        qWarning() << Q_FUNC_INFO << bus.lastError().message();
        qApp->quit();
    }
    const bool registerService = bus.registerService(s_dbusService);
    qDebug() << Q_FUNC_INFO << "registerService" << registerService;
    if (!registerService) {
        qWarning() << Q_FUNC_INFO << bus.lastError().message();
        qApp->quit();
    }
}

int runDBusApp(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTimer::singleShot(0, &initializeDBus);
    return app.exec();
}

int runSailfishApp(int argc, char *argv[])
{
    qmlRegisterType<SearchEnginesModel>("org.coderus.searchengines", 1, 0, "SearchEnginesModel");

    return SailfishApp::main(argc, argv);
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "dbus") == 0) {
        return runDBusApp(argc, argv);
    } else {
        return runSailfishApp(argc, argv);
    }
}
