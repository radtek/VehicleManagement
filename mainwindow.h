#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlTableModel>
#include "xml.h"
#include "histogram.h"
#include "pie.h"
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connectDB();//连接数据库
    void initData();//初始化

private slots:
    void on_action_car_triggered();

    void on_action_calc_triggered();

    void on_comboBox_factory_currentIndexChanged(const QString &arg1);

    void on_comboBox_brand_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_pushButton_cancel_clicked();

    void on_pushButton_submit_clicked();

    void on_toolBox_currentChanged(int index);

    void on_pushButton_add_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_cancel_2_clicked();

    void on_pushButton_submit_2_clicked();

    void on_comboBox_fac_currentIndexChanged(const QString &arg1);

    void on_comboBox_bra_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    int car_last;//剩余车辆
    int car_sell;//出售数量
    QSqlTableModel *model;
};

#endif // MAINWINDOW_H
