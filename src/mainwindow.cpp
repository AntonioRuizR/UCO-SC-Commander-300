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

// Qt 5.15.2 project compiled with MinGW 8.1.0 32-bit

#include "mainwindow.h"
#include "dialog_about.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringRef>
#include <QTextCodec>
#include <QTimer>
#include <QFocusEvent>

// GUI START
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("UCO - Serial Commander 300"));
    initial_window_config(); //Initial configuration of GUI's elements
    serial_port_Config(); //Initial configuration of serial port
    setupRealtimeDataDemo(ui->plot); //Initial configuration of graph widget
}

// CLOSE GUI
MainWindow::~MainWindow()
{
    //When GUI is closed, thread timer is stopped and serial port connection is closed
    dataTimer.stop();
    if(serial_p->isOpen()){
        serial_p->close();
        connection=0;
        disconnect_var=0;
    }
    delete ui;
}

// INITIAL GUI CONFIG
void MainWindow::initial_window_config()
{
    QPixmap logo_uco(":/images/uco.png");
    QPixmap logo_prinia(":/images/prinia_logo.jpg");
    int width_logouco = ui->label_logouco->width();
    int height_logouco = ui->label_logouco->height();
    int width_logo_prinia = ui->label_logoprinia->width();
    int height_logo_prinia = ui->label_logoprinia->height();
    ui->label_logouco->setPixmap(logo_uco.scaled(width_logouco,height_logouco,Qt::KeepAspectRatio));
    ui->label_logoprinia->setPixmap(logo_prinia.scaled(width_logo_prinia,height_logo_prinia,Qt::KeepAspectRatio));

    ui->label_2->setEnabled(false);
    ui->label_3->setEnabled(false);
    ui->spinBox->setEnabled(false);
    ui->spinBox_2->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
}

// SERIAL PORT CONFIG
void MainWindow::serial_port_Config()
{
    serial_p = new QSerialPort;
    serial_p->setBaudRate(QSerialPort::Baud9600);
    serial_p->setDataBits(QSerialPort::Data8);
    serial_p->setParity(QSerialPort::NoParity);
    serial_p->setStopBits(QSerialPort::OneStop);
    serial_p->setFlowControl(QSerialPort::NoFlowControl);

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        portName = serialPortInfo.portName();
        ui->comboBox->addItem(serialPortInfo.portName());
    }
}

// PLOT INITIAL CONFIGURATION
void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    // Antialiasing disabled
    customPlot->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    customPlot->xAxis->setTickLabelFont(font);
    customPlot->yAxis->setTickLabelFont(font);
    customPlot->legend->setFont(font);

    QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
    ticker->setTickCount(10); // Esto establecerá 5 divisiones principales
    ticker->setTickStepStrategy(QCPAxisTicker::tssReadability); // Esto hará que las divisiones sean legibles

    customPlot->xAxis->setTicker(ticker);
    customPlot->yAxis->setTicker(ticker);
    customPlot->replot();

    //Graph style OP
    graphPen_OP.setColor(Qt::green);
    graphPen_OP.setWidthF(2);

    ui->label_6->setStyleSheet("QLabel { color : green; }");
    ui->label_7->setStyleSheet("QLabel { color : green; }");

    //Graph style PV

    graphPen_PV.setColor(Qt::blue);
    graphPen_PV.setWidthF(2);

    ui->label_8->setStyleSheet("QLabel { color : blue; }");
    ui->label_9->setStyleSheet("QLabel { color : blue; }");

    //Graph style SP

    graphPen_SP.setColor(Qt::red);
    graphPen_SP.setWidthF(2);

    ui->label_10->setStyleSheet("QLabel { color : red; }");
    ui->label_11->setStyleSheet("QLabel { color : red; }");

    //Graph style distance 4

    graphPen_Hist.setColor(Qt::magenta);
    graphPen_Hist.setWidthF(2);

    //Graph OP

    customPlot->addGraph();
    customPlot->graph(0)->setPen(graphPen_OP);
    customPlot->graph(0)->setName("OP");

    //Graph PV:

    customPlot->addGraph();
    customPlot->graph(1)->setPen(graphPen_PV);
    customPlot->graph(1)->setName("PV");

    //Alarm SP:

    customPlot->addGraph();
    customPlot->graph(2)->setPen(graphPen_SP);
    customPlot->graph(2)->setName("SP");


    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(0, 100.2);
    customPlot->xAxis->setRange(0, x_axis_range);
    customPlot->xAxis->setLabel("Seconds");
    customPlot->yAxis->setLabel("%");
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    customPlot->xAxis->setRangeLower(0);
    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

