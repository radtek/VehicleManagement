#ifndef XML_H
#define XML_oH
#include <QString>
#include <QDomDocument>//文件
#include <QDomProcessingInstruction>//格式头部
#include <QDomElement>//元素
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QDateTime>

class XML
{
public:
    XML();
    static void createXML(QString filePath);//创建xml空文件
    static void appendXML(QString filePath, QStringList list);
    static void writeXML(QDomDocument &doc, QDomElement &root, QStringList &list);
    static void readXML(QString filePath, QStringList &factory_list,
                        QStringList &brand_list, QStringList &price_list,
                        QStringList &num_list, QStringList &tatol_list);
};

#endif // XML_H
