#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <QMenu>
#include <QMouseEvent>

class MyTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    MyTableWidget(QWidget *parent = nullptr);
    ~MyTableWidget();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void onOpenFileDir();

private:
    QMenu   *m_pMenu;
};

#endif // MYTABLEWIDGET_H