/***************************************************
**                                               **
**    MAIN THREAD LINKED WITH TIMER (500 MS)     **
**                                               **
***************************************************/

void MainWindow::timerThread(){

    if(serial_p->isOpen()){

        if(check_mode<10){                          // Every 10 iterations, checks the current functioning mode and changes it if required
            check_mode++;
        }
        else{
            check_mode=0;
            bool current_mode = get_func_mode();
            if(current_mode != mode_func){
                if(current_mode==0){
                    on_pushButton_2_clicked();
                }
                else{
                    on_pushButton_3_clicked();
                }
            }
        }

        if(updateParam == true){                   // If a parameter has been modified, it is sent to the Commander 300
            if(paramSP == true){
                set_value_SP(textoSP);
            }
            if(paramOP == true){
                set_value_OP(textoOP);
            }
            updateParam = false;
        }

        if(mode_func == 1){                        // If Commander 300 is in Auto mode, SP value is obtained
            if(paramSP == false){
                gettextoSP = get_value_SP();
                ui->label_11->setText(gettextoSP);
            }
        }

        get_value_PV();                            // PV value is obtained
        ui->label_9->setText(textoPV);

        gettextoOP = get_value_OP();               // OP value is obtained
        ui->label_7->setText(gettextoOP);

        get_parameters();                          // KP, TI and TD are obtained

        key_plot = (thread_time_gl.elapsed())/1000.0;  // Time elapsed since the beginning of the test is obtained

        if(save_data==1){
            save_file();                           // Data is saved in specified csv file
        }

        realtimeplot();                            // Data is represented in main plot

        if(mode_change==1){
            if(mode_func==0){
                manual_mode();                     // Manual mode is set if required
            }
            else{
                auto_mode();                       // Auto mode is set if required
            }
            mode_change=0;
        }

        if(disconnect_var==1){
            disconnect_s();                        // If disconnection is requested, serial port disconnection function is called
        }
    }
}

// PLOTTING FUNCTION
void MainWindow::realtimeplot()
{
    double value_SP=0;
    double value_OP = gettextoOP.toDouble();
    double value_PV = textoPV.toDouble();
    if(mode_func==1){
        value_SP = gettextoSP.toDouble();
    }

    // New data is plotted
    ui->plot->graph(0)->addData(key_plot,value_OP);
    ui->plot->graph(1)->addData(key_plot,value_PV);
    if(mode_func==1){
        ui->plot->graph(2)->addData(key_plot,value_SP);
    }

    if(ui->plot->xAxis->range() != previousRange){
        range_update = 0;
    }

    //Dynamic plotting
    if((ui->plot->xAxis->range() == previousRange and range_update == 1) or reset_zoom_var == 1){
        if(key_plot >= x_axis_range) {
            ui->plot->xAxis->setRange(key_plot - x_axis_range, key_plot);
        } else {
            ui->plot->xAxis->setRange(0, x_axis_range);
        }
        reset_zoom_var = 0;
        range_update = 1;
    }

    // Update plot
    previousRange = ui->plot->xAxis->range();
    ui->plot->replot();
}

//CSV file saving function
void MainWindow::save_file(){
    QFile file(csv_save_file);

    if(mode_func==0){
        if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << gettextoOP << "," << textoPV << "," << "0.0" << "," << textoKP << "," << textoTi << "," << textoTd << "\n";
        }
        file.close();
    }
    else{
        if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << gettextoOP << "," << textoPV << "," << gettextoSP << "," << textoKP << "," << textoTi << "," << textoTd << "\n";
        }
        file.close();
    }
}

// DISCONNECT FUNCTION (CALLED IN MAIN COMMUNICATION THREAD)
void MainWindow::disconnect_s(){
    if (serial_p->isOpen()){
        serial_p->close();
        connection=0;
        ui->pushButton->setText("Connect");
        start_thread=1;
        save_data = 0;
        dataTimer.stop();
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->pushButton_4->setEnabled(true);
        ui->spinBox->setEnabled(false);
        ui->spinBox_2->setEnabled(false);
    }
}

/***************************************************************************
 **                                                                       **
 **                                                                       **
 **                       COMMUNICATION/MESSAGE FUNCTIONS                 **
 **                                                                       **
 **                                                                       **
 ***************************************************************************/


//MANUAL MODE FUNCTION
//Command "STX W 01 AM 1 ETX" is sent
void MainWindow::manual_mode(){

    QString basura = serial_p->readAll(); //Cleans serial port if necessary

    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x57);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x41);
    send_data.append(0x4D);
    send_data.append(0x31);
    send_data.append(0x03);

    serial_p->write(send_data);

    //Event loop configured to read incoming serial port data until the message is complete or a timer has ended.
    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);  // If the received message is not read correctly in 200 ms, read loop is skipped.

    // Serial port is read until the message is complete
    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<6 && !received_data.contains(static_cast<char>(0x06)));

    for(int i = 0; i<received_data.length(); i++){
        if(received_data[i] == static_cast<char>(0x06)){
            mensaje_entendido=1;
        }
    }

    int OP_1_int = get_value_OP().toDouble();
    ui->spinBox->setValue(OP_1_int);    // Current OP value is set in OP field box.

    if(start_thread==1){    // Communication thread is enabled (Each 500 ms)
        thread_time_gl.start();
        start_thread=0;
    }
    mode_func = 0;
}

//AUTO MODE FUNCTION
//Command "STX W 01 AM 0 ETX" is sent
void MainWindow::auto_mode(){

    QString basura = serial_p->readAll(); //Cleans serial port if necessary

    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x57);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x41);
    send_data.append(0x4D);
    send_data.append(0x30);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<6 && !received_data.contains(static_cast<char>(0x06)));

    for(int i = 0; i<received_data.length(); i++){
        if(received_data[i] == static_cast<char>(0x06)){
            mensaje_entendido=1;
        }
    }

    int SP_1_int = get_value_SP().toDouble();
    ui->spinBox_2->setValue(SP_1_int);

    mode_func = 1;
    if(start_thread==1){
        thread_time_gl.start();
        start_thread=0;
    }
}

// SP VALUE SET FUNCTION
void MainWindow::set_value_SP(QString SP){

    QString basura = serial_p->readAll();   //Cleans serial port if necessary
    int i;
    QByteArray sp_ba;
    QByteArray send_data;
    QByteArray received_data;

    sp_ba = SP.toUtf8();
    for (i=0; i<sp_ba.length(); i++){
        char aux = sp_ba[i];
        if(aux == 44){
            send_data[6 + i] = 0x2E;
        }
        else{
            send_data[6 + i] = aux;
        }
    }

    int k = i; 
    send_data[0] = 0x02;
    send_data[1] = 0x57;
    send_data[2] = 0x30;
    send_data[3] = 0x31;
    send_data[4] = 0x44;
    send_data[5] = 0x55;
    //send_data[6] = SP
    send_data[6 + k] = 0x03;

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<6 && !received_data.contains(static_cast<char>(0x06)));

    for(int i = 0; i<received_data.length(); i++){
        if(received_data[i] == static_cast<char>(0x06)){
            mensaje_entendido=1;
        }
    }
    mensaje_entendido=0;
    paramSP = false;
}

