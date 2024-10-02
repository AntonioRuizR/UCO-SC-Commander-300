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

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
