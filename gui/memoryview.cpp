#include "memoryview.h"

MemoryView::MemoryView(QWidget *parent):
    DataView(0x10000, "%1", parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
        this, SLOT(showContextMenu(const QPoint &)));

    for (int i = MEM_SCREEN_BEGIN; i < 0x10000; ++i) {
        this->item(i, 0)->setBackgroundColor(QColor(219, 232, 255));
        this->item(i, 1)->setBackgroundColor(QColor(219, 232, 255));
        this->item(i, 2)->setBackgroundColor(QColor(219, 232, 255));
    }
}

void MemoryView::showContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);
   QAction action1("Voir l'instruction désassemblée", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(requestSwitchToCode()));
   _selected_row = this->itemAt(pos)->row();
   contextMenu.addAction(&action1);
   contextMenu.exec(mapToGlobal(pos));
}

void MemoryView::requestSwitchToCode() {
   emit (switchToCode(_selected_row));
}
