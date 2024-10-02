/*************************************************************************************
**                                                                                  **
** UCO - Serial Commander 300, a GUI to monitor the ABB Commander 300 UPC           **
**    Copyright (C) 2024  University of Córdoba                                     **
**                                                                                  **
**    This program is free software: you can redistribute it and/or modify          **
**    it under the terms of the GNU General Public License as published by          **
**    the Free Software Foundation, either version 3 of the License, or any         **
**    later version.                                                                **
**                                                                                  **
**    This program is distributed in the hope that it will be useful,               **
**    but WITHOUT ANY WARRANTY; without even the implied warranty of                **
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 **
**    GNU General Public License for more details.                                  **
**                                                                                  **
**    You should have received a copy of the GNU General Public License             **
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.        **
**                                                                                  **
**************************************************************************************
**  Contact: juan.garrido@uco.es                                                    **
**             Date: 05.06.24                                                       **
**          Version: 1.0.0                                                          **
**************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"
#include "qserialport.h"
#include <QMainWindow>

#include <QElapsedTimer>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupRealtimeDataDemo(QCustomPlot *customPlot);
    void save_file();
    void initial_window_config();
    void serial_port_Config();
    bool mode_func = 0;
    void manual_mode();
    void auto_mode();
    void set_value_SP(QString SP);
    void set_value_OP(QString OP);
    void set_value_KP(QString KP);
    void disconnect_s();
    bool get_func_mode();

    bool disconnect_var = 0;
    bool reset_zoom_var = 1;
    bool range_update = 1;

    QString get_value_SP();
    QString get_value_OP();
    void get_value_PV();

    QString get_value_KP();
    QString get_value_Ti();
    QString get_value_Td();

    //QByteArray cad[32];
    char cad[32];
    //QByteArray received_data;

    QString textoSP;
    QString gettextoSP;
    QString textoSPaux;

    QString textoOP;
    QString gettextoOP;

    QString textoOPaux;
    QString textoKP;
    QString textoTi;
    QString textoTd;
    QString textoTiaux;
    QString textoTdaux;

    QCPRange previousRange;


    //QString csv_dir ="C:/Users/Automática/Desktop";
    QString csv_dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString csv_save_file=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/Test_300.csv";
    bool save_data=0;

    bool paramSP = false;

    bool updateParam = false;
    bool paramOP = false;
    bool mensaje_entendido=0;

    QString textoPV;

    QString textoKPaux;

    QElapsedTimer thread_time_gl;
    QElapsedTimer timerThread_t;
    double key_plot;

    bool mode_change=0;
    bool func_mode_initial;
    bool start_thread=1;
    int check_mode=0;

    void get_parameters();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_4_clicked();
    void on_lineEdit_3_returnPressed();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_comboBox_activated(int index);
    void on_pushButton_5_clicked();
    void realtimeplot();
    void timerThread();

    void on_spinBox_editingFinished();

    void on_spinBox_2_editingFinished();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;

    QString portName;
    QSerialPort *serial_p;
    bool connection=0;

    QPen graphPen_OP;
    QPen graphPen_PV;
    QPen graphPen_SP;
    QPen graphPen_Hist;

    double valor_prueba=0;

    QTimer dataTimer;
    double key=0;
    int x_axis_range=300;
};
#endif // MAINWINDOW_H
