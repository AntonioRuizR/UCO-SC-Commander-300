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

#ifndef DIALOG_ABOUT_H
#define DIALOG_ABOUT_H

#include <QDialog>

namespace Ui {
class Dialog_about;
}

class Dialog_about : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_about(QWidget *parent = nullptr);
    ~Dialog_about();

private:
    Ui::Dialog_about *ui;
};

#endif // DIALOG_ABOUT_H
