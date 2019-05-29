#include "searchenginesmodel.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QXmlQuery>

static const QString s_directory = QStringLiteral("/usr/lib/mozembedlite/chrome/embedlite/content/");
static const QString s_extension = QStringLiteral(".xml");
static const QString s_regexpRemover = QStringLiteral("[^\\w]");

SearchEnginesModel::SearchEnginesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    beginResetModel();
    const QDir dir(s_directory);
    for (const QString &filename : dir.entryList({QStringLiteral("*%1").arg(s_extension)})) {
        bool ok = true;
        const ModelData data = loadModelData(s_directory + filename, &ok);
        if (ok) {
            m_modelData.append(data);
        }
    }
    endResetModel();
}

int SearchEnginesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_modelData.count();
}

QVariant SearchEnginesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_modelData.count())
        return QVariant();
    switch (role) {
    case TitleRole:
        return m_modelData.at(index.row()).title;
    case FilenameRole:
        return m_modelData.at(index.row()).filename;
    default:
        qWarning() << Q_FUNC_INFO << "Unexpected role:" << role;
        return QVariant();
    }
}

QHash<int, QByteArray> SearchEnginesModel::roleNames() const
{
    static const QHash<int, QByteArray> r = {
        { TitleRole, QByteArrayLiteral("title") },
        { FilenameRole, QByteArrayLiteral("filename") },
    };
    return r;
}

void SearchEnginesModel::add(const QString &title, const QString &data)
{
    QString name = title.trimmed().toLower();
    name.replace(QRegExp(s_regexpRemover), QString());
    const QString filename = s_directory + name + s_extension;

    for (const ModelData &modelData : m_modelData) {
        if (modelData.filename == filename) {
            return;
        }
    }

    beginInsertRows(QModelIndex(), m_modelData.count(), m_modelData.count());

    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.coderus.searchengines"),
                                                      QStringLiteral("/org/coderus/searchengines"),
                                                      QStringLiteral("org.coderus.searchengines"),
                                                      QStringLiteral("installSearchEngine"));
    msg.setArguments({filename, data});
    qDebug() << Q_FUNC_INFO << filename << data <<
    QDBusConnection::systemBus().call(msg);

    bool ok = false;
    const ModelData modelData = loadModelData(filename, &ok);
    if (ok) {
        m_modelData.append(modelData);
    }

    endInsertRows();

    if (!ok) {
        beginResetModel();

        QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.coderus.searchengines"),
                                                          QStringLiteral("/org/coderus/searchengines"),
                                                          QStringLiteral("org.coderus.searchengines"),
                                                          QStringLiteral("removeSearchEngine"));
        msg.setArguments({filename});
        qDebug() << Q_FUNC_INFO << filename <<
        QDBusConnection::systemBus().call(msg);

        endResetModel();
    }
}

void SearchEnginesModel::remove(const QString &filename)
{
    int index = -1;
    for (int i = 0; i < m_modelData.count(); i++) {
        if (m_modelData[i].filename == filename) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);

    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.coderus.searchengines"),
                                                      QStringLiteral("/org/coderus/searchengines"),
                                                      QStringLiteral("org.coderus.searchengines"),
                                                      QStringLiteral("removeSearchEngine"));
    msg.setArguments({filename});
    qDebug() << Q_FUNC_INFO << filename <<
    QDBusConnection::systemBus().call(msg);

    m_modelData.removeAt(index);

    endRemoveRows();
}

SearchEnginesModel::ModelData SearchEnginesModel::loadModelData(const QString &filename, bool *ok) const
{
    qDebug() << Q_FUNC_INFO << filename;

    ModelData data;
    QFile searchEngine(filename);
    if (!searchEngine.open(QFile::ReadOnly)) {
        if (ok) {
            *ok = false;
        }
        return data;
    }
    QXmlQuery query;
    const QByteArray xmlData = searchEngine.readAll();
    query.setFocus(QString::fromUtf8(xmlData));
    if (xmlData.contains(QByteArrayLiteral("http://a9.com/-/spec/opensearch/1.1/"))) {
        query.setQuery(QStringLiteral("declare default element namespace \"http://a9.com/-/spec/opensearch/1.1/\"; //ShortName/text()"));
    } else {
        query.setQuery(QStringLiteral("//ShortName/text()"));
    }
    if (!query.isValid()) {
        if (ok) {
            *ok = false;
        }
        return data;
    }
    QString queryResult;
    query.evaluateTo(&queryResult);
    data.title = queryResult.trimmed();
    data.filename = searchEngine.fileName();

    if (ok) {
        *ok = true;
    }
    return data;
}
