// Copyright (c) 2018 The Curium developers
// Copyright (c) 2018-2018 The Phore developers
// Copyright (c) 2019 The ODIN developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/odin-config.h"
#endif

#include "configuremasternodepage.h"
#include "ui_configuremasternodepage.h"

#include "activemasternode.h"
#include "bitcoingui.h"
#include "csvmodelwriter.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "masternode-budget.h"
#include "masternode-payments.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "masternodelist.h"
#include "wallet.h"
#include <univalue.h>

#include <QIcon>
#include <QMenu>
#include <QString>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <string>

ConfigureMasternodePage::ConfigureMasternodePage(Mode mode, QWidget* parent) : QDialog(parent),
                                                                   ui(new Ui::ConfigureMasternodePage),
                                                                   mapper(0),
                                                                   mode(mode)
{
    ui->setupUi(this);

    ui->aliasEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->vpsIpEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->privKeyEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->outputEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->outputIdEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);

	GUIUtil::setupAliasWidget(ui->aliasEdit, this);
	GUIUtil::setupIPWidget(ui->vpsIpEdit, this);
	GUIUtil::setupPrivKeyWidget(ui->privKeyEdit, this);
	GUIUtil::setupTXIDWidget(ui->outputEdit, this);
	GUIUtil::setupTXIDIndexWidget(ui->outputIdEdit, this);

    switch (mode) {
        case NewConfigureMasternode:
            setWindowTitle(tr("New Masternode Alias"));
            break;
        case EditConfigureMasternode:
            setWindowTitle(tr("Edit Masternode Alias"));
            break;
    }

}

ConfigureMasternodePage::~ConfigureMasternodePage()
{
    delete ui;
}

void ConfigureMasternodePage::counter(int counter)
{
   setCounters(counter);
}

void ConfigureMasternodePage::MNAliasCache(QString MnAliasCache)
{
   setMnAliasCache(MnAliasCache);
}

void ConfigureMasternodePage::loadAlias(QString strAlias)
{
   ui->aliasEdit->setText(strAlias);
}

void ConfigureMasternodePage::loadIP(QString strIP)
{
   ui->vpsIpEdit->setText(strIP);
}

void ConfigureMasternodePage::loadPrivKey(QString strPrivKey)
{
   ui->privKeyEdit->setText(strPrivKey);
}

void ConfigureMasternodePage::loadTxHash(QString strTxHash)
{
   ui->outputEdit->setText(strTxHash);
}

void ConfigureMasternodePage::loadOutputIndex(QString strOutputIndex)
{
   ui->outputIdEdit->setText(strOutputIndex);
}

bool ConfigureMasternodePage::validateMasternode()
{
    std::string strError = "";

    // RegEx for Masternode IP Address validation
    QRegExp ipRx("^(?:[0-9.]+|(?:\\[[0-9a-fA-F:]+\\]))(:[0-9]+)$");

    // Validation for Masternode Private Key
    CBitcoinSecret privKeyStr;
    bool privKeyGood = privKeyStr.SetString(ui->privKeyEdit->text().toStdString().c_str());

    if (ui->aliasEdit->text().toStdString().empty()) {
        strError = "Masternode Alias cannot be empty.";
    } else if (ui->vpsIpEdit->text().toStdString().empty()) {
        strError = "Masternode IP Address cannot be empty.";
    } else if (!ui->vpsIpEdit->text().contains(ipRx)) {
        strError = "Masternode IP Address is invalid."
            "\nIt should be either an IPv4 or IPv6 address and include the port."
            "\nIPv4 Example: 123.123.123.123:22100"
            "\nIPv6 Example: [2a01:4f8:c2c:5b9c:e513::6]:22100";
    } else if (ui->privKeyEdit->text().toStdString().empty()) {
        strError = "Private Key cannot be empty.";
    } else if (!privKeyGood) {
        strError = "Private Key is invalid."
            "\nTo generate a valid Masternode private key click 'Autofill Private Key' below "
            "\nor enter 'masternode genkey' into the Debug Console.";
    } else if (ui->outputEdit->text().toStdString().size() < 64) {
        strError = "Invalid Output TX, it should look like:"
            "\n(3bcf71c20dcbac71f43e06ab4b43f7dcb6f32b45f2e0de641bbe0f3989d74ba8)";
    } else if (ui->outputIdEdit->text().toStdString().empty()) {
        strError = "Output ID cannot be empty.";
    }

    if (!strError.empty())
    {
        QMessageBox::critical(this, tr("Masternode Save Error"), QString::fromStdString(strError));
        return false;
    }

    return true;
}

