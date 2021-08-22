#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QAbstractSocket>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private slots:
    void onLoginSuccess();
    void onLoginFailed(QString &msg);
    void on_loginPushButton_clicked();

    void on_registerPushButton_clicked();

    void OnWebSocketConnected();
    void OnWebSocketDisconnected();
    void OnWebSocketError(QAbstractSocket::SocketError err);

private:
    Ui::LoginDialog *ui;
    bool m_bCtrlPressed;
    QString m_strWebsocketUrl;
};

#endif // LOGINDIALOG_H
