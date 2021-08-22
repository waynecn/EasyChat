#include "uploadfilewidget.h"

#include <QHeaderView>

UploadFileWidget::UploadFileWidget(QWidget *parent):
    QTableWidget (parent)
{
    setColumnCount(5);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList headerLables;
    headerLables.push_back(" 文件名称 ");
    headerLables.push_back(" 上传时间 ");
    headerLables.push_back(" 剩余时间 ");
    headerLables.push_back(" 速度 ");
    headerLables.push_back(" 进度 ");
    setHorizontalHeaderLabels(headerLables);
}

UploadFileWidget::~UploadFileWidget() {

}
