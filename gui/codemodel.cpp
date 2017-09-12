#include "codemodel.h"

CodeModel::CodeModel(QObject *parent):
    QAbstractListModel(parent)
{
}


QVariant CodeModel::data(const QModelIndex &index, int role) const {
    int row = index.row();

    if (role == Qt::DisplayRole) {
        return QString ("aa");
    } else if (role == Qt::CheckStateRole) {
        return Qt::Checked;
    }

    return QVariant();
}


int CodeModel::rowCount(const QModelIndex &parent) const {

    return 5;
}
