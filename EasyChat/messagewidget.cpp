#include "messagewidget.h"
#include "ui_messagewidget.h"
#include "tools.h"
#include "settingdlg.h"
#include "mynetworkcontroller.h"
#include "clientthread.h"

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QTabBar>
#include <QJsonArray>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

MessageWidget::MessageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageWidget),
    m_pTextBrowser(nullptr),
    m_pFile(nullptr)
{
    ui->setupUi(this);

    m_pTextBrowser = new QTextBrowser();
    m_pTextBrowser->setOpenExternalLinks(false);
    m_pTextBrowser->setOpenLinks(false);
    m_PersonalTextBrowser = new QTextBrowser();
    m_PersonalTextBrowser->setOpenExternalLinks(false);
    m_PersonalTextBrowser->setOpenLinks(false);
    int tabCount = ui->tabWidget->count();
    for (int i = 0; i < tabCount; ++i) {
        ui->tabWidget->removeTab(0);
    }
    ui->tabWidget->setStyleSheet(TAB_BAR_STYLE_SHEET);
    ui->tabWidget->addTab(m_pTextBrowser, "首页");
    ui->tabWidget->addTab(m_PersonalTextBrowser, "个人空间");
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);
    ui->tabWidget->tabBar()->setTabButton(1, QTabBar::RightSide, nullptr);

    m_strContentTemplateWithoutLink = "<div><a style=\"color:blue\">%1:</a><a style=\"color:gray\">(%2)</a>\
        <br />&nbsp;&nbsp;&nbsp;&nbsp;<a>%3</a></div>";
    m_strContentTemplateWithLink = "<div><a style=\"color:blue\">%1:</a><a style=\"color:gray\">(%2)</a><br />&nbsp;&nbsp;&nbsp;&nbsp;<a>%3</a> \
        &nbsp;&nbsp;&nbsp;&nbsp;<a>上传文件:</a><a href=\"%4\">%5</a></div>";
    m_strContentTemplateWithLinkWithImage = "<div><a style=\"color:blue\">%1:</a><a style=\"color:gray\">(%2)</a><br />&nbsp;&nbsp;&nbsp;&nbsp;\
        <a>%3</a><br /><div><img src=\"%4\" /></div><br />&nbsp;&nbsp;&nbsp;&nbsp;<a>上传文件:</a><a href=\"%5\">%6</a></div>";
    m_strContentTemplateWithoutLinkWithImage = "<div><a style=\"color:blue\">%1:</a><a style=\"color:gray\">(%2)</a><br />&nbsp;&nbsp;&nbsp;&nbsp;\
        <a>%3</a><br /><div><img src=\"%4\" /></div><br /></div>";

    QDir dir(APPLICATION_IMAGE_DIR);
    if (!dir.exists()) {
        dir.mkpath(APPLICATION_IMAGE_DIR);
    }

    connect(&g_WebSocket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(OnWebSocketMsgReceived(const QString &)));
    connect(&g_WebSocket, SIGNAL(pong(quint64, const QByteArray &)), this, SLOT(onPongMessage(quint64, const QByteArray &)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(OnTabCloseRequested(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
    connect(m_pTextBrowser, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(onAnchorClicked(const QUrl &)));
    connect(m_PersonalTextBrowser, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(onAnchorClicked(const QUrl &)));

    m_Timer = new QTimer(this);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimerOut()));
    m_Timer->start(10 * 1000);    //10 seconds
    m_PingOvertimeCount = 0;
}

MessageWidget::~MessageWidget()
{
    if (m_pTextBrowser != nullptr) {
        delete m_pTextBrowser;
        m_pTextBrowser = nullptr;
    }
    if (m_PersonalTextBrowser != nullptr) {
        delete m_PersonalTextBrowser;
        m_PersonalTextBrowser = nullptr;
    }
    delete m_Timer;
    m_Timer = nullptr;
    delete ui;
}

void MessageWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    if (m_bCtrlPressed && e->key() == Qt::Key_U) {
        uploadClient();
    }

    if (m_bCtrlPressed && e->key() == Qt::Key_T) {
        uploadFileByTCP();
    }

    if (m_bCtrlPressed && e->key() == Qt::Key_P) {
        uploadFileToPersonalSpace();
    }

    e->accept();
}

void MessageWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}

void MessageWidget::SendMessage2(MessageStruct &msg) {
    OnSendMessage(msg);
}

void MessageWidget::OnSendMessage(MessageStruct &msg) {
    QString textMsg = msg.msg;
    textMsg = textMsg.replace("\n", "<br />&nbsp;&nbsp;&nbsp;&nbsp;");
    int currentIndex = ui->tabWidget->currentIndex();
    if (currentIndex == 0) {
        if (msg.fileUrl.isEmpty()) {
            m_messageMap["main"] += m_strContentTemplateWithoutLink.arg(msg.userName).arg(msg.sendTime).arg(textMsg);
        } else {
            m_messageMap["main"] += m_strContentTemplateWithLink.arg(msg.userName).arg(msg.sendTime).arg(textMsg).arg(msg.fileUrl).arg(msg.fileName);
        }
        m_pTextBrowser->setHtml("<html>" + m_messageMap["main"] + "</html>");
        m_pTextBrowser->moveCursor(QTextCursor::End);
    } else if (currentIndex == 1) {
        //上传到个人空间
        if (msg.fileUrl.isEmpty()) {
            m_messageMap["personal"] += m_strContentTemplateWithoutLink.arg(msg.userName).arg(msg.sendTime).arg(textMsg);
        } else {
            m_messageMap["personal"] += m_strContentTemplateWithLink.arg(msg.userName).arg(msg.sendTime).arg(textMsg).arg(msg.fileUrl).arg(msg.fileName);
        }
        m_PersonalTextBrowser->setHtml("<html>" + m_messageMap["personal"] + "</html>");
        m_PersonalTextBrowser->moveCursor(QTextCursor::End);
    } else {
        QTextBrowser *browser = (QTextBrowser *)ui->tabWidget->currentWidget();
        QString currentUserID = m_vecTabWidgetUserIDs[currentIndex - 2];
        msg.toUserID = currentUserID;
        if (msg.fileUrl.isEmpty()) {
            m_messageMap[currentUserID] += m_strContentTemplateWithoutLink.arg(msg.userName).arg(msg.sendTime).arg(textMsg);
        } else {
            m_messageMap[currentUserID] += m_strContentTemplateWithLink.arg(msg.userName).arg(msg.sendTime).arg(textMsg).arg(msg.fileUrl).arg(msg.fileName);
        }
        browser->setHtml("<html>" + m_messageMap[currentUserID] + "</html>");
        browser->moveCursor(QTextCursor::End);
        connect(browser, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(onAnchorClicked(const QUrl &)));
    }
    if (1 != currentIndex) {
        QByteArray byteMsg = tools::GetInstance()->GenerateWebsocketMsg(msg);
        qint64 ret = g_WebSocket.sendTextMessage(byteMsg);
        if (ret != byteMsg.size()) {
            qDebug() << "byte msg size:" << byteMsg.size() << " send msg size:" << ret;
            qDebug() << "websocket message send failed, message is:" << byteMsg;
        }
    }
}

