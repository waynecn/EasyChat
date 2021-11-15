#include "registerdlg.h"
#include "ui_registerdlg.h"
#include "tools.h"
#include "mynetworkcontroller.h"

#include <QTextCodec>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonObject>
#include <QCloseEvent>
#include <QSettings>

RegisterDlg::RegisterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDlg)
{
    Qt::WindowFlags flags= this->windowFlags();
    setWindowFlags(flags&~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
}

RegisterDlg::~RegisterDlg()
{
    delete ui;
}

void RegisterDlg::on_okBtn_clicked()
{
    QString pwd1 = ui->passwordLineEdit->text();
    QString pwd2 = ui->confirmPasswordLineEdit->text();
    QString auth = ui->authLineEdit->text();
    if (pwd1 != pwd2) {
        QString msg = "两次输入的密码不一样";
        QMessageBox box;
        box.setWindowTitle("警告");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    if (auth.isEmpty()) {
        QString msg = "授权码不能为空";
        QMessageBox box;
        box.setWindowTitle("警告");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    if (auth != "10086") {
        QString msg = "授权码有误，请联系开发者QQ:635864540";
        QMessageBox box;
        box.setWindowTitle("警告");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.httpRequestType = REQUEST_REGISTER;
    params.userName = ui->userNameLineEdit->text();
    params.password = ui->passwordLineEdit->text();
    params.phoneNumber = ui->mobileLineEdit->text();
    params.requestTime = tools::GetInstance()->GetCurrentTime2();

    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(registerFailed(QString &)), this, SLOT(onRegisterFailed(QString &)));
    connect(controller, SIGNAL(registerSuccess()), this, SLOT(onRegisterSuccess()));
    controller->StartWork();
}

void RegisterDlg::on_cancelBtn_clicked()
{
    reject();
}

void RegisterDlg::onRegisterFailed(QString &msg) {
    QMessageBox box;
    box.setWindowTitle("警告");
    box.setText("注册失败:" + msg);
    box.addButton("确定", QMessageBox::AcceptRole);
    box.exec();
}

void RegisterDlg::onRegisterSuccess() {
    accept();
}
