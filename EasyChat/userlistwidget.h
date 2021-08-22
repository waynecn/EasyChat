#ifndef USERLISTWIDGET_H
#define USERLISTWIDGET_H

#include "common.h"

#include <QWidget>
#include <QMap>
#include <QTableWidgetItem>

namespace Ui {
class UserListWidget;
}

class UserListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserListWidget(QWidget *parent = nullptr);
    ~UserListWidget();

public slots:
    void OnOnelineUsersMessage(QMap<QString, UserInfo> &onlineUsersMap);

private:
    Ui::UserListWidget *ui;

signals:
    void onItemDoubleClicked(QTableWidgetItem *item);
};

#endif // USERLISTWIDGET_H
