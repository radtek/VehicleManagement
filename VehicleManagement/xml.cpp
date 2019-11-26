#include "xml.h"

XML::XML()
{

}
//创建xml空文件
void XML::createXML(QString filePath)
{
    QFile file(filePath);
    if(file.exists() == true)//文件存在不重新创建
    {
        return;
    }
    else
    {
        if(file.open(QIODevice::WriteOnly) == true)
        {
            //创建xml文档对象
            QDomDocument doc;
            //创建xml头部格式 <?xml version="1.0" encoding="utf-8"?>
            QDomProcessingInstruction ins;
            ins = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
            //追加元素
            doc.appendChild(ins);

            //根节点
            QDomElement root = doc.createElement("日销售清单");
            doc.appendChild(root);

            //保存
            QTextStream stream(&file);
            doc.save(stream, 4);//4为缩进字符

            file.close();

        }
        else
        {
            qDebug() << "文件名: " <<  __FILE__ << "行号: "
                     <<  __LINE__ << "函数名: "
                     <<  __FUNCTION__ << "文件打开失败";
            return;
        }
    }
}

void XML::appendXML(QString filePath, QStringList list)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly ) == false)
    {
        qDebug() << "文件名: " <<  __FILE__ << "行号: "
                 <<  __LINE__ << "函数名: "
                 <<  __FUNCTION__ << "文件打开失败";
        return;
    }

    //file和xml文档对象关联
    QDomDocument doc;
    if(doc.setContent(&file) == false)//如果关联失败
    {
        qDebug() << "文件名: " <<  __FILE__ << "行号: "
                 <<  __LINE__ << "函数名: "
                 <<  __FUNCTION__ << "文件关联失败";
        file.close();
        return;
    }

    //获取根节点
    QDomElement root = doc.documentElement();
    //获取当前时间
    QString date_str = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    //判断根节点下有没有子节点
    if(root.hasChildNodes() == false)//没有子节点
    {
        //创建日期子节点
        QDomElement date_emt = doc.createElement("日期");
        //创建date属性
        QDomAttr date_attr = doc.createAttribute("date");
        date_attr.setNodeValue(date_str);
        //节点和属性关联
        date_emt.setAttributeNode(date_attr);
        //把日期节点追加到根节点上
        root.appendChild(date_emt);

        //写有效数据
        writeXML(doc, date_emt, list);
    }
    else
    {
        //查找最后一个子节点
        QDomElement last_emt = root.lastChildElement();
        if(last_emt.attribute("date") == date_str)//如果有当天日期
        {
            //写有效数据
            writeXML(doc, last_emt, list);
        }
        else
        {
            //创建日期子节点
            QDomElement date_emt = doc.createElement("日期");
            //创建date属性
            QDomAttr date_attr = doc.createAttribute("date");
            date_attr.setNodeValue(date_str);
            //节点和属性关联
            date_emt.setAttributeNode(date_attr);
            //把日期节点追加到根节点上
            root.appendChild(date_emt);

            //写有效数据
            writeXML(doc, date_emt, list);

        }
    }

    //保存文件
    file.close();
    if(file.open(QIODevice::WriteOnly) == false)
    {
        qDebug() << "文件名: " <<  __FILE__ << "行号: "
                 <<  __LINE__ << "函数名: "
                 <<  __FUNCTION__ << "文件打开失败";
        file.close();
        return;
    }
    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

void XML::writeXML(QDomDocument &doc, QDomElement &root, QStringList &list)
{
    //当前时间
    QString date_str = QDateTime::currentDateTime().toString("hh-mm-ss");

    //创建元素
    //时间元素
    QDomElement tim_emt = doc.createElement("时间");
    QDomAttr tim_attr = doc.createAttribute("time");
    tim_attr.setNodeValue(date_str);
    tim_emt.setAttributeNode(tim_attr);
    root.appendChild(tim_emt);

    //厂家、品牌、报价、数量、金额元素
    QDomElement factory = doc.createElement("厂家");
    QDomElement brand = doc.createElement("品牌");
    QDomElement price = doc.createElement("报价");
    QDomElement num = doc.createElement("数量");
    QDomElement total = doc.createElement("金额");

    QDomText text = doc.createTextNode(list.at(0));
    factory.appendChild(text);

    text = doc.createTextNode(list.at(1));
    brand.appendChild(text);

    text = doc.createTextNode(list.at(2));
    price.appendChild(text);

    text = doc.createTextNode(list.at(3));
    num.appendChild(text);

    text = doc.createTextNode(list.at(4));
    total.appendChild(text);

    tim_emt.appendChild(factory);
    tim_emt.appendChild(brand);
    tim_emt.appendChild(price);
    tim_emt.appendChild(num);
    tim_emt.appendChild(total);
}
//
void XML::readXML(QString filePath, QStringList &factory_list,
                  QStringList &brand_list, QStringList &price_list,
                  QStringList &num_list, QStringList &tatol_list)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly ) == false)
    {
        qDebug() << "文件名: " <<  __FILE__ << "行号: "
                 <<  __LINE__ << "函数名: "
                 <<  __FUNCTION__ << "文件打开失败";
        return;
    }

    //file和xml文档对象关联
    QDomDocument doc;
    if(doc.setContent(&file) == false)//如果关联失败
    {
        qDebug() << "文件名: " <<  __FILE__ << "行号: "
                 <<  __LINE__ << "函数名: "
                 <<  __FUNCTION__ << "文件关联失败";
        file.close();
        return;
    }

    //获取根节点
    QDomElement root = doc.documentElement();
    file.close();

    QString date_str = QDateTime::currentDateTime().toString("yyyy-MM-dd");//日期
    //判断根节点下有没有子节点
    if(root.hasChildNodes() == false)//没有子节点
    {
        return;
    }
    else //有子节点
    {
        QDomElement last_emt = root.lastChildElement();
        //判断有没有当天日期
        if(last_emt.attribute("date") == date_str)
        {
            //找出当前日期下所有时间子节点
            QDomNodeList node_list = last_emt.childNodes();
            for(int i = 0; i < node_list.size(); ++i)
            {
                QDomNodeList sub_list = node_list.at(i).toElement().childNodes();
                //厂家
                QString factory = sub_list.at(0).toElement().text();
                factory_list.append(factory);

                QString brand = sub_list.at(1).toElement().text();
                brand_list.append(brand);

                QString price = sub_list.at(2).toElement().text();
                price_list.append(price);

                QString num = sub_list.at(3).toElement().text();
                num_list.append(num);

                QString tatol = sub_list.at(4).toElement().text();
                tatol_list.append(tatol);
            }
        }
        else
        {
            return;
        }
    }

}
