#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QWidget>

class Tools : public QObject
{
public:
    static QFileInfoList getFilesPath(QString path);
    static QString getUuid();

    static QString readerFile(QString path);
    static bool writerFile(QString path, QString text);
    static QStringList textToStringList(QString text);

    static int timestampFromDateTime(QString dateTime);
    static QString timestampToDateTime(int timestamp);

    static bool compareVersions(const QString string, const QString string1);
    static QStringList splitNotesData(QString string);

    static QGraphicsDropShadowEffect* createShadowEffect();

    static void changeWidgetBorder(QWidget *widget, const QString color, int width);
};

#endif // TOOLS_H