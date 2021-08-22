#include "mytablewidget.h"

#include <QDebug>
#include <QProcess>

MyTableWidget::MyTableWidget(QWidget *parent):
    QTableWidget (parent),
    m_pMenu(nullptr)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    m_pMenu = new QMenu();
    QAction *action = m_pMenu->addAction("打开目录");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(onOpenFileDir()));
}

MyTableWidget::~MyTableWidget() {
    if (nullptr != m_pMenu) {
        delete m_pMenu;
        m_pMenu = nullptr;
    }
}

void MyTableWidget::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void MyTableWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton && itemAt(event->pos()) != nullptr) {
        m_pMenu->exec(event->globalPos());
    }
    event->ignore();
}

void MyTableWidget::onOpenFileDir() {
    QString file = currentItem()->toolTip();
    file = file.replace('/', '\\');
    QString cmd = "explorer /e,/select," + file;
    QProcess proc;
    proc.execute(cmd);
    proc.close();
}
