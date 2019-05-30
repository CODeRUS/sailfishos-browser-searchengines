#include "dbusadaptor.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QFile>

static const QString s_directory = QStringLiteral("/usr/lib/mozembedlite/chrome/embedlite/content/");

DBusAdaptor::DBusAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(5000);
    connect(m_timer, &QTimer::timeout, [](){
        qApp->quit();
    });
}

void DBusAdaptor::installSearchEngine(const QString &filename, const QString &data)
{
    qDebug() << Q_FUNC_INFO << filename << data;
    m_timer->stop();

    QFile file(s_directory + filename);
    if (!file.open(QFile::WriteOnly)) {
        m_timer->start();
        return;
    }

    file.write(data.toUtf8());

    m_timer->start();
}

void DBusAdaptor::removeSearchEngine(const QString &filename)
{
    m_timer->stop();

    qDebug() << Q_FUNC_INFO << filename <<
    QFile::remove(s_directory + filename);

    m_timer->start();
}
