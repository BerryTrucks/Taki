/* $Id: genconfig.h */
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

#ifndef GENCONFIG_H
#define GENCONFIG_H

#include "generalsettings.h"
#if defined(Q_OS_QNX)
#include "QsKineticScroller.h"
#endif
#include <QDialog>

namespace Ui {
    class GenConfig;
}

class GenConfig : public QDialog
{
    Q_OBJECT

public:
    enum SettingsSections {
        Platform = 0,
        Transport,
        Media,
        STUN,
        Contacts,
        History,
        Security
    };

    explicit GenConfig(GeneralSettings *genSettings, const int tabIndex = 0,
                       QWidget *parent = 0);
    ~GenConfig();
    void getResult();

private:
    Ui::GenConfig *ui;
#if defined(Q_OS_QNX)
    QsKineticScroller *codecsScroller;
#endif
    GeneralSettings *genSettings;

private slots:
    void onPbBrowseCAListFile();
    void onPbBrowsePublicCertFile();
    void onPbBrowsePrivateKeyFile();
};

#endif // GENCONFIG_H
