#ifndef SEARCHENGINESMODEL_H
#define SEARCHENGINESMODEL_H

#include <QAbstractListModel>

class SearchEnginesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SearchEnginesModel(QObject *parent = nullptr);

    struct ModelData {
        QString title;
        QString filename;
    };

    enum SearchEngineRoles {
        TitleRole = Qt::DisplayRole,
        FilenameRole = Qt::UserRole + 1,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void add(const QString &title, const QString &data);
    void remove(const QString &filename);

private:
    ModelData loadModelData(const QString &filename, bool *ok = nullptr) const;

    QVector<ModelData> m_modelData;

};

#endif // SEARCHENGINESMODEL_H