//SET OP VALUE FUNCTION
void MainWindow::set_value_OP(QString OP){

    QString basura = serial_p->readAll();   //Cleans serial port if necessary
    int i;
    QByteArray op_ba;
    QByteArray send_data;
    QByteArray received_data;

    op_ba = OP.toUtf8();

    for (i=0; i<op_ba.length(); i++){
        char aux = op_ba[i];
        if(aux == 44){
            send_data[6 + i] = 0x2E;
        }
        else{
            send_data[6 + i] = aux;
        }
    }

    int k = i;

    send_data[0] = 0x02;
    send_data[1] = 0x57;
    send_data[2] = 0x30;
    send_data[3] = 0x31;
    send_data[4] = 0x4F;
    send_data[5] = 0x50;
    //send_data[6] = OP
    send_data[6 + k] = 0x03;

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15))) // Si el temporizador ha expirado, sal del bucle
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<6 && !received_data.contains(static_cast<char>(0x06)));

    for(int i = 0; i<received_data.length(); i++){
        if(received_data[i] == static_cast<char>(0x06)){
            mensaje_entendido=1;
        }
    }

    mensaje_entendido=0;
    paramOP = false;
}

QString MainWindow::get_value_OP(){

    int i;
    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x4F);
    send_data.append(0x50);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15))) // Si el temporizador ha expirado, sal del bucle
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));

    textoOPaux = "";

    int j = 0;
    for(i = 0; i < received_data.size(); i++){
        if(received_data[i] == static_cast<char>(0x2D) || received_data[i] == static_cast<char>(0x2E) || (received_data[i] >= static_cast<char>(0x30) && received_data[i] <= static_cast<char>(0x39))){
            j++;
            if(j > 2){
                textoOPaux = textoOPaux + received_data[i];
            }
        }
    }
    return textoOPaux;
}

void MainWindow::get_value_PV(){

    int i;
    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x4D);
    send_data.append(0x56);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));

    textoPV = "";

    int j = 0;
    for(i = 0; i < received_data.size(); i++){
        if(received_data[i] == static_cast<char>(0x2D) || received_data[i] == static_cast<char>(0x2E) || (received_data[i] >= static_cast<char>(0x30) && received_data[i] <= static_cast<char>(0x39))){
            j++;
            if(j > 2){
                textoPV = textoPV + received_data[i];
            }
        }
    }
}

// GET SP VALUE FUNCTION
QString MainWindow::get_value_SP(){

    int i;
    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x53);
    send_data.append(0x50);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));
    textoSPaux = "";

    int j = 0;
    for(i = 0; i < received_data.size(); i++){
        if(received_data[i] == static_cast<char>(0x2D) || received_data[i] == static_cast<char>(0x2E) || (received_data[i] >= static_cast<char>(0x30) && received_data[i] <= static_cast<char>(0x39))){
            j++;
            if(j > 2){
                textoSPaux = textoSPaux + received_data[i];
            }
        }
    }
    return textoSPaux;
}

QString MainWindow::get_value_KP()
{
    int i;
    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x50);
    send_data.append(0x42);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));

    textoKPaux = "";

    int j = 0;
    for(i = 0; i < received_data.size(); i++){
        if(received_data[i] == static_cast<char>(0x2D) || received_data[i] == static_cast<char>(0x2E) || (received_data[i] >= static_cast<char>(0x30) && received_data[i] <= static_cast<char>(0x39))){
            j++;
            if(j > 2){
                textoKPaux = textoKPaux + received_data[i];
            }
        }
    }
    return textoKPaux;
}

QString MainWindow::get_value_Ti()
{
    int i;
    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x49);
    send_data.append(0x54);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));

    textoTiaux = "";

    int j = 0;
    for(i = 0; i < received_data.size(); i++){
        if(received_data[i] == static_cast<char>(0x2D) || received_data[i] == static_cast<char>(0x2E) || (received_data[i] >= static_cast<char>(0x30) && received_data[i] <= static_cast<char>(0x39))){
            j++;
            if(j > 2){
                textoTiaux = textoTiaux + received_data[i];
            }
        }
    }
    return textoTiaux;
}

