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

#include "dialog_about.h"
#include "ui_dialog_about.h"

Dialog_about::Dialog_about(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog_about)
{
    ui->setupUi(this);

    setWindowTitle(tr("About:"));

    QPixmap logo_uco(":/images/uco.png");
    QPixmap logo_epsc(":/images/epsc.png");
    QPixmap logo_prinia(":/images/prinia_logo.jpg");
    int width_logo_uco = ui->label_logouco->width();
    int height_logo_uco = ui->label_logouco->height();
    int width_logo_epsc = ui->label_logoepsc->width();
    int height_logo_epsc = ui->label_logoepsc->height();
    int width_logo_prinia = ui->label_5->width();
    int height_logo_prinia = ui->label_5->height();
    ui->label_logouco->setPixmap(logo_uco.scaled(width_logo_uco,height_logo_uco,Qt::KeepAspectRatio));
    ui->label_logoepsc->setPixmap(logo_epsc.scaled(width_logo_epsc,height_logo_epsc,Qt::KeepAspectRatio));
    ui->label_5->setPixmap(logo_prinia.scaled(width_logo_prinia,height_logo_prinia,Qt::KeepAspectRatio));

    ui->label_3->setText("<a href=\"https://www.researchgate.net/profile/Mario-Ruz-2\">Mario L. Ruz Ruiz</a>");
    ui->label_3->setOpenExternalLinks(true);

    ui->label_2->setText("<a href=\"https://www.researchgate.net/profile/Antonio-Ruiz-Ruiz-3\">Antonio Ruiz Ruiz</a>");
    ui->label_2->setOpenExternalLinks(true);

    ui->label_4->setText("<a href=\"https://www.researchgate.net/profile/Juan-Garrido-26\">Juan Garrido Jurado</a>");
    ui->label_4->setOpenExternalLinks(true);

    ui->label_6->setText("<a href=\"http://www.uco.es/grupos/prinia/fvazquez\">Francisco Vázquez Serrano</a>");
    ui->label_6->setOpenExternalLinks(true);

    ui->label_7->setText("<a href=\"https://github.com/AntonioRuizR/UCO-SC-Commander-300\">Source code avaliable in GitHub</a>");
    ui->label_7->setOpenExternalLinks(true);

    //ui->label->setText(tr("Application developed at the EPSC of the University of Cordoba (Spain)."));
    //ui->label->setOpenExternalLinks(true);
}

Dialog_about::~Dialog_about()
{
    delete ui;
}
