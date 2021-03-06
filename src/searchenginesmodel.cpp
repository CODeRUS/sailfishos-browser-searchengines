#include "searchenginesmodel.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QXmlStreamReader>

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
        const ModelData data = loadModelData(filename, &ok);
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
    const QString filename = name + s_extension;

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
    QFile searchEngine(s_directory + filename);
    if (!searchEngine.open(QFile::ReadOnly)) {
        if (ok) {
            *ok = false;
        }
        return data;
    }
    QString queryResult;
    QXmlStreamReader reader(&searchEngine);
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement && reader.name() == QStringLiteral("ShortName")) {
            reader.readNext();
            queryResult = reader.text().toString();
            break;
        }
    }
    data.title = queryResult;
    data.filename = filename;

    if (ok) {
        *ok = true;
    }
    return data;
}
