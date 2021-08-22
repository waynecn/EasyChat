#include "userlistwidget.h"
#include "ui_userlistwidget.h"

UserListWidget::UserListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserListWidget)
{
    ui->setupUi(this);

    ui->userListTableWidget->setColumnCount(1);
    QTableWidgetItem *item = new QTableWidgetItem(" 在线用户 ");
    ui->userListTableWidget->setHorizontalHeaderItem(0, item);
    ui->userListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->userListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->userListTableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SIGNAL(onItemDoubleClicked(QTableWidgetItem *)));
}

UserListWidget::~UserListWidget()
{
    delete ui;
}

void UserListWidget::OnOnelineUsersMessage(QMap<QString, UserInfo> &onlineUsersMap) {
    QList<QString> keys = onlineUsersMap.keys();
    ui->userListTableWidget->setRowCount(keys.size());
    for (int i = 0; i < keys.size(); ++i) {
        QTableWidgetItem *userNameItem = new QTableWidgetItem(onlineUsersMap[keys[i]].userName);
        userNameItem->setToolTip(onlineUsersMap[keys[i]].userID);
        ui->userListTableWidget->setItem(i, 0, userNameItem);
    }
}
