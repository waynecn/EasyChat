#include "mytextedit.h"

#include <QMimeData>
#include <QFile>
#include <QDebug>
MyTextEdit::MyTextEdit(QWidget *parent) : QTextEdit (parent)
{

}

MyTextEdit::~MyTextEdit() {

}

bool MyTextEdit::canInsertFromMimeData(const QMimeData *source) {
    return true;
}

void MyTextEdit::insertFromMimeData(const QMimeData *source) {
    if (source->hasUrls()) {
        QList<QUrl> urls = source->urls();
        for (QUrl url : urls) {
            QString file = url.toLocalFile();

            if (QFile::exists(file)) {
                emit uploadFile(file);
            }
        }
        return;
    }
    QTextEdit::insertFromMimeData(source);
}