void ConfigureMasternodePage::saveCurrentRow()
{
    if (!validateMasternode()) return;

    switch (mode) {
        case NewConfigureMasternode:
            if(ui->aliasEdit->text().toStdString().empty() ||
                ui->vpsIpEdit->text().toStdString().empty() ||
                ui->privKeyEdit->text().toStdString().empty() ||
                ui->outputEdit->text().toStdString().empty() ||
                ui->outputIdEdit->text().toStdString().empty()) {
                break;
            }

            masternodeConfig.add(
                ui->aliasEdit->text().toStdString(),
                ui->vpsIpEdit->text().toStdString(),
                ui->privKeyEdit->text().toStdString(),
                ui->outputEdit->text().toStdString(),
                ui->outputIdEdit->text().toStdString());

            masternodeConfig.writeToMasternodeConf();
            break;
        case EditConfigureMasternode:
            if(ui->aliasEdit->text().toStdString().empty() ||
                ui->vpsIpEdit->text().toStdString().empty() ||
                ui->privKeyEdit->text().toStdString().empty() ||
                ui->outputEdit->text().toStdString().empty() ||
                ui->outputIdEdit->text().toStdString().empty()) {
                break;
            }

            QString MnAlias = getMnAliasCache();
            ConfigureMasternodePage::updateAlias(ui->aliasEdit->text().toStdString(),
                ui->vpsIpEdit->text().toStdString(),
                ui->privKeyEdit->text().toStdString(),
                ui->outputEdit->text().toStdString(),
                ui->outputIdEdit->text().toStdString(),
                MnAlias.toStdString());
            break;
    }
}

void ConfigureMasternodePage::accept()
{
    if (!validateMasternode()) return;

	saveCurrentRow();
	emit accepted();
    QDialog::accept();
}


void ConfigureMasternodePage::updateAlias(std::string Alias, std::string IP, std::string PrivKey, std::string TxHash, std::string OutputIndex, std::string mnAlias)
{
	BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
		if(mnAlias == mne.getAlias()) {
			int count = 0;
			count = getCounters();
			vector<COutPoint> confLockedCoins;
			uint256 mnTxHash;
			mnTxHash.SetHex(mne.getTxHash());
			int nIndex;
			if(!mne.castOutputIndex(nIndex))
				continue;
			COutPoint outpoint = COutPoint(mnTxHash, nIndex);
			confLockedCoins.push_back(outpoint);
			pwalletMain->UnlockCoin(outpoint);

			masternodeConfig.deleteAlias(count);
			masternodeConfig.add(Alias, IP, PrivKey, TxHash, OutputIndex);
			// write to masternode.conf
			masternodeConfig.writeToMasternodeConf();
		}
	}

}

void ConfigureMasternodePage::on_AutoFillPrivKey_clicked()
{
    CKey secret;
    secret.MakeNewKey(false);

	ui->privKeyEdit->setText(QString::fromStdString(CBitcoinSecret(secret).ToString()));
}


void ConfigureMasternodePage::on_AutoFillOutputs_clicked()
{
    // Find possible candidates
    vector<COutput> possibleCoins = activeMasternode.SelectCoinsMasternode();
        int test = 0;
    BOOST_FOREACH (COutput& out, possibleCoins) {
        std::string TXHash = out.tx->GetHash().ToString();
        std::string OutputID = std::to_string(out.i);
                BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
                        if(OutputID == mne.getOutputIndex() && TXHash == mne.getTxHash()) {
                                test = 1;

                        }
                }

                if(test == 0) {
                        ui->outputEdit->setText(QString::fromStdString(out.tx->GetHash().ToString()));
                        ui->outputIdEdit->setText(QString::fromStdString(std::to_string(out.i)));

                        break;
                }
                test = 0;
    }
}

