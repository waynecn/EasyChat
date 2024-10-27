#include "inputmessagewidget.h"
#include "ui_inputmessagewidget.h"
#include "tools.h"
#include "settingdlg.h"

InputMessageWidget::InputMessageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputMessageWidget)
{
    ui->setupUi(this);

    ui->sendMessagePushButton->setStyleSheet(SEND_MESSAGE_BUTTON_STYLE_SHEET);

    connect(ui->inputMessageTextEdit, SIGNAL(uploadFile(QString &)), this, SIGNAL(uploadFile(QString &)));
    connect(ui->inputMessageTextEdit, SIGNAL(uploadTmpFile(QString &)), this, SIGNAL(uploadTmpFile(QString &)));
}

InputMessageWidget::~InputMessageWidget()
{
    delete ui;
}

void InputMessageWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_Return) {
        on_sendMessagePushButton_clicked();
    }

    e->accept();
}

void InputMessageWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}

void InputMessageWidget::on_sendMessagePushButton_clicked()
{
    QString msg = ui->inputMessageTextEdit->toPlainText();
    if (msg.isEmpty()) {
        return;
    }

    MessageStruct msgStruct;
    msgStruct.msgType = NORMAL_MSG;
    msgStruct.msg = msg;
    msgStruct.userID = g_userID;
    msgStruct.userName = g_userName;
    msgStruct.sendTime = tools::GetInstance()->GetCurrentTime2();

    emit sendMessage(msgStruct);
    ui->inputMessageTextEdit->clear();
}

void InputMessageWidget::EnableSendMessageButton(bool b) {
    ui->sendMessagePushButton->setEnabled(b);
}

QString InputMessageWidget::GetInputMessage() {
    return ui->inputMessageTextEdit->toPlainText();
}

void InputMessageWidget::ClearMessageContent() {
    ui->inputMessageTextEdit->clear();
}
