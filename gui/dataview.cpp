#include "dataview.h"

DataView::DataView(int size, QString label_formatting, QWidget *parent):
    QTableWidget(parent), _data(0), _size(size)
{
    this->setRowCount(_size);
    this->setColumnCount(3);

    QAbstractItemModel* model = this->model();
            QStringList labels;
    for (int i = 0; i < this->rowCount(); ++i) {
        QVariant data = model->headerData(i, Qt::Vertical);
        labels << QString(label_formatting).arg(data.toInt() - 1);
        for (int j = 0; j < this->columnCount(); ++j) {
            this->setItem(i, j, new QTableWidgetItem(""));
        }
    }
    /* Update vertical header labels */
    this->setVerticalHeaderLabels(labels);
    this->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Dec")));
    this->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Hex")));
    this->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("Char")));

    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    //this->setShowGrid(false);
}

void DataView::setData(uword *data) {
    this->_data = data;
}

void DataView::_updateRow(int row, int value) {
    if (0 <= row && row < this->rowCount()) {
        this->item(row, 0)->setText(QString::number(value));
        this->item(row, 1)->setText("0x" + QString::number(value, 16).rightJustified(4, '0'));
        this->item(row, 2)->setText(QString((0<value && value < 128) ? (char)value : ' '));
    }
}

void DataView::updateOptimize(QVector<int> indices) {
    if (this->_data) {
        for (int t : indices) {
            _updateRow(t, this->_data[t]);
        }
    }
}

void DataView::focusOn(int row) {
    this->setCurrentCell(row, 0);
}

void DataView::update() {
    for (int i = 0; i < this->rowCount(); ++i) {
        this->_updateRow(i, _data[i]);
    }
    this->resizeColumnsToContents();
}

void DataView::editCell(int row, int column) {
    bool conversion = true;
    int v = 0;
    QString value = this->item(row, column)->text();
    if (column == 0) {
        v = value.toInt(&conversion);
    } else if (column == 1) {
        v = value.toInt(&conversion, 16);
    } else {
        if (value.size() == 1)
            v = (int)value.at(0).toLatin1();
    }
    if (conversion) {
        _data[row] = v;
        emit memoryChanged(row);
    } else {
        v = _data[row];
    }
    this->_updateRow(row, v);
}
