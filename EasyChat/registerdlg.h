#ifndef REGISTERDLG_H
#define REGISTERDLG_H

#include "common.h"

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class RegisterDlg;
}

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDlg(QWidget *parent = nullptr);
    ~RegisterDlg();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void onRegisterFailed(QString &msg);
    void onRegisterSuccess();

private:
    Ui::RegisterDlg *ui;
};

#endif // REGISTERDLG_H
