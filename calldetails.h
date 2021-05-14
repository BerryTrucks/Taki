/* $Id: calldetails.h */
/*
 * Copyright (C) 2012-2014 Dmytro Mishchenko <arkadiamail@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef CALLDETAILS_H
#define CALLDETAILS_H

#include "cdr.h"
#include <QDialog>

namespace Ui {
class CallDetails;
}

class CallDetails : public QDialog
{
    Q_OBJECT
    
public:
    explicit CallDetails(const Cdr &cdr, QWidget *parent = 0);
    ~CallDetails();

private slots:
    void startPlayback();
    void stopPlayback();
    void on_btCallDetailsDelete_clicked();
    void on_btCallDetailsAddContact_clicked();

signals:
    void delCallDetail();
    void addCallDetailToContact();
    void startWavPlayback(const QString &fn);
    void stopWavPlayback();

private:
    Ui::CallDetails *ui;
    Cdr m_cdr;
};

#endif // CALLDETAILS_H
