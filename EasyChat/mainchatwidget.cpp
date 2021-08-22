#include "mainchatwidget.h"
#include "ui_mainchatwidget.h"
#include "mynetworkcontroller.h"
#include "tools.h"
#include "settingdlg.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

MainChatWidget::MainChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainChatWidget),
    m_pMessageWidget(nullptr),
    m_pInputMessageWidget(nullptr),
    m_pUserListWidget(nullptr)
{
    ui->setupUi(this);
    ui->sendFilePushButton->setStyleSheet(SEND_FILE_BUTTON_STYLE_SHEET);
    ui->fileListPushButton->setStyleSheet(FILE_LIST_BUTTON_STYLE_SHEET);

    m_pMessageWidget = new MessageWidget();
    m_pInputMessageWidget = new InputMessageWidget();
    m_pUserListWidget = new UserListWidget();

    connect(m_pInputMessageWidget, SIGNAL(sendMessage(MessageStruct&)), m_pMessageWidget, SLOT(OnSendMessage(MessageStruct&)));

    ui->messageVerticalLayout->addWidget(m_pMessageWidget);
    ui->inputMessageVerticalLayout->addWidget(m_pInputMessageWidget);
    ui->userListHorizontalLayout->addWidget(m_pUserListWidget);

    connect(m_pMessageWidget, SIGNAL(onlineUsers(QMap<QString, UserInfo> &)), m_pUserListWidget, SLOT(OnOnelineUsersMessage(QMap<QString, UserInfo> &)));
    connect(m_pUserListWidget, SIGNAL(onItemDoubleClicked(QTableWidgetItem *)), m_pMessageWidget, SLOT(OnItemDoubleClicked(QTableWidgetItem *)));
}

MainChatWidget::~MainChatWidget()
{
    if (nullptr != m_pMessageWidget) {
        delete m_pMessageWidget;
        m_pMessageWidget = nullptr;
    }
    if (nullptr != m_pInputMessageWidget) {
        delete m_pInputMessageWidget;
        m_pInputMessageWidget = nullptr;
    }
    if (nullptr != m_pUserListWidget) {
        delete m_pUserListWidget;
        m_pUserListWidget = nullptr;
    }
    delete ui;
}

void MainChatWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    e->accept();
}

void MainChatWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}

void MainChatWidget::EnableSendMessageButton(bool b) {
    m_pInputMessageWidget->EnableSendMessageButton(b);
}

void MainChatWidget::QueryFileList() {
    on_fileListPushButton_clicked();
}

void MainChatWidget::OnDownloadItem(QTableWidgetItem *item) {
    if (item == nullptr || item->text().isEmpty()) {
        QString msg = "OnDownloadItem item为空或item->text为空";
        if (item != nullptr) {
            msg += ", text:" + item->text();
        }
        qWarning() << msg;
        return;
    }

    QFileDialog fDlg;
    fDlg.setAcceptMode(QFileDialog::AcceptSave);
    fDlg.setFileMode(QFileDialog::AnyFile);
    QString fileName = item->text();
    QSettings settings;
    QString saveFileDir = settings.value(SETTING_SAVE_FILE_DIR, "C:/").toString();
    fileName = fDlg.getSaveFileName(this, "Save File", saveFileDir + fileName);
    qDebug() << "fileName:" << fileName;
    if (fileName.isEmpty()) {
        return;
    }

    fileName = fileName.replace("\\", "/");
    qDebug() << "download file:" << item->text() << " save file:" << fileName;
    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.requestTime = tools::GetInstance()->GetCurrentTime();
    params.httpRequestType = REQUEST_DOWNLOAD_FILE;
    params.fileName = item->text();
    params.saveFileName = fileName.mid(fileName.lastIndexOf("/") + 1);
    params.userID = g_userID;
    params.userName = g_userName;
    params.saveFileDir = fileName.mid(0, fileName.lastIndexOf("/") + 1);

    saveFileDir = params.saveFileDir;
    settings.setValue(SETTING_SAVE_FILE_DIR, saveFileDir);

    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(requestFinished(NetworkParams &)), this, SLOT(OnRequestFinished(NetworkParams &)));
    connect(controller, SIGNAL(updateRequestProcess(NetworkParams &)), this, SIGNAL(updateRequestProcess(NetworkParams &)));
    connect(controller, SIGNAL(downloadFileFailed(NetworkParams &, QString &)), this, SLOT(onDownloadFileFailed(NetworkParams &, QString &)));
    controller->StartWork();
    //emit upload file actions for file widget to show the uploading progress
    emit downloadingFile(params);
}

void MainChatWidget::on_sendFilePushButton_clicked()
{
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
    params.userID = g_userID;
    params.userName = g_userName;
    params.toUserID = g_toUserID;
    params.toUserName = g_toUserName;
    params.filePath = filePath;
    QString fileName = filePath.mid(filePath.lastIndexOf("/") + 1);
    params.fileName = fileName;
    params.fileLink = "http://" + g_serverHost + ":" + g_serverPort + "/uploads/" + fileName;
    params.requestTime = tools::GetInstance()->GetCurrentTime();

    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(requestFinished(NetworkParams &)), this, SLOT(OnRequestFinished(NetworkParams &)));
    connect(controller, SIGNAL(updateRequestProcess(NetworkParams &)), this, SIGNAL(updateRequestProcess(NetworkParams &)));
    connect(controller, SIGNAL(uploadFileFailed(NetworkParams &, QString &)), this, SLOT(onUploadFileFailed(NetworkParams &, QString &)));
    controller->StartWork();
    //emit upload file actions for file widget to show the uploading progress
    emit uploadingFile(params);
}

void MainChatWidget::OnRequestFinished(NetworkParams &params) {
    if (params.httpRequestType == REQUEST_GET_UPLOAD_FILES) {
        return;
    }
    MessageStruct msgStruct;
    msgStruct.userID = g_userID;
    msgStruct.userName = g_userName;
    msgStruct.toUserID = g_toUserID;
    msgStruct.toUserName = g_toUserName;
    msgStruct.sendTime = tools::GetInstance()->GetCurrentTime();
    msgStruct.msg = m_pInputMessageWidget->GetInputMessage();
    if (params.httpRequestType == REQUEST_UPLOAD_FILE) {
        msgStruct.fileName = params.fileName;
        msgStruct.fileUrl = params.fileLink;
        m_pMessageWidget->SendMessage(msgStruct);
        m_pInputMessageWidget->ClearMessageContent();
    } else if (params.httpRequestType == REQUEST_DOWNLOAD_FILE) {
        qDebug() << "下载完成，保存至：" << params.saveFileDir;
    }
}

void MainChatWidget::on_fileListPushButton_clicked()
{
    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.httpRequestType = REQUEST_GET_UPLOAD_FILES;
    params.userID = g_userID;
    params.userName = g_userName;
    params.requestTime = tools::GetInstance()->GetCurrentTime();

    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(fileListRequestFinished(NetworkParams &, QJsonArray &)), this, SIGNAL(fileListRequestFinished(NetworkParams &, QJsonArray &)));
    controller->StartWork();
}

void MainChatWidget::onUploadFileFailed(NetworkParams &params, QString &msg) {
    QMessageBox::warning(nullptr, "WARN", "上传文件:" + params.fileName + " 失败:" + msg);
}

void MainChatWidget::onDownloadFileFailed(NetworkParams &params, QString &msg) {
    QMessageBox::warning(nullptr, "WARN", "下载文件:" + params.fileName + " 失败:" + msg);
}
