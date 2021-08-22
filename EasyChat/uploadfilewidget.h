#ifndef UPLOADFILEWIDGET_H
#define UPLOADFILEWIDGET_H

#include <QObject>
#include <QTableWidget>

class UploadFileWidget : public QTableWidget
{
    Q_OBJECT

public:
    UploadFileWidget(QWidget *parent = nullptr);
    ~UploadFileWidget();
};

#endif // UPLOADFILEWIDGET_H
