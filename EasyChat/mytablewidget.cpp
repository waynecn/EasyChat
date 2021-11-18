#include "mytablewidget.h"

#include <QDebug>
#include <QProcess>

MyTableWidget::MyTableWidget(QWidget *parent):
    QTableWidget (parent),
    m_pMenu(nullptr)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    m_pMenu = new QMenu();
    QAction *action = m_pMenu->addAction("打开目录");
    m_pMenu->addSeparator();
    QAction *delAction = m_pMenu->addAction("删除");
    connect(action, SIGNAL(triggered(bool)), this, SLOT(onOpenFileDir()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(onDeleteFile()));
}

MyTableWidget::~MyTableWidget() {
    if (nullptr != m_pMenu) {
        delete m_pMenu;
        m_pMenu = nullptr;
    }
}

void MyTableWidget::mousePressEvent(QMouseEvent *event) {
    setCurrentItem(itemAt(event->pos()));
    QTableWidget::mousePressEvent(event);
}

void MyTableWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton && itemAt(event->pos()) != nullptr) {
        setCurrentItem(itemAt(event->pos()));
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

void MyTableWidget::onDeleteFile() {
    QString file = currentItem()->toolTip();
#if defined(Q_OS_LINUX)
    QString cmd = "rm -f " + file;
    QProcess p;
    p.execute(cmd);
    p.close();
#else
    file = file.replace("/", "\\");
    QString cmd = "del \"" + file + "\"";
    //system(cmd.toUtf8());
    system(cmd.toLocal8Bit());
#endif
}