QString MainWindow::get_value_Td()
{
    int i;
    QByteArray send_data;
    QByteArray received_data;

    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x44);
    send_data.append(0x54);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));

    textoTdaux = "";

    int j = 0;
    for(i = 0; i < received_data.size(); i++){
        if(received_data[i] == static_cast<char>(0x2D) || received_data[i] == static_cast<char>(0x2E) || (received_data[i] >= static_cast<char>(0x30) && received_data[i] <= static_cast<char>(0x39))){
            j++;
            if(j > 2){
                textoTdaux = textoTdaux + received_data[i];
            }
        }
    }
    return textoTdaux;
}

// CHECK CURRENT OPERATING MODE: STX R 01 AM ETX
//ANSWER: 01AM0 ACK or 01AM1 ACK
bool MainWindow::get_func_mode()
{
    int i;
    QByteArray send_data;
    QByteArray received_data;
    send_data.append(0x02);
    send_data.append(0x52);
    send_data.append(0x30);
    send_data.append(0x31);
    send_data.append(0x41);
    send_data.append(0x4D);
    send_data.append(0x03);

    serial_p->write(send_data);

    QTimer timer_loop;
    timer_loop.setSingleShot(true);

    QEventLoop loop;
    QObject::connect(serial_p, &QSerialPort::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer_loop, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer_loop.start(200);

    do {
        if (!timer_loop.isActive() or received_data.contains(static_cast<char>(0x06)) or received_data.contains(static_cast<char>(0x15)))
            break;
        loop.exec();
        received_data.append(serial_p->readAll());
    } while(received_data.size()<11 && !received_data.contains(static_cast<char>(0x06)));

    if(received_data[4] == static_cast<char>(0x30)){
        return 1;
    }
    else if (received_data[4] == static_cast<char>(0x31)){
        return 0;
    }
    else{
        return 1;
    }
}


/***************************************************************************
 **                                                                       **
 **                                                                       **
 **                       GUI ELEMENTS FUNCTIONS                          **
 **                                                                       **
 **                                                                       **
 ***************************************************************************/


// WHEN RETURN IS PRESSED AFTER INTRODUCING A CSV FILE NAME
void MainWindow::on_lineEdit_3_returnPressed()
{
    csv_save_file.clear();  // Previous chosen directory and file is unselected
    QString filename_csv = ui->lineEdit_3->text();
    if((filename_csv == "") or (filename_csv == " ")){
        QMessageBox::information(this, "Warning", tr("File Test_300.csv saved in chosen folder. If no folder has been specified, csv file is located in Dektop")); // If no name is specified, a warning window is displayed
        csv_save_file.append(csv_dir + "/" + "Test_300.csv");
        save_data = 1;
        ui->groupBox_3->setTitle("Save text file: RECORDING");
        return;
    }
    else{
        csv_save_file.append(csv_dir + "/" + filename_csv + ".csv");    // Complete folder route + file name is configured
    }
    save_data = 1;
    ui->groupBox_3->setTitle("Save text file: RECORDING");
}

// CONNECT / DISCONNECT BUTTON
// If "Connect" button is pressed, GUI is configured, graph is cleared, serial port connection is established and checks the current operation mode of thr PID
// When this button is pressed while connected ("Disconnect"), the disconnection is done in the main communication thread avoiding unintentional errors.

void MainWindow::on_pushButton_clicked()
{
    if(connection==0){
        serial_p->setPortName(ui->comboBox->currentText());
        if (serial_p->open(QIODevice::ReadWrite)){
            if(disconnect_var == 1){
                ui->plot->graph(0)->data()->clear();
                ui->plot->graph(1)->data()->clear();
                ui->plot->graph(2)->data()->clear();
                ui->plot->xAxis->setRange(0, 300);
                ui->plot->xAxis->setLabel("Seconds");
                ui->plot->replot();
            }
            ui->pushButton->setText("Disconnect");
            ui->pushButton_4->setEnabled(false);
            ui->pushButton_5->setEnabled(false);
            disconnect_var = 0;
            connection=1;
            ui->label_12->setText("");
            ui->label_13->setText("");
            ui->label_15->setText("");

            bool func_mode = get_func_mode();
            if(func_mode == 1){
                on_pushButton_3_clicked();
            }
            else{
                on_pushButton_2_clicked();
            }

            get_parameters();
            key_plot=0.0;
        }
        else{
            QMessageBox::information(this, "Warning", tr("Connection via serial port with Commander 300 not succesful"));
            return;
        }
    }
    else{
        if (serial_p->isOpen()){
            if(!dataTimer.isActive()){
                disconnect_s();
                ui->groupBox_3->setTitle("Save text file:");
            }
            disconnect_var=1;
        }
        ui->pushButton_5->setEnabled(true);
    }
}

// MANUAL BUTTON PRESSED
void MainWindow::on_pushButton_2_clicked()
{
    mode_func = 0;

    ui->label_2->setEnabled(true);
    ui->label_3->setEnabled(false);
    ui->spinBox->setEnabled(true);
    ui->spinBox_2->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(true);
    ui->label_10->setVisible(false);
    ui->label_11->setVisible(false);
    ui->plot->graph(2)->data()->clear();
    ui->plot->graph(2)->setVisible(false);
    ui->plot->replot();

    if(dataTimer.isActive()){
        mode_change=1;
    }
    else{
        manual_mode();
        connect(&dataTimer, SIGNAL(timeout()), this, SLOT(timerThread()));
        dataTimer.start(500);
        mode_change=0;
    }
}

// AUTO BUTTON PRESSED
void MainWindow::on_pushButton_3_clicked()
{
    mode_func = 1;

    ui->label_2->setEnabled(false);
    ui->label_3->setEnabled(true);
    ui->spinBox->setEnabled(false);
    ui->spinBox_2->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(false);
    ui->plot->graph(2)->setVisible(true);
    ui->label_10->setVisible(true);
    ui->label_11->setVisible(true);

    if(dataTimer.isActive()){
        mode_change=1;
    }
    else{
        auto_mode();
        connect(&dataTimer, SIGNAL(timeout()), this, SLOT(timerThread()));
        dataTimer.start(500);
        mode_change=0;
    }
}

// CSV FOLDER SPECIFICATION BUTTON
void MainWindow::on_pushButton_4_clicked()
{
    csv_dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),                                                   "/home",
                                                QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
}

