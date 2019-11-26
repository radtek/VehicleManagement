#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //打开数据库
    connectDB();
    //初始化数据
    initData();

    XML::createXML("./SalesDiary.xml");
}

MainWindow::~MainWindow()
{
    delete ui;
}

//连接数据库
void MainWindow::connectDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("car");
    db.setUserName("root");
    db.setPassword("123456");
    if(db.open() == false)
    {
        QMessageBox::warning(this, tr("错误"),
                         db.lastError().text(),
                         QMessageBox::Ok);
        return;
    }

}
//初始化数据
void MainWindow::initData()
{
    car_last = 0;
    car_sell = 0;

    QSqlQueryModel *querymodel = new QSqlQueryModel(this);//新建模型
    querymodel->setQuery("select name from factory");//存储sql语句

    ui->comboBox_factory->setModel(querymodel);
    ui->comboBox_fac->setModel(querymodel);

    ui->label_last->setText(tr("0"));//剩余数量
    ui->lineEdit_total->setEnabled(false);//金额

    //****************新车入库********************
    //设置模型
    model = new QSqlTableModel(this);
    model->setTable("brand");//指定使用哪个表
    //把model放在view
    ui->tableView->setModel(model);
    //model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //显示model里的数据
    model->select();
    //model->setHeaderData(0, Qt::Horizontal, tr("id"));
    //设置model的编辑模式,手动提交修改
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

}

//车辆管理菜单槽函数
void MainWindow::on_action_car_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->car);
    ui->label->setText(tr("车辆管理"));
}
//销售统计菜单槽函数
void MainWindow::on_action_calc_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->calc);
    ui->label->setText(tr("销售统计"));
}
//厂家下拉框槽函数
void MainWindow::on_comboBox_factory_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "请选择厂家")
    {
        //内容清空
        ui->comboBox_brand->clear();//品牌下拉框情况
        ui->lineEdit_price->clear();//报价清空
        ui->label_last->setText("0");//剩余数量
        ui->lineEdit_total->clear();//金额清空
        ui->lineEdit_total->setEnabled(false);
        ui->spinBox->setValue(0);//数量选择框
        ui->spinBox->setEnabled(false);
        ui->pushButton_submit->setEnabled(false);
    }
    else
    {
        //先清空
        ui->comboBox_brand->clear();

        QSqlQuery query;
        QString sql = QString("select name from brand where factory = '%1'").arg(arg1);
        //执行sql语句
        query.exec(sql);
        while (query.next())
        {
            QString name = query.value(0).toString();
            ui->comboBox_brand->addItem(name);
        }

    }

}
//品牌槽函数
void MainWindow::on_comboBox_brand_currentIndexChanged(const QString &arg1)
{
    //清零
    ui->spinBox->setValue(0);
    ui->pushButton_submit->setEnabled(false);
    ui->spinBox->setEnabled(false);

    QSqlQuery query;
    QString sql = QString("select price, last, sell from brand where name = '%1'").arg(arg1);
    //执行sql语句
    query.exec(sql);
    while (query.next())
    {
        QString price = query.value(0).toString();//获取价格
        QString last = query.value(1).toString();//获取剩余数量
        QString sell = query.value(2).toString();//获取销售数量
        //更新成员变量
        car_last = last.toInt();
        car_sell = sell.toInt();
        //更新界面
        ui->lineEdit_price->setText(price);
        ui->label_last->setText(last);
    }

    //如果有剩余车辆数量
    if(car_last > 0)
    {
        ui->pushButton_submit->setEnabled(true);
        ui->spinBox->setEnabled(true);
    }
}
//数量选择框槽函数
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    //更新剩余数量
    int num = car_last - arg1;
    if(num < 0)//控制越界
    {
        ui->spinBox->setValue(car_last);
        ui->label_last->setText("0");
    }
    else
    {
        ui->label_last->setText(QString::number(num));
    }

    //报价
    int price = ui->lineEdit_price->text().toInt();
    int sum = price * ui->spinBox->value();
    ui->lineEdit_total->setText(QString::number(sum));
}
//取消按钮槽函数
void MainWindow::on_pushButton_cancel_clicked()
{
    ui->comboBox_factory->setCurrentIndex(0);
}
//确定按钮槽函数
void MainWindow::on_pushButton_submit_clicked()
{
    //获取信息
    //出售车辆
    int sum_sell = ui->spinBox->value() + car_sell;
    //剩余车辆
    int last_car = ui->label_last->text().toInt();

    //更新数据库
    QSqlQuery query;
    QString sql = QString("update brand set sell = %1, last = %2 where name = '%3'")
            .arg(sum_sell)
            .arg(last_car)
            .arg(ui->comboBox_brand->currentText());
    query.exec(sql);

    //更新xml文件
    QStringList list;
    list << ui->comboBox_factory->currentText()
         << ui->comboBox_brand->currentText()
         << ui->lineEdit_price->text()
         << ui->spinBox->text()
         << ui->lineEdit_total->text();
    XML::appendXML("./SalesDiary.xml", list);

    //显示销售清单
    ui->textEdit->clear();//清空
    ui->textEdit->setText("********日销售清单********");
    ui->textEdit->setAlignment(Qt::AlignCenter);
    QStringList factory_list, brand_list, price_list, num_list, tatol_list;
    XML::readXML("./SalesDiary.xml", factory_list, brand_list, price_list, num_list, tatol_list);
    for(int i = 0; i < factory_list.size(); ++i)
    {
        QString str = QString("[%1:%2]%3:%4:卖出了%5辆车,单价:%6,总价:%7")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"))
                .arg(QDateTime::currentDateTime().toString("hh-mm-ss"))
                .arg(factory_list.at(i))
                .arg(brand_list.at(i))
                .arg(num_list.at(i))
                .arg(price_list.at(i))
                .arg(tatol_list.at(i));
        ui->textEdit->append(str);
    }

    //响应界面
    on_comboBox_brand_currentIndexChanged(ui->comboBox_brand->currentText());

}
//toolbox槽函数
void MainWindow::on_toolBox_currentChanged(int index)
{
   if(index == 0)
   {
       ui->textEdit->show();
   }
   if(index ==1)
   {
       ui->textEdit->hide();
   }
}
//********************新车入库槽函数********************
//添加
void MainWindow::on_pushButton_add_clicked()
{
    //添加空记录
    QSqlRecord record = model->record();
    //获取多少行
    int row = model->rowCount();
    model->insertRecord(row, record);
}
//删除
void MainWindow::on_pushButton_delete_clicked()
{
    //获取选中的模型
    QItemSelectionModel *seletion_model = ui->tableView->selectionModel();
    //取出模型中的索引
    QModelIndexList list = seletion_model->selectedRows();
    //删除所有选中的行
    for(int i = 0; i < list.size(); i++)
    {
        model->removeRow(list.at(i).row());
    }
}
//撤销
void MainWindow::on_pushButton_cancel_2_clicked()
{
    model->revertAll();
    model->submitAll();
}
//提交
void MainWindow::on_pushButton_submit_2_clicked()
{
    model->submitAll();//提交动作
}