void MessageWidget::OnWebSocketMsgReceived(const QString &msg) {
    qDebug() << "receive msg:" << msg;
    if (msg.isEmpty()) {
        return;
    }
    QJsonParseError jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toUtf8(), &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError) {
        qDebug() << "解析ws数据失败";
        return;
    }

    if (jsonDoc.isObject()) {
        if (jsonDoc["message"].isObject()) {
            //处理消息信息
            QJsonObject jsonMsg = jsonDoc["message"].toObject();
            QString strUserId = jsonMsg["userid"].toString();
            if (strUserId != g_userID) {
                MessageStruct msgInfo;
                msgInfo.userName = jsonMsg["username"].toString();
                msgInfo.userID = jsonMsg["userid"].toString();
                msgInfo.msg = jsonMsg["message"].toString();
                msgInfo.sendTime = jsonMsg["time"].toString();
                msgInfo.fileUrl = jsonMsg["filelink"].toString();
                msgInfo.imgUrl = jsonMsg["image"].toString();

                if (!msgInfo.imgUrl.isEmpty()) {
                    emit downloadImage(msgInfo.imgUrl, APPLICATION_IMAGE_DIR);
                }

                m_vecMsgInfos.push_back(msgInfo);

                if (msgInfo.fileUrl.isEmpty()) {
                    if (msgInfo.imgUrl.isEmpty()) {
                        m_messageMap["main"] += m_strContentTemplateWithoutLink.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg);
                    } else {
                        QString fileName = msgInfo.imgUrl.mid(msgInfo.imgUrl.lastIndexOf('/') + 1);
                        m_messageMap["main"] += m_strContentTemplateWithoutLinkWithImage.arg(msgInfo.userName).arg(msgInfo.msg).arg(msgInfo.sendTime).arg(APPLICATION_IMAGE_DIR + fileName);
                    }
                } else {
                    QString fileName = msgInfo.fileUrl.mid(msgInfo.fileUrl.lastIndexOf('/') + 1);
                    if (msgInfo.imgUrl.isEmpty()) {
                        m_messageMap["main"] += m_strContentTemplateWithLink.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(msgInfo.fileUrl).arg(fileName);
                    } else {
                        m_messageMap["main"] += m_strContentTemplateWithLinkWithImage.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(APPLICATION_IMAGE_DIR + fileName).arg(msgInfo.fileUrl).arg(fileName);
                    }
                }
                m_pTextBrowser->setHtml("<html>" + m_messageMap["main"] + "</html>");
                m_pTextBrowser->moveCursor(QTextCursor::End);
                emit newMessageArrived();
                ui->tabWidget->setCurrentIndex(0);
                QApplication::alert(this);
            }
        }
        else if (jsonDoc["online"].isObject()) {
            //处理在线信息
            QJsonObject jsonOnline = jsonDoc["online"].toObject();
            QString strUserId = jsonOnline["userid"].toString();
            if (strUserId.compare(g_userID, Qt::CaseInsensitive) != 0) {
                //将在线信息更新到右侧
                if (!m_onlineUserMap.contains(strUserId)) {
                    UserInfo userInfo;
                    userInfo.userID = strUserId;
                    userInfo.userName = jsonOnline["username"].toString();
                    userInfo.loginTime = tools::GetInstance()->GetCurrentTime2();
                    m_onlineUserMap[strUserId] = userInfo;
                }
            }

            //将在线用户信息更新到右侧
            emit onlineUsers(m_onlineUserMap);
        } else if (jsonDoc[g_userID].isObject()) {
            //私发消息 单独显示
            QJsonObject jsonMsg = jsonDoc[g_userID].toObject();
            QString strUserId = jsonMsg["userid"].toString();
            if (strUserId != g_userID) {
                MessageStruct msgInfo;
                msgInfo.userName = jsonMsg["username"].toString();
                msgInfo.userID = jsonMsg["userid"].toString();
                msgInfo.msg = jsonMsg["message"].toString();
                msgInfo.sendTime = jsonMsg["time"].toString();
                msgInfo.fileUrl = jsonMsg["filelink"].toString();
                msgInfo.imgUrl = jsonMsg["image"].toString();

                if (!msgInfo.imgUrl.isEmpty()) {
                    emit downloadImage(msgInfo.imgUrl, APPLICATION_IMAGE_DIR);
                }

                //查找是否有窗口存在
                bool bExist = false;
                int nIndex = 0;
                for (int index = 0; index < ui->tabWidget->count(); ++index) {
                    QString tabName = ui->tabWidget->tabText(index);
                    if (tabName == msgInfo.userName) {
                        bExist = true;
                        nIndex = index;
                        break;
                    }
                }
                if (!bExist) {
                    QTextBrowser *pEdit = new QTextBrowser();
                    pEdit->setReadOnly(true);
                    pEdit->setOpenLinks(false);
                    pEdit->setOpenExternalLinks(false);
                    if (msgInfo.fileUrl.isEmpty()) {
                        if (msgInfo.imgUrl.isEmpty()) {
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithoutLink.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg);
                        } else {
                            QString fileName = msgInfo.imgUrl.mid(msgInfo.imgUrl.lastIndexOf('/') + 1);
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithoutLinkWithImage.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(APPLICATION_IMAGE_DIR + fileName);
                        }
                    } else {
                        QString fileName = msgInfo.fileUrl.mid(msgInfo.fileUrl.lastIndexOf('/') + 1);
                        if (msgInfo.imgUrl.isEmpty()) {
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithLink.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(msgInfo.fileUrl).arg(fileName);
                        } else {
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithLinkWithImage.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(APPLICATION_IMAGE_DIR + fileName).arg(msgInfo.fileUrl).arg(fileName);
                        }
                    }
                    pEdit->setHtml("<html>" + m_messageMap[msgInfo.userID] + "</html>");
                    pEdit->moveCursor(QTextCursor::End);
                    m_vecTabWidgetUserIDs.push_back(msgInfo.userID);
                    ui->tabWidget->addTab(pEdit, msgInfo.userName);
                    ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);
                    ui->tabWidget->setCurrentWidget(pEdit);
                    connect(pEdit, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(onAnchorClicked(const QUrl &)));
                } else {
                    ui->tabWidget->setCurrentIndex(nIndex);
                    QTextBrowser *pEdit = (QTextBrowser *)ui->tabWidget->widget(nIndex);
                    if (msgInfo.fileUrl.isEmpty()) {
                        if (msgInfo.imgUrl.isEmpty()) {
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithoutLink.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg);
                        } else {
                            QString fileName = msgInfo.imgUrl.mid(msgInfo.imgUrl.lastIndexOf('/') + 1);
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithoutLinkWithImage.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(APPLICATION_IMAGE_DIR + fileName);
                        }
                    } else {
                        QString fileName = msgInfo.fileUrl.mid(msgInfo.fileUrl.lastIndexOf('/') + 1);
                        if (msgInfo.imgUrl.isEmpty()) {
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithLink.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(msgInfo.fileUrl).arg(fileName);
                        } else {
                            m_messageMap[msgInfo.userID] += m_strContentTemplateWithLinkWithImage.arg(msgInfo.userName).arg(msgInfo.sendTime).arg(msgInfo.msg).arg(APPLICATION_IMAGE_DIR + fileName).arg(msgInfo.fileUrl).arg(fileName);
                        }
                    }
                    pEdit->setHtml("<html>" + m_messageMap[msgInfo.userID] + "</html>");
                    pEdit->moveCursor(QTextCursor::End);
                    connect(pEdit, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(onAnchorClicked(const QUrl &)));
                }
                emit newMessageArrived();
                QApplication::alert(this);
            }
        }
    } else if (jsonDoc.isArray()) {
        m_onlineUserMap.clear();
        QJsonArray array = jsonDoc.array();
        for (int i = 0; i < array.size(); ++i) {
            QJsonObject jsonObj = array[i].toObject();
            if (jsonObj["Online"].isObject()) {
                //处理在线信息
                QJsonObject jsonOnline = jsonObj["Online"].toObject();
                QString strUserId = jsonOnline["Userid"].toString();
                if (strUserId != g_userID) {
                    //将在线信息更新到右侧
                    UserInfo userinfo;
                    userinfo.userID = strUserId;
                    userinfo.userName = jsonOnline["Username"].toString();
                    if (!m_onlineUserMap.contains(strUserId)) {
                        m_onlineUserMap[strUserId] = userinfo;
                    }
                }

                //将m_vecOnlineUsers中的内容更新到右侧
                emit onlineUsers(m_onlineUserMap);
            }
        }
    }
}

