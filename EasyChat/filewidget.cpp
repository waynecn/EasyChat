#include "filewidget.h"
#include "ui_filewidget.h"
#include "settingdlg.h"

FileWidget::FileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileWidget)
{
    ui->setupUi(this);

    ui->downLoadTableWidget->setColumnCount(6);
    ui->downLoadTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->downLoadTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList headerLables2;
    headerLables2.push_back(" 文件名称 ");
    headerLables2.push_back(" 下载时间 ");
    headerLables2.push_back(" 剩余时间 ");
    headerLables2.push_back(" 速度 ");
    headerLables2.push_back(" 进度 ");
    headerLables2.push_back(" 完成时间 ");
    ui->downLoadTableWidget->setHorizontalHeaderLabels(headerLables2);

    ui->uploadTableWidget->setColumnCount(6);
    ui->uploadTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->uploadTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList headerLables;
    headerLables.push_back(" 文件名称 ");
    headerLables.push_back(" 上传时间 ");
    headerLables.push_back(" 剩余时间 ");
    headerLables.push_back(" 速度 ");
    headerLables.push_back(" 进度 ");
    headerLables.push_back(" 完成时间 ");
    ui->uploadTableWidget->setHorizontalHeaderLabels(headerLables);
}

FileWidget::~FileWidget()
{
    delete ui;
}

void FileWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = true;
    }
    if (m_bCtrlPressed && e->key() == Qt::Key_E) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        dlg->exec();
    }

    e->accept();
}

void FileWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_bCtrlPressed = false;
    }

    e->accept();
}

void FileWidget::OnProcessFile(NetworkParams &params) {
    if (params.httpRequestType == REQUEST_UPLOAD_FILE) {
        if (!m_progressBarMap.contains(params.paramID)) {
            QProgressBar *progress = new QProgressBar();
            m_progressBarMap[params.paramID] = progress;
            progress->setMaximum(params.totalSize);
        }
        ui->uploadTableWidget->setRowCount(ui->uploadTableWidget->rowCount() + 1);
        int currentRow = ui->uploadTableWidget->rowCount() - 1;
        params.itemRow = currentRow;
        QTableWidgetItem *fileName = new QTableWidgetItem(params.fileName);
        fileName->setToolTip(params.fileName);
        QTableWidgetItem *requestTime = new QTableWidgetItem(params.requestTime);
        QTableWidgetItem *leftTime = new QTableWidgetItem(params.timeLeft);
        QTableWidgetItem *speed = new QTableWidgetItem(params.speed);
        QTableWidgetItem *doneTime = new QTableWidgetItem(params.requestEndTime);
        ui->uploadTableWidget->setItem(currentRow, 0, fileName);
        ui->uploadTableWidget->setItem(currentRow, 1, requestTime);
        ui->uploadTableWidget->setItem(currentRow, 2, leftTime);
        ui->uploadTableWidget->setItem(currentRow, 3, speed);
        ui->uploadTableWidget->setCellWidget(currentRow, 4, m_progressBarMap[params.paramID]);
        ui->uploadTableWidget->setItem(currentRow, 5, doneTime);
        m_networkParamsMap[params.paramID] = params;
    } else if (params.httpRequestType == REQUEST_DOWNLOAD_FILE || params.httpRequestType == REQUEST_DOWNLOAD_CLIENT) {
        if (!m_progressBarMap.contains(params.paramID)) {
            QProgressBar *progress = new QProgressBar();
            progress->setMaximum(params.totalSize);
            m_progressBarMap[params.paramID] = progress;
        }
        ui->downLoadTableWidget->setRowCount(ui->downLoadTableWidget->rowCount() + 1);
        int currentRow = ui->downLoadTableWidget->rowCount() - 1;
        params.itemRow = currentRow;
        QTableWidgetItem *fileName = new QTableWidgetItem(params.fileName);
        QTableWidgetItem *requestTime = new QTableWidgetItem(params.requestTime);
        QTableWidgetItem *leftTime = new QTableWidgetItem(params.timeLeft);
        QTableWidgetItem *speed = new QTableWidgetItem(params.speed);
        QTableWidgetItem *doneTime = new QTableWidgetItem(params.requestEndTime);
        fileName->setToolTip(params.saveFileDir + params.saveFileName);
        requestTime->setToolTip(params.saveFileDir + params.saveFileName);
        leftTime->setToolTip(params.saveFileDir + params.saveFileName);
        speed->setToolTip(params.saveFileDir + params.saveFileName);
        doneTime->setToolTip(params.saveFileDir + params.saveFileName);
        ui->downLoadTableWidget->setItem(currentRow, 0, fileName);
        ui->downLoadTableWidget->setItem(currentRow, 1, requestTime);
        ui->downLoadTableWidget->setItem(currentRow, 2, leftTime);
        ui->downLoadTableWidget->setItem(currentRow, 3, speed);
        ui->downLoadTableWidget->setCellWidget(currentRow, 4, m_progressBarMap[params.paramID]);
        ui->downLoadTableWidget->setItem(currentRow, 5, doneTime);
        m_networkParamsMap[params.paramID] = params;
    }
}

void FileWidget::OnUpdateRequestProcess(NetworkParams &params) {
    if (m_progressBarMap.contains(params.paramID)) {
        if (params.httpRequestType == REQUEST_UPLOAD_FILE) {
            ui->uploadTableWidget->item(m_networkParamsMap[params.paramID].itemRow, 2)->setText(QString("%1s").arg(params.timeLeft));
            ui->uploadTableWidget->item(m_networkParamsMap[params.paramID].itemRow, 3)->setText(QString("%1kB/s").arg(params.speed));
            ui->uploadTableWidget->item(m_networkParamsMap[params.paramID].itemRow, 5)->setText(params.requestEndTime);
            if (params.totalSize == 0 || params.recved == 0) {
                m_progressBarMap[params.paramID]->setMaximum(1);
                m_progressBarMap[params.paramID]->setValue(1);
            } else {
                m_progressBarMap[params.paramID]->setMaximum(params.totalSize);
                m_progressBarMap[params.paramID]->setValue(params.recved);
            }
        } else if (params.httpRequestType == REQUEST_DOWNLOAD_FILE) {
            ui->downLoadTableWidget->item(m_networkParamsMap[params.paramID].itemRow, 2)->setText(QString("%1s").arg(params.timeLeft));
            ui->downLoadTableWidget->item(m_networkParamsMap[params.paramID].itemRow, 3)->setText(QString("%1kB/s").arg(params.speed));
            ui->downLoadTableWidget->item(m_networkParamsMap[params.paramID].itemRow, 5)->setText(params.requestEndTime);
            m_progressBarMap[params.paramID]->setMaximum(params.totalSize);
            m_progressBarMap[params.paramID]->setValue(params.recved);
        }
    }
}
