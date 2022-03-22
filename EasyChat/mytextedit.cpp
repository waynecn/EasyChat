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
    //当拷贝的文字内容中包含 a.b这种，sources->hasUrls为true 导致粘贴失效
    bool enabled = false;
    if (source->hasUrls()) {
        QList<QUrl> urls = source->urls();
        for (QUrl url : urls) {
            QString file = url.toLocalFile();

            if (QFile::exists(file)) {
                emit uploadFile(file);
                enabled = true;
            }
        }
        if (enabled)
            return;
    }
    QTextEdit::insertFromMimeData(source);
}
