#include "memoryview.h"

MemoryView::MemoryView(QWidget *parent):
    QTableWidget(parent)
{
    this->setRowCount(0x10000);
    this->setColumnCount(3);

    QAbstractItemModel* model = this->model();
            QStringList labels;
    for (int i = 0; i < this->rowCount(); ++i) {
        QVariant data = model->headerData(i, Qt::Vertical);
        labels << QString("%1").arg(data.toInt() - 1);
        for (int j = 0; j < this->columnCount(); ++j) {
            this->setItem(i, j, new QTableWidgetItem(""));
        }
    }
    /* Update vertical header labels */
    this->setVerticalHeaderLabels(labels);
    this->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Dec")));
    this->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Hex")));
    this->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("Char")));
}

void MemoryView::setMachine(Machine &machine) {
    this->_machine = &machine;
}

void MemoryView::_updateRow(int row, int value) {
    if (row < this->rowCount()) {
        this->item(row, 0)->setText(QString::number(value));
        this->item(row, 1)->setText("0x" + QString::number(value, 16).rightJustified(4, '0'));
        this->item(row, 2)->setText(QString((0<value && value < 128) ? (char)value : ' '));
    }
    /*
    this->setItem(row, 1, new QTableWidgetItem("0x" + QString::number(value, 16)));
    this->setItem(row, 2, new QTableWidgetItem(QString((0 < value && value < 128) ? (char)value : ' ')));
    */
}

void MemoryView::update() {
    for (int i = 0; i < 0x10000; ++i) {
        this->_updateRow(i, _machine->memory[i]);
    }
    this->resizeColumnsToContents();
}

void MemoryView::editCell(int row, int column) {
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
        _machine->memory[row] = v;
        emit memoryChanged(row);
    } else {
        v = _machine->memory[row];
    }
    this->_updateRow(row, v);
}
