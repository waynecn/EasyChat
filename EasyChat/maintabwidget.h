#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include <QTabWidget>
#include <QKeyEvent>

namespace Ui {
class MainTabWidget;
}

class MainTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit MainTabWidget(QWidget *parent = nullptr);
    ~MainTabWidget();

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private:
    Ui::MainTabWidget *ui;
    bool m_bCtrlPressed;
};

#endif // MAINTABWIDGET_H