void MessageWidget::OnTabCloseRequested(int index) {
    QString userId = m_vecTabWidgetUserIDs[index - 2];
    m_messageMap[userId].clear();
    m_vecTabWidgetUserIDs.removeAt(index - 2);
    ui->tabWidget->removeTab(index);
}

void MessageWidget::OnItemDoubleClicked(QTableWidgetItem *item) {
    if (item->row() == 0) {
        return;
    }
    QString userID = item->toolTip();
    if (userID.isEmpty()) {
        return;
    }

    QString userName = m_onlineUserMap[userID].userName;
    if (userName.isEmpty()) {
        return;
    }

    //在现有标签中能找到该用户的对话框 则置为前页
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        QString tabName = ui->tabWidget->tabText(i);
        if (tabName.compare(userName) == 0) {
            ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }

    m_vecTabWidgetUserIDs.push_back(userID);
    g_toUserID = userID;
    g_toUserName = userName;
    //未找到 则新建标签
    QTextBrowser *browser = new QTextBrowser();
    browser->setReadOnly(true);
    browser->setOpenLinks(false);
    browser->setOpenExternalLinks(false);

    ui->tabWidget->addTab(browser, userName);
    ui->tabWidget->setCurrentWidget(browser);
}

void MessageWidget::onCurrentChanged(int index) {
    if (index < 2) {
        g_toUserID = "";
        return;
    }
    g_toUserID = m_vecTabWidgetUserIDs[index - 2];
    if (!g_toUserID.isEmpty()) {
        g_toUserName = m_onlineUserMap[g_toUserID].userName;
    }
}

