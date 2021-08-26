#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tools.h"
#include "settingdlg.h"

#include <QJsonObject>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pMainTabWidget(nullptr),
    m_pMainChatWidget(nullptr),
    m_pFileWidget(nullptr),
    m_pFileListTableWidget(nullptr),
    m_bCtrlPressed(false)
{
    Qt::WindowFlags flags= this->windowFlags();
    setWindowFlags(flags&~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    m_pMainTabWidget = new MainTabWidget();
    m_pMainChatWidget = new MainChatWidget();
    m_pFileWidget = new FileWidget();
    m_pFileListTableWidget = new FileListTableWidget();
    m_pMainTabWidget->addTab(m_pMainChatWidget, "聊天");
    m_pMainTabWidget->addTab(m_pFileWidget, "文件");
    m_pMainTabWidget->addTab(m_pFileListTableWidget, "文件列表");
    this->setCentralWidget(m_pMainTabWidget);

    connect(m_pMainChatWidget, SIGNAL(fileListRequestFinished(QJsonArray &)), m_pFileListTableWidget, SLOT(OnFileList(QJsonArray &)));
    connect(m_pMainChatWidget, SIGNAL(uploadingFile(NetworkParams &)), m_pFileWidget, SLOT(OnProcessFile(NetworkParams &)));
    connect(m_pMainChatWidget, SIGNAL(updateRequestProcess(NetworkParams &)), m_pFileWidget, SLOT(OnUpdateRequestProcess(NetworkParams &)));
    connect(m_pMainChatWidget, SIGNAL(downloadingFile(NetworkParams &)), m_pFileWidget, SLOT(OnProcessFile(NetworkParams &)));
    connect(m_pMainChatWidget, SIGNAL(uploadingClient(NetworkParams &)), m_pFileWidget, SLOT(OnProcessFile(NetworkParams &)));
    connect(m_pFileListTableWidget, SIGNAL(downloadItem(QTableWidgetItem *)), m_pMainChatWidget, SLOT(OnDownloadItem(QTableWidgetItem *)));
    m_pMainChatWidget->QueryFileList();
}

MainWindow::~MainWindow()
{
    if (nullptr != m_pMainChatWidget) {
        delete m_pMainChatWidget;
        m_pMainChatWidget = nullptr;
    }
    if (nullptr != m_pFileWidget) {
        delete m_pFileWidget;
        m_pFileWidget = nullptr;
    }
    if (nullptr != m_pFileListTableWidget) {
        delete m_pFileListTableWidget;
        m_pFileListTableWidget = nullptr;
    }
    if (nullptr != m_pMainTabWidget) {
        delete m_pMainTabWidget;
        m_pMainTabWidget = nullptr;
    }
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    e->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}


