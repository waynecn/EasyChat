#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include "common.h"

#include <QWidget>
#include <QMap>
#include <QProgressBar>
#include <QKeyEvent>

namespace Ui {
class FileWidget;
}

class FileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWidget(QWidget *parent = nullptr);
    ~FileWidget();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

public slots:
    void OnProcessFile(NetworkParams &params);
    void OnUpdateRequestProcess(NetworkParams &params);

private:
    Ui::FileWidget *ui;
    QMap<QString, QProgressBar*> m_progressBarMap;
    QMap<QString, NetworkParams> m_networkParamsMap;
    bool m_bCtrlPressed;
};

#endif // FILEWIDGET_H
