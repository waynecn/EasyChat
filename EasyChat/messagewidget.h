#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H
#include "common.h"

#include <QWidget>
#include <QMap>
#include <QJsonObject>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVector>
#include <QTableWidgetItem>
#include <QKeyEvent>

namespace Ui {
class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = nullptr);
    ~MessageWidget();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void SendMessage(MessageStruct &msg);

    void uploadClient();

public slots:
    void OnSendMessage(MessageStruct &msg);
    void OnWebSocketMsgReceived(const QString &msg);
    void OnTabCloseRequested(int index);
    void OnItemDoubleClicked(QTableWidgetItem *item);

private slots:
    void onCurrentChanged(int index);
    void onUploadClientSuccess();
    void onUploadClientFailed(NetworkParams &params, QString &msg);
    void onAnchorClicked(const QUrl &url);

private:
    Ui::MessageWidget *ui;
    QString m_strContentTemplateWithoutLink;
    QString m_strContentTemplateWithLink;
    QString m_strContentTemplateWithLinkWithImage;
    QString m_strContentTemplateWithoutLinkWithImage;
    QMap<QString, QString>  m_messageMap;
    QVector<MessageStruct>    m_vecMsgInfos;
    QMap<QString, UserInfo>   m_onlineUserMap;
    QTextBrowser *m_pTextBrowser;
    QVector<QString> m_vecTabWidgetUserIDs;
    bool m_bCtrlPressed;
    QFile *m_pFile;

signals:
    void downloadImage(QString strUrl, QString saveDir);
    void newMessageArrived();
    void onlineUsers(QMap<QString, UserInfo> &onlineUserMap);
    void onRequestFinished(NetworkParams &params);
    void onDownloadFileFailed(NetworkParams &params, QString &msg);
    void downloadingFile(NetworkParams &params);
    void updateRequestProcess(NetworkParams &params);
    void uploadingClient(NetworkParams &params);
};

#endif // MESSAGEWIDGET_H
