#include "logindialog.h"
#include "ui_logindialog.h"
#include "common.h"
#include "mynetworkcontroller.h"
#include "tools.h"
#include "registerdlg.h"
#include "settingdlg.h"

#include <QMessageBox>
#include <QSettings>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    Qt::WindowFlags flags= this->windowFlags();
    setWindowFlags(flags&~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    QSettings settings;
    QString userName = settings.value(SETTING_USERNAME,"").toString();
    QString userPassword = settings.value(SETTING_USERPASSWORD,"").toString();
    bool rememberPassword = settings.value(SETTING_REMEMBER_PASSWORD, false).toBool();
    if (!userName.isEmpty()) {
        ui->userNameLineEdit->setText(userName);
    }
    if (rememberPassword && !userPassword.isEmpty()) {
        ui->passwordLineEdit->setText(userPassword);
        ui->checkBox->setChecked(true);
    } else {
        ui->checkBox->setChecked(false);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    e->accept();
}

void LoginDialog::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}

void LoginDialog::closeEvent(QCloseEvent */*e*/) {
    accept();
    exit(0);
}

void LoginDialog::on_loginPushButton_clicked()
{
    NetworkParams params;
    params.httpRequestType = REQUEST_LOGIN;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.requestTime = tools::GetInstance()->GetCurrentTime();
    params.userName = ui->userNameLineEdit->text();
    params.password = ui->passwordLineEdit->text();
    if (params.userName.isEmpty() || params.password.isEmpty()) {
        QMessageBox::warning(nullptr, "Warn", "用户名或密码不能为空", QMessageBox::Ok);
        return;
    }
    g_userName = params.userName;
    g_password = ui->passwordLineEdit->text();
    ui->loginPushButton->setEnabled(false);
    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(loginFailed(QString &)), this, SLOT(onLoginFailed(QString &)));
    connect(controller, SIGNAL(loginSuccess()), this, SLOT(onLoginSuccess()));
    controller->StartWork();
}

void LoginDialog::onLoginFailed(QString &msg) {
    QMessageBox::warning(nullptr, "Warn", "登录失败:" + msg);
    ui->loginPushButton->setEnabled(true);
    reject();
}

void LoginDialog::onLoginSuccess() {
    QSettings settings;
    settings.setValue(SETTING_USERNAME, ui->userNameLineEdit->text());
    settings.setValue(SETTING_USERPASSWORD, ui->passwordLineEdit->text());
    settings.setValue(SETTING_REMEMBER_PASSWORD, ui->checkBox->isChecked());
    accept();

    m_strWebsocketUrl = QString("ws://%1:%2/ws").arg(g_serverHost).arg(g_serverPort);
    QUrl url(m_strWebsocketUrl);
    g_WebSocket.open(url);
    connect(&g_WebSocket, SIGNAL(connected()), this, SLOT(OnWebSocketConnected()));
    connect(&g_WebSocket, SIGNAL(disconnected()), this, SLOT(OnWebSocketDisconnected()));
    connect(&g_WebSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnWebSocketError(QAbstractSocket::SocketError)));
}

void LoginDialog::on_registerPushButton_clicked()
{
    RegisterDlg *dlg = new RegisterDlg();
    dlg->exec();

    dlg->close();
    delete dlg;
    dlg = nullptr;
}



void LoginDialog::OnWebSocketConnected() {
    qDebug() << "OnWebSocketConnected";

    g_WebSocket.sendTextMessage(tools::GetInstance()->GenerateOnlineUserMessage());
}

void LoginDialog::OnWebSocketDisconnected() {
    qDebug() << "disconnected";

    QUrl url(m_strWebsocketUrl);
    g_WebSocket.open(url);
}

void LoginDialog::OnWebSocketError(QAbstractSocket::SocketError err) {
    qDebug() << "error happened" << WEBSOCKET_ERROR_STRINGS[err + 1];
}