void MessageWidget::onUploadClientSuccess() {
}

void MessageWidget::onUploadClientFailed(NetworkParams &params, QString &msg) {
    QMessageBox::warning(nullptr, "Warn", "上传客户端:" + params.fileName + " 失败:" + msg);
}

void MessageWidget::onAnchorClicked(const QUrl &url) {
    if (url.isEmpty()) {
        return;
    }
    QString strUrl = url.toString();
    qDebug() << "clicked:" << strUrl;
    int index = strUrl.lastIndexOf("/");
    QString originalFileName = strUrl.mid(index + 1);

    QFileDialog fDlg;
    fDlg.setAcceptMode(QFileDialog::AcceptSave);
    fDlg.setFileMode(QFileDialog::AnyFile);
    QSettings settings;
    QString saveFileDir = settings.value(SETTING_SAVE_FILE_DIR, "C:/").toString();
    QString saveFilePath = fDlg.getSaveFileName(this, "Save File", saveFileDir + originalFileName);
    qDebug() << "fileName:" << saveFilePath;
    if (saveFilePath.isEmpty()) {
        return;
    }

    saveFilePath = saveFilePath.replace("\\", "/");
    QString saveFileName = saveFilePath.mid(saveFilePath.lastIndexOf("/") + 1);
    qDebug() << "download file:" << originalFileName << " save file:" << saveFileName;
    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.requestTime = tools::GetInstance()->GetCurrentTime2();
    params.httpRequestType = REQUEST_DOWNLOAD_FILE;
    params.fileName = originalFileName;
    params.saveFileName = saveFileName;
    params.userID = g_userID;
    params.userName = g_userName;
    params.saveFileDir = saveFilePath.mid(0, saveFilePath.lastIndexOf("/") + 1);

    saveFileDir = params.saveFileDir;
    settings.setValue(SETTING_SAVE_FILE_DIR, saveFileDir);

    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(requestFinished(NetworkParams &)), this, SIGNAL(onRequestFinished(NetworkParams &)));
    connect(controller, SIGNAL(updateRequestProcess(NetworkParams &)), this, SIGNAL(updateRequestProcess(NetworkParams &)));
    connect(controller, SIGNAL(downloadFileFailed(NetworkParams &, QString &)), this, SIGNAL(onDownloadFileFailed(NetworkParams &, QString &)));
    controller->StartWork();
    //emit upload file actions for file widget to show the uploading progress
    emit downloadingFile(params);
}

