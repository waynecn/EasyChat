#include "maintabwidget.h"
#include "ui_maintabwidget.h"
#include "common.h"
#include "settingdlg.h"

MainTabWidget::MainTabWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::MainTabWidget)
{
    ui->setupUi(this);

    setStyleSheet(TAB_BAR_STYLE_SHEET);
}

MainTabWidget::~MainTabWidget()
{
    delete ui;
}

void MainTabWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    e->accept();
}

void MainTabWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}
