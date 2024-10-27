#include "mytextedit.h"
#include "tools.h"

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
    if (source->hasImage()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        QImage::Format format = image.format();
        switch (format) {
        case QImage::Format_Invalid:
            qDebug() << "invalid format";
            break;
        case QImage::Format_Mono:
            qDebug() << "Mono format";
            break;
        case QImage::Format_MonoLSB:
            qDebug() << "MonoLSB format";
            break;
        case QImage::Format_Indexed8:
            qDebug() << "Indexed8 format";
            break;
        case QImage::Format_RGB32:
            qDebug() << "RGB32 format";
            saveRGB32(image);
            break;
        case QImage::Format_ARGB32:
            qDebug() << "ARGB32 format";
            saveARGB32(image);
            break;
        case QImage::Format_ARGB8565_Premultiplied:
            qDebug() << "ARGB8565_Premultiplied format";
            break;
        case QImage::Format_RGB666:
            qDebug() << "RGB666 format";
            break;
        case QImage::Format_ARGB6666_Premultiplied:
            qDebug() << "ARGB6666_Premultiplied format";
            break;
        case QImage::Format_RGB555:
            qDebug() << "RGB555 format";
            break;
        case QImage::Format_ARGB8555_Premultiplied:
            qDebug() << "ARGB8555_Premultiplied format";
            break;
        case QImage::Format_RGB888:
            qDebug() << "RGB888 format";
            break;
        case QImage::Format_RGB444:
            qDebug() << "RGB444 format";
            break;
        case QImage::Format_ARGB4444_Premultiplied:
            qDebug() << "ARGB4444_Premultiplied format";
            break;
        }
        //image.save("");
    }
    QTextEdit::insertFromMimeData(source);
}

bool MyTextEdit::saveRGB32(QImage &image) {
    QString fileName = APPLICATION_TMPIMAGE_DIR + "/" + tools::GetInstance()->GetCurrentTime3() + ".PNG";
    qDebug() << "fileName:" << fileName;
    if (image.save(fileName)) {
        if (QFile::exists(fileName)) {
            emit uploadTmpFile(fileName);
        }
        return true;
    }
    qDebug() << "save image tmp file failed, fileName:" << fileName;
    return false;
}

bool MyTextEdit::saveARGB32(QImage &image) {
    QString fileName = APPLICATION_TMPIMAGE_DIR + "/" + tools::GetInstance()->GetCurrentTime3() + ".PNG";
    qDebug() << "fileName:" << fileName;
    if (image.save(fileName)) {
        if (QFile::exists(fileName)) {
            emit uploadTmpFile(fileName);
        }
        return true;
    }
    qDebug() << "save image tmp file failed, fileName:" << fileName;
    return false;
}
