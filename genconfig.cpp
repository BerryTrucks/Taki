/* $Id: genconfig.cpp */
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

#include "genconfig.h"
#include "ui_genconfig.h"
#include <QFileDialog>
#include <QFileInfo>

GenConfig::GenConfig(GeneralSettings *genSet, const int tabIndex,
                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenConfig),
#if defined(Q_OS_QNX)
    codecsScroller(new QsKineticScroller(this)),
#endif
    genSettings(genSet)
{
    ui->setupUi(this);
    ui->cbCodecAlaw->setCurrentIndex(genSettings->codecPriorG711A);
    ui->cbCodecUlaw->setCurrentIndex(genSettings->codecPriorG711U);
    ui->cbCodecGsm->setCurrentIndex(genSettings->codecPriorGsm);
    ui->cbCodecG722->setCurrentIndex(genSettings->codecPriorG722);
    ui->cbCodecIlbc->setCurrentIndex(genSettings->codecPriorIlbc);
    ui->cbCodecSpeexNb->setCurrentIndex(genSettings->codecPriorSpeexNb);
    ui->cbCodecSpeexUwb->setCurrentIndex(genSettings->codecPriorSpeexUwb);
    ui->cbCodecSpeexWb->setCurrentIndex(genSettings->codecPriorSpeexWb);
    ui->leStunServer->setText(genSettings->stunServer);
    ui->chkbUseStun->setChecked(genSettings->useStunServer);
    ui->sbPort->setValue(genSettings->port);
    ui->chkbTCP->setChecked(genSettings->protoTCP);
    ui->chkbTLS->setChecked(genSettings->protoTLS);
    ui->chkbUDP->setChecked(genSettings->protoUDP);
    ui->chkbCompForm->setChecked(genSettings->compactFormHeaders);
    ui->sbCallHistoryMaxCalls->setValue(genSettings->callHistoryMaxCalls);
    ui->cbNetPref->setCurrentIndex(genSettings->requiredNetworkInterface);
    ui->chkbHeadless->setChecked(genSettings->runHeadless);
    ui->cbDisplayContOrder->setCurrentIndex(genSettings->displayContOrder);
    ui->cbDefaultTab->setCurrentIndex(genSettings->defaultTab);
    ui->cbVerifyServer->setChecked(genSettings->tlsVerifyServer);
    ui->cbUseSystemCA->setChecked(genSettings->tlsUseSystemCA);
    ui->cbUserBuiltinCA->setChecked(genSettings->tlsUseBuiltinCA);
    ui->leCAListFile->setText(genSettings->caFile);
    ui->cbVerifyClient->setChecked(genSettings->tlsVerifyClient);
    ui->lePublicCertFile->setText(genSettings->certFile);
    ui->lePrivateKeyFile->setText(genSettings->privKeyFile);
    ui->lePrivateKeyPassword->setText(genSettings->privKeyPassword);

#if defined(Q_OS_QNX)
    ui->frDesktopControls->hide();
    ui->frCaListFile->hide();
    ui->grTLSMutualAuth->hide();
#else
    ui->frBBControls->hide();
#endif
    if (tabIndex > 0 && tabIndex < ui->listWidget->count()) {
        ui->listWidget->setCurrentItem(ui->listWidget->item(tabIndex));
    }
#if defined(Q_OS_QNX)
    codecsScroller->enableKineticScrollFor(ui->saCodecs);
#endif
    // Hide not yet implemented controls
    ui->chkbHeadless->hide();
}

GenConfig::~GenConfig()
{
    delete ui;
}

void GenConfig::getResult()
{
    genSettings->codecPriorG711A = ui->cbCodecAlaw->currentIndex();
    genSettings->codecPriorG711U = ui->cbCodecUlaw->currentIndex();
    genSettings->codecPriorGsm = ui->cbCodecGsm->currentIndex();
    genSettings->codecPriorG722 = ui->cbCodecG722->currentIndex();
    genSettings->codecPriorIlbc = ui->cbCodecIlbc->currentIndex();
    genSettings->codecPriorSpeexNb = ui->cbCodecSpeexNb->currentIndex();
    genSettings->codecPriorSpeexUwb = ui->cbCodecSpeexUwb->currentIndex();
    genSettings->codecPriorSpeexWb = ui->cbCodecSpeexWb->currentIndex();
    genSettings->useStunServer = ui->chkbUseStun->checkState();
    genSettings->stunServer = ui->leStunServer->text();
    genSettings->port = ui->sbPort->value();
    genSettings->protoTCP = ui->chkbTCP->checkState();
    genSettings->protoTLS = ui->chkbTLS->checkState();
    genSettings->protoUDP = ui->chkbUDP->checkState();
    genSettings->compactFormHeaders = ui->chkbCompForm->checkState();
    genSettings->callHistoryMaxCalls = ui->sbCallHistoryMaxCalls->value();
    genSettings->requiredNetworkInterface = ui->cbNetPref->currentIndex();
    genSettings->runHeadless = ui->chkbHeadless->checkState();
    genSettings->displayContOrder = ui->cbDisplayContOrder->currentIndex();
    genSettings->defaultTab = ui->cbDefaultTab->currentIndex();
    genSettings->tlsVerifyServer = ui->cbVerifyServer->checkState();
    genSettings->tlsUseSystemCA = ui->cbUseSystemCA->checkState();
    genSettings->tlsUseBuiltinCA = ui->cbUserBuiltinCA->checkState();
    genSettings->caFile = ui->leCAListFile->text();
    genSettings->tlsVerifyClient = ui->cbVerifyClient->checkState();
    genSettings->certFile = ui->lePublicCertFile->text();
    genSettings->privKeyFile = ui->lePrivateKeyFile->text();
    genSettings->privKeyPassword = ui->lePrivateKeyPassword->text();
}

void GenConfig::onPbBrowseCAListFile()
{
    QString dir;
    if (!genSettings->caFile.isEmpty()) {
        dir = QFileInfo(genSettings->caFile).dir().path();
    }
    QString file1Name = QFileDialog::getOpenFileName(
                this, tr("Certificate of Authority (CA) list file"),
                dir, tr("Certificate files *.pem *.crt (*.pem *.crt)"));
    if (!file1Name.isNull()) {
        ui->leCAListFile->setText(file1Name);
    }
}

void GenConfig::onPbBrowsePublicCertFile()
{
    QString dir;
    if (!genSettings->certFile.isEmpty()) {
        dir = QFileInfo(genSettings->certFile).dir().path();
    }
    QString file1Name = QFileDialog::getOpenFileName(
                this, tr("Public endpoint certificate file"),
                dir, tr("Certificate files *.pem *.crt (*.pem *.crt)"));
    if (!file1Name.isNull()) {
        ui->lePublicCertFile->setText(file1Name);
    }
}

void GenConfig::onPbBrowsePrivateKeyFile()
{
    QString dir;
    if (!genSettings->privKeyFile.isEmpty()) {
        dir = QFileInfo(genSettings->privKeyFile).dir().path();
    }
    QString file1Name = QFileDialog::getOpenFileName(
                this, tr("Private key file of the endpoint certificate"),
                dir, tr("Key files *.key (*.key)"));
    if (!file1Name.isNull()) {
        ui->lePrivateKeyFile->setText(file1Name);
    }
}
