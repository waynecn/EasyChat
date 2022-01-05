#ifndef INPUTMESSAGEWIDGET_H
#define INPUTMESSAGEWIDGET_H
#include "common.h"

#include <QWidget>
#include <QKeyEvent>

namespace Ui {
class InputMessageWidget;
}

class InputMessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InputMessageWidget(QWidget *parent = nullptr);
    ~InputMessageWidget();
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void EnableSendMessageButton(bool b);
    QString GetInputMessage();
    void ClearMessageContent(); //clear input text editor content

private slots:
    void on_sendMessagePushButton_clicked();

private:
    Ui::InputMessageWidget *ui;
    bool m_bCtrlPressed;

signals:
    void sendMessage(MessageStruct &msg);
    void uploadFile(QString &filePath);
};

#endif // INPUTMESSAGEWIDGET_H
