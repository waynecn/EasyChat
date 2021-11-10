#ifndef FILELISTTABLEWIDGET_H
#define FILELISTTABLEWIDGET_H

#include "common.h"

#include <QTableWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QKeyEvent>

namespace Ui {
class FileListTableWidget;
}

class FileListTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit FileListTableWidget(QWidget *parent = nullptr);
    ~FileListTableWidget();
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void OnFileList(QJsonArray &files);
    void onRefreshFileList();

private slots:
    void onItemClicked(QTableWidgetItem *item);
    void onCopyLink();
    void download();
    void deleteFile();
    void onDeleteFileSuccess();
    void onDeleteFileFailed(NetworkParams &params, QString &msg);

private:
    Ui::FileListTableWidget *ui;
    QMenu *m_pMenu;
    QPoint m_point;
    bool m_bCtrlPressed;

signals:
    void downloadItem(QTableWidgetItem *item);
};

#endif // FILELISTTABLEWIDGET_H