void MessageWidget::uploadClient() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    QString filePath;
    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (files.size() > 0) {
            filePath = files[0];
        }
    }

    if (filePath.isEmpty()) {
        return;
    }

    filePath = filePath.replace("\\", "/");
    QString fileName = filePath.mid(filePath.lastIndexOf('/') + 1);

    int versionStartIndex = filePath.lastIndexOf('-');
    if (versionStartIndex == -1) {
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText("文件名不符合要求");
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    int versionEndIndex = filePath.lastIndexOf('.');
    if (versionEndIndex == -1 || versionEndIndex < versionStartIndex) {
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText("文件名不符合要求");
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    QString versionStr = filePath.mid(versionStartIndex + 1, versionEndIndex - versionStartIndex - 1);
    qDebug() << "VersionStr:" << versionStr;
    if (versionStr.length() < 1) {
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText("文件名未包含版本号");
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    NetworkParams params;
    params.httpRequestType = REQUEST_UPLOAD_CLIENT;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.requestTime = tools::GetInstance()->GetCurrentTime2();
    params.userID = g_userID;
    params.userName = g_userName;
    params.fileName = fileName;
    params.filePath = filePath;
    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(uploadClientSuccess()), this, SLOT(onUploadClientSuccess()));
    connect(controller, SIGNAL(uploadClientFailed(NetworkParams &, QString &)), this, SLOT(onUploadClientFailed(NetworkParams &, QString &)));
    connect(controller, SIGNAL(updateRequestProcess(NetworkParams &)), this, SIGNAL(updateRequestProcess(NetworkParams &)));
    controller->StartWork();
    emit uploadingClient(params);
}

void MessageWidget::uploadFileByTCP() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    QString filePath;
    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (files.size() > 0) {
            filePath = files[0];
        }
    }

    if (filePath.isEmpty()) {
        return;
    }

    filePath = filePath.replace("\\", "/");
    qDebug() << "filePath:" << filePath;

    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.httpRequestType = REQUEST_UPLOAD_FILE_BY_TCP;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.requestTime = tools::GetInstance()->GetCurrentTime2();
    params.userID = g_userID;
    params.userName = g_userName;
    params.fileName = filePath.mid(filePath.lastIndexOf("/") + 1);
    params.filePath = filePath;
    params.fileLink = "http://" + g_serverHost + ":" + g_serverPort + "/uploads/" + params.fileName;
    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(uploadClientSuccess()), this, SLOT(onUploadClientSuccess()));
    connect(controller, SIGNAL(uploadClientFailed(NetworkParams &, QString &)), this, SLOT(onUploadClientFailed(NetworkParams &, QString &)));
    connect(controller, SIGNAL(updateRequestProcess(NetworkParams &)), this, SIGNAL(updateRequestProcess(NetworkParams &)));
    controller->StartWork();
    emit uploadingClient(params);
    return;

    //直接使用线程调用client发送文件，但是这样进度没法在文件列表展示进度
//    ClientThread *thread = new ClientThread(filePath, g_userName);
//    thread->start();
}

void MessageWidget::uploadFileToPersonalSpace() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    QString filePath;
    if (dialog.exec()) {
        QStringList files = dialog.selectedFiles();
        if (files.size() > 0) {
            filePath = files[0];
        }
    }

    if (filePath.isEmpty()) {
        return;
    }

    filePath = filePath.replace("\\", "/");
    qDebug() << "filePath:" << filePath;

    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.httpRequestType = REQUEST_UPLOAD_FILE;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.requestTime = tools::GetInstance()->GetCurrentTime2();
    params.userID = g_userID;
    params.userName = g_userName;
    params.fileName = filePath.mid(filePath.lastIndexOf("/") + 1);
    params.filePath = filePath;
    params.fileLink = "http://" + g_serverHost + ":" + g_serverPort + "/uploads/" + params.fileName;
    params.toUserID = g_userID;
    params.toUserName = g_userName;
    params.uploadToPersonalSpace = true;
    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(uploadClientSuccess()), this, SLOT(onUploadClientSuccess()));
    connect(controller, SIGNAL(uploadClientFailed(NetworkParams &, QString &)), this, SLOT(onUploadClientFailed(NetworkParams &, QString &)));
    connect(controller, SIGNAL(updateRequestProcess(NetworkParams &)), this, SIGNAL(updateRequestProcess(NetworkParams &)));
    controller->StartWork();
    emit uploadingClient(params);
    return;
}

void MessageWidget::onTimerOut() {
    g_WebSocket.ping();
    m_Timer->start(10 * 1000);
    qint64 seconds = m_ServerPongTime.secsTo(QDateTime::currentDateTime());
    if (abs(seconds) > 11) {
        qDebug() << "server may not available at:" << tools::GetInstance()->GetCurrentTime2();
        if (m_PingOvertimeCount > 3) {
            g_WebSocket.close();
            m_PingOvertimeCount = 0;
            return;
        }
        m_PingOvertimeCount++;
    }
}

void MessageWidget::onPongMessage(quint64 elapsedTime, const QByteArray &payload) {
    m_ServerPongTime = QDateTime::currentDateTime();
}
