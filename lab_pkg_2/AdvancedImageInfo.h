#ifndef ADVANCEDIMAGEINFO_H
#define ADVANCEDIMAGEINFO_H

#include <QString>
#include <QImage>

class AdvancedImageInfo {
public:
    static QString getDetailedInfo(const QString &filePath);

private:
    static QString getJpegInfo(const QString &filePath);
    static QString getGifInfo(const QString &filePath);
    static QString getPngInfo(const QString &filePath);
    static QString getTiffInfo(const QString &filePath);
};

#endif // ADVANCEDIMAGEINFO_H
