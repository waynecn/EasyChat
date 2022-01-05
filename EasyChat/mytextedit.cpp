#include "mytextedit.h"

#include <QMimeData>
#include <QFile>

MyTextEdit::MyTextEdit(QWidget *parent) : QTextEdit (parent)
{

}

MyTextEdit::~MyTextEdit() {

}

bool MyTextEdit::canInsertFromMimeData(const QMimeData *source) {
    if (source->hasUrls()) {
        return true;
    }
}

void MyTextEdit::insertFromMimeData(const QMimeData *source) {
    QList<QUrl> urls = source->urls();
    for (QUrl url : urls) {
        QString file = url.toLocalFile();

        if (QFile::exists(file)) {
            emit uploadFile(file);
        }
    }
}
