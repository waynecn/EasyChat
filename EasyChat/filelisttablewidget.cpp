#include "filelisttablewidget.h"
#include "ui_filelisttablewidget.h"
#include "mynetworkcontroller.h"
#include "tools.h"
#include "settingdlg.h"

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QClipboard>
#include <QMessageBox>

FileListTableWidget::FileListTableWidget(QWidget *parent) :
    QTableWidget(parent),
    ui(new Ui::FileListTableWidget),
    m_pMenu(nullptr)
{
    ui->setupUi(this);
    setSelectionMode(QAbstractItemView::SingleSelection);

    this->setColumnCount(4);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList headerLables;
    headerLables.push_back(" 文件 ");
    headerLables.push_back(" 文件大小 ");
    headerLables.push_back(" 用户 ");
    headerLables.push_back(" 操作 ");
    this->setHorizontalHeaderLabels(headerLables);

    m_pMenu = new QMenu();
    QAction *copyLink = m_pMenu->addAction("拷贝链接");
    QAction *download = m_pMenu->addAction("下载");
    QAction *refresh = m_pMenu->addAction("刷新");
    m_pMenu->addSeparator();
    QAction *deleteFile = m_pMenu->addAction("删除");

    connect(this, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(onItemClicked(QTableWidgetItem *)));

    connect(copyLink, SIGNAL(triggered(bool)), this, SLOT(onCopyLink()));
    connect(download, SIGNAL(triggered(bool)), this, SLOT(download()));
    connect(refresh, SIGNAL(triggered(bool)), this, SLOT(onRefreshFileList()));
    connect(deleteFile, SIGNAL(triggered(bool)), this, SLOT(deleteFile()));
}

FileListTableWidget::~FileListTableWidget()
{
    if (m_pMenu != nullptr) {
        delete m_pMenu;
        m_pMenu = nullptr;
    }
    delete ui;
}

void FileListTableWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    e->accept();
}

void FileListTableWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}

void FileListTableWidget::mousePressEvent(QMouseEvent *event) {
    QTableWidget::mousePressEvent(event);
}

void FileListTableWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {

        m_pMenu->exec(event->globalPos());
    }
    if (event->button() == Qt::LeftButton) {
        this->itemClicked(currentItem());
    }
    event->ignore();
}

void FileListTableWidget::OnFileList(QJsonArray &files) {
    this->resize(this->size());
    int size = files.size();
    this->setRowCount(size);

    for (int i = 0; i < size; ++i) {
        QJsonObject obj = files[i].toObject();
        QTableWidgetItem *item = new QTableWidgetItem(obj["FileName"].toString());
        this->setItem(i, 0, item);
        this->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(obj["FileSize"].toInt())));
        QString uploadUser = obj["UploadUser"].toObject()["Valid"].toBool() ? obj["UploadUser"].toObject()["String"].toString() : "未知";
        this->setItem(i, 2, new QTableWidgetItem(uploadUser));
        this->setItem(i, 3, new QTableWidgetItem("下载"));
    }
    update();
}

void FileListTableWidget::onItemClicked(QTableWidgetItem *item) {
    if (item->column() == 3) {
        qDebug() << item->text();
        QTableWidgetItem * fileNameItem = this->item(item->row(), 0);

        emit downloadItem(fileNameItem);
    }
}

void FileListTableWidget::onCopyLink() {
    QString url = "http://" + g_serverHost + ":" + g_serverPort + "/uploads/" + this->item(this->currentItem()->row(), 0)->text();

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(url);
}
void FileListTableWidget::download() {
    QTableWidgetItem *item = this->currentItem();
    QTableWidgetItem *fileNameItem = this->item(item->row(), 0);

    emit downloadItem(fileNameItem);
}

void FileListTableWidget::onRefreshFileList() {
    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.httpRequestType = REQUEST_GET_UPLOAD_FILES;
    params.requestTime = tools::GetInstance()->GetCurrentTime();
    params.userID = g_userID;
    params.userName = g_userName;
    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(fileListRequestFinished(QJsonArray &)), this, SLOT(OnFileList(QJsonArray &)));
    controller->StartWork();
}

void FileListTableWidget::deleteFile() {
    QTableWidgetItem *item = currentItem();
    QTableWidgetItem *fileNameItem = this->item(item->row(), 0);

    QString fileName = fileNameItem->text();

    QMessageBox box;
    box.setWindowTitle("提示");
    box.setText("确定要删除文件:" + fileName + "吗？");
    box.addButton("确定", QMessageBox::AcceptRole);
    box.addButton("取消", QMessageBox::RejectRole);
    int nRet = box.exec();
    if (nRet != 0) {
        return;
    }

    NetworkParams params;
    params.paramID = tools::GetInstance()->GenerateRandomID();
    params.httpRequestType = REQUEST_DELETE_FILE;
    params.fileName = fileName;
    params.userID = g_userID;
    params.userName = g_userName;
    params.requestTime = tools::GetInstance()->GetCurrentTime();
    MyNetworkController *controller = new MyNetworkController(params);
    connect(controller, SIGNAL(deleteFileSuccess()), this, SLOT(onDeleteFileSuccess()));
    connect(controller, SIGNAL(deleteFileFailed(NetworkParams &, QString &)), this, SLOT(onDeleteFileFailed(NetworkParams &, QString &)));
    controller->StartWork();
}

void FileListTableWidget::onDeleteFileSuccess() {
//    NetworkParams params;
//    params.paramID = tools::GetInstance()->GenerateRandomID();
//    params.httpRequestType = REQUEST_GET_UPLOAD_FILES;
//    params.userID = g_userID;
//    params.userName = g_userName;
//    params.requestTime = tools::GetInstance()->GetCurrentTime();
//    MyNetworkController *controller = new MyNetworkController(params);
//    connect(controller, SIGNAL(fileListRequestFinished(NetworkParams &, QJsonArray &)), this, SLOT(OnFileList(NetworkParams &, QJsonArray &)));
//    controller->StartWork();
    onRefreshFileList();
}

void FileListTableWidget::onDeleteFileFailed(NetworkParams &params, QString &msg) {
    QMessageBox box;
    box.setWindowTitle("提示");
    box.setText("删除文件:" + params.fileName + "失败:" + msg);
    box.addButton("确定", QMessageBox::AcceptRole);
    box.exec();
}