//**********************销售统计槽函数************************
//厂家下拉框
void MainWindow::on_comboBox_fac_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "请选择厂家")
    {
        return;
    }
    else
    {
        //先清空
        ui->comboBox_bra->clear();
        ui->widget->Clear();

        //画柱形图
        ui->widget->SetMaxValue(100);
        QSqlQuery query;
        QString sql = QString("select name from brand where factory = '%1'").arg(arg1);
        //执行sql语句
        query.exec(sql);
        while (query.next())
        {
            //品牌
            QString brand_name = query.value(0).toString();
            ui->comboBox_bra->addItem(brand_name);

            //品牌销量
            QString sql2 = QString("select sell from brand where name = '%1'").arg(brand_name);
            QSqlQuery query2;
            query2.exec(sql2);
            while(query2.next())
            {
                int sell = query2.value(0).toInt();
                //绘图
                ui->widget->AddItem(brand_name, sell);
            }
        }



    }

}
//品牌下拉框
void MainWindow::on_comboBox_bra_currentIndexChanged(const QString &arg1)
{
    //清零

    QSqlQuery query;
    QString sql = QString("select sell ,last from brand where name = '%1'").arg(arg1);
    double sell, last;
    //执行sql语句
    query.exec(sql);
    while (query.next())
    {
        sell = query.value(0).toDouble();//获取销售量
        last = query.value(1).toDouble();//获取剩余数量
    }

    //画饼
    QVector<SectorInfo> v;
    SectorInfo info;
    info.description = "已出售";
    info.percent = sell / (sell+last) * 100;
    v.push_back(info);

    info.description = "剩余";
    info.percent = last / (sell+last) * 100;
    v.push_back(info);

    ui->widget_2->setData(v);
}
