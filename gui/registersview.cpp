#include "registersview.h"

RegistersView::RegistersView(QWidget *parent):
    DataView(16, "r%1", parent)
{

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showContextMenu(const QPoint &)));
}
void RegistersView::showContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);
   QAction action1("Accèder à cette zone mémoire", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(switchToMemorySelected()));
   _selected_row = this->itemAt(pos)->row();
   contextMenu.addAction(&action1);
   contextMenu.exec(mapToGlobal(pos));
}

void RegistersView::switchToMemorySelected() {
   emit (sendSelected(this->_data[_selected_row]));
}