// HELP BUTTON
void MainWindow::on_pushButton_5_clicked()
{
    Dialog_about dialog;
    dialog.setModal(true);
    dialog.exec();
}

// EXIT BUTTON
void MainWindow::on_pushButton_6_clicked()
{
    //Serial port is disconnected before exiting
    if(serial_p->isOpen()){
        serial_p->close();
        connection=0;
    }
    this->close();
}

// SELECTED SERIAL PORT COMBO BOX
void MainWindow::on_comboBox_activated(int index)
{

}

// Function called to obtain Kp, Ti and Td parameters
void MainWindow::get_parameters(){
    textoKP = get_value_KP();
    textoTi = get_value_Ti();
    textoTd = get_value_Td();
    ui->label_12->setText(textoKP);
    ui->label_13->setText(textoTi);
    ui->label_15->setText(textoTd);
}

// Function linked with OP set field box. When return is pressed OR THE FIELD IS CLICKED AND DESELECTED, a request is sent to change the OP value in the communication thread
// IMPORTANT: RETURN KEY IS NOT THE ONLY OPTION TO CHANGE OP VALUE. IF THE BOX IS CLICKED AND DESELECTED THE REQUEST IS ALSO SENT
void MainWindow::on_spinBox_editingFinished()
{
    textoOP = ui->spinBox->text();
    paramOP = true;
    updateParam = true;
}

// Function linked with SP set field box. When return is pressed OR THE FIELD IS CLICKED AND DESELECTED, a request is sent to change the OP value in the communication thread
// IMPORTANT: RETURN KEY IS NOT THE ONLY OPTION TO CHANGE OP VALUE. IF THE BOX IS CLICKED AND DESELECTED THE REQUEST IS ALSO SENT
void MainWindow::on_spinBox_2_editingFinished()
{
    textoSP = ui->spinBox_2->text();
    paramSP = true;
    updateParam = true;
}

//When "Reset zoom" button is pressed, plot range is automatically updated
void MainWindow::on_pushButton_7_clicked()
{
    reset_zoom_var = 1;
}

