#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H


#include <QTextEdit>

class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MyTextEdit(QWidget *parent = nullptr);
    ~MyTextEdit();

private:
    bool canInsertFromMimeData(const QMimeData *source);
    void insertFromMimeData(const QMimeData *source);

signals:
    void uploadFile(QString &filePath);
};

#endif // MYTEXTEDIT_H
