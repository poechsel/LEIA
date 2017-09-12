#ifndef CODEVIEW_H
#define CODEVIEW_H

#include <QObject>
#include <QWidget>
#include <QAbstractListModel>

class CodeModel: public QAbstractListModel
{
    Q_OBJECT
public:
    CodeModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // CODEVIEW_H
