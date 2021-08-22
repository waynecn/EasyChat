#include "downloadfilewidget.h"

#include <QHeaderView>

DownloadFileWidget::DownloadFileWidget(QWidget *parent):
    QTableWidget (parent)
{
    setColumnCount(5);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList headerLables2;
    headerLables2.push_back(" 文件名称 ");
    headerLables2.push_back(" 下载时间 ");
    headerLables2.push_back(" 剩余时间 ");
    headerLables2.push_back(" 速度 ");
    headerLables2.push_back(" 进度 ");
    setHorizontalHeaderLabels(headerLables2);

}

DownloadFileWidget::~DownloadFileWidget() {

}
