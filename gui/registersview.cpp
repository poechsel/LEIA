#include "registersview.h"

RegistersView::RegistersView(QWidget *parent):
    QTableWidget(parent)
{
    this->setRowCount(16);
    this->setColumnCount(3);

    QAbstractItemModel* model = this->model();
            QStringList labels;
    for (int i = 0; i < 16; ++i) {
        QVariant data = model->headerData(i, Qt::Vertical);
        labels << QString("r%1").arg(data.toInt() - 1);
        for (int j = 0; j < this->columnCount(); ++j) {
            this->setItem(i, j, new QTableWidgetItem(""));
        }
    }
    labels << "PC";
    for (int j = 0; j < this->columnCount(); ++j) {
        this->setItem(17, j, new QTableWidgetItem(""));
    }

    /* Update vertical header labels */
    this->setVerticalHeaderLabels(labels);
    this->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Dec")));
    this->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Hex")));
    this->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("Char")));


    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showContextMenu(const QPoint &)));
}
void RegistersView::showContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);

   QAction action1("Accèder à cette zone mémoire", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
   contextMenu.addAction(&action1);

   contextMenu.exec(mapToGlobal(pos));
}

void RegistersView::update() {
    if (!this->_machine)
        return;
    for (int i = 0; i < 16; ++i)
        this->_updateRow(i, this->_machine->registers[i]);
}

void RegistersView::setMachine(Machine &machine) {
    this->_machine = &machine;
}

void RegistersView::_updateRow(int row, int value) {
    if (0 <= row && row < this->rowCount()) {
        this->item(row, 0)->setText(QString::number(value));
        this->item(row, 1)->setText("0x" + QString::number(value, 16).rightJustified(4, '0'));
        this->item(row, 2)->setText(QString((0<value && value < 128) ? (char)value : ' '));
    }
}

void RegistersView::editCell(int row, int column) {
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
    if (row < 16) {
    if (conversion) {
        _machine->registers[row] = v;
    } else {
        v = _machine->registers[row];
    }
    }
    this->_updateRow(row, v);
}
