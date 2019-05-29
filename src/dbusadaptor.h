#ifndef DBUSADAPTOR_H
#define DBUSADAPTOR_H

#include <QDBusAbstractAdaptor>

class QTimer;
class DBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.searchengines")
public:
    explicit DBusAdaptor(QObject *parent = nullptr);

public slots:
    void installSearchEngine(const QString &filename, const QString &data);
    void removeSearchEngine(const QString &filename);

private:
    QTimer *m_timer;
};

#endif // DBUSADAPTOR_H
