#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "maintabwidget.h"
#include "mainchatwidget.h"
#include "filewidget.h"
#include "filelisttablewidget.h"

#include <QMainWindow>
#include <QSettings>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private slots:
    void onFileListButtonClicked();
    void onCurrentChanged(int index);

private:
    Ui::MainWindow *ui;
    MainTabWidget *m_pMainTabWidget;
    MainChatWidget *m_pMainChatWidget;
    FileWidget *m_pFileWidget;
    QSettings m_Settings;
    QString m_strWebsocketUrl;
    FileListTableWidget *m_pFileListTableWidget;
    bool m_bCtrlPressed;
    bool m_bFirstRun;

signals:
};

#endif // MAINWINDOW_H
