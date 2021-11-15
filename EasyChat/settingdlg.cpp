#include "settingdlg.h"
#include "ui_settingdlg.h"

#include <QMessageBox>
#include <QSettings>

SettingDlg *SettingDlg::m_pInstance = nullptr;
SettingDlg::SettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDlg)
{
    Qt::WindowFlags flags= this->windowFlags();
    setWindowFlags(flags&~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    QSettings setting;
    QString host = setting.value(SETTING_WEBSOCKET_SERVER_HOST, "").toString();
    QString port = setting.value(SETTING_SERVER_PORT, "").toString();
    bool bTCP = setting.value(SETTING_USE_TCP, false).toBool();
    if (!host.isEmpty()) {
        QStringList ipList = host.split(",");
        for (int i = 0; i < ipList.size(); ++i) {
            ui->ipList->addItem(ipList[i]);
        }
    }
    if (!setting.value(SETTING_CURRENT_SERVER_HOST, "").toString().isEmpty()) {
        ui->ipList->setCurrentText(setting.value(SETTING_CURRENT_SERVER_HOST).toString());
    }
    if (!port.isEmpty()) {
        ui->portLineEdit->setText(port);
    }
    ui->TCPSwitchRadio->setChecked(bTCP);

    ui->versionLabel->setText("当前版本：" + APPLICATION_VERSION);
}

SettingDlg::~SettingDlg()
{
    delete ui;
}

void SettingDlg::on_okBtn_clicked()
{
    QString currentIP = ui->ipList->currentText();
    QString port = ui->portLineEdit->text();
    bool bTCP = ui->TCPSwitchRadio->isChecked();
    if (currentIP.isEmpty() || port.isEmpty()) {
        QString msg = "IP或端口不能为空";
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    QStringList ipList = currentIP.split(".");
    if (ipList.size() != 4) {
        QString msg = "IP地址填写有误";
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    for (int i = 0; i < ipList.size(); ++i) {
        int nItem = ipList[i].toInt();
        if (nItem < 0 || nItem > 255) {
            QString msg = "IP地址非法";
            QMessageBox box;
            box.setWindowTitle("提示");
            box.setText(msg);
            box.addButton("确定", QMessageBox::AcceptRole);
            box.exec();
            return;
        }
    }

    bool bOk = false;
    int nPort = port.toInt(&bOk);
    if (!bOk || nPort < 0) {
        QString msg = "端口非法";
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    QSettings setting;

    bool bExit = false;
    QString oldIP = setting.value(SETTING_WEBSOCKET_SERVER_HOST).toString();
    QString oldPort = setting.value(SETTING_SERVER_PORT).toString();
    QString currentServerHost = setting.value(SETTING_CURRENT_SERVER_HOST).toString();
    bool useTCP = setting.value(SETTING_USE_TCP, false).toBool();
    if (((!currentServerHost.isEmpty() || !oldPort.isEmpty()) && (currentServerHost != currentIP || oldPort != port)) ||
            useTCP != bTCP) {
        QString msg = "配置发生改变,需要重启程序";
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText(msg);
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        bExit = true;
    }

    bool bExist = false;
    QStringList oldIPList = oldIP.split(",");
    for (int i = 0; i < oldIPList.size(); ++i) {
        if (currentIP.compare(oldIPList[i]) == 0) {
            bExist = true;
            break;
        }
    }
    if (!bExist && !oldIP.isEmpty()) {
        currentIP += "," + oldIP;
    }

    if (!bExist) {
        setting.setValue(SETTING_WEBSOCKET_SERVER_HOST, currentIP);
    }
    setting.setValue(SETTING_CURRENT_SERVER_HOST, ui->ipList->currentText());
    setting.setValue(SETTING_SERVER_PORT, port);
    setting.setValue(SETTING_USE_TCP, bTCP);
    accept();
    if (bExit) {
        RestartApp();
        exit(-1);
    }
}

void SettingDlg::on_cancelBtn_clicked()
{
    reject();
}
