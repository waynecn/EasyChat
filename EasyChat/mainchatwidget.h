#ifndef MAINCHATWIDGET_H
#define MAINCHATWIDGET_H

#include "messagewidget.h"
#include "inputmessagewidget.h"
#include "userlistwidget.h"

#include <QWidget>
#include <QPushButton>
#include <QKeyEvent>

namespace Ui {
class MainChatWidget;
}

class MainChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainChatWidget(QWidget *parent = nullptr);
    ~MainChatWidget();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void EnableSendMessageButton(bool b);
    void QueryFileList();

public slots:
    void OnDownloadItem(QTableWidgetItem *item);

private slots:
    void on_sendFilePushButton_clicked();
    void OnRequestFinished(NetworkParams &params);

    void on_fileListPushButton_clicked();
    void onUploadFileFailed(NetworkParams &params, QString &msg);
    void onDownloadFileFailed(NetworkParams &params, QString &msg);

private:
    Ui::MainChatWidget *ui;
    MessageWidget *m_pMessageWidget;
    InputMessageWidget *m_pInputMessageWidget;
    UserListWidget *m_pUserListWidget;
    bool m_bCtrlPressed;

signals:
    void fileListRequestFinished(QJsonArray &array);
    void uploadingFile(NetworkParams &params);
    void updateRequestProcess(NetworkParams &params);
    void downloadingFile(NetworkParams &params);
};

#endif // MAINCHATWIDGET_H
