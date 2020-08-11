#include "addressesdialog.h"
#include "ui_addressesdialog.h"
#include <wallet.h>

AddressesDialog::AddressesDialog(CWallet* pwallet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressesDialog)
{
    ui->setupUi(this);
    m_wallet = pwallet;
}

AddressesDialog::~AddressesDialog()
{
    delete ui;
}

/**
 * @brief AddressesDialog::PopulateSeedsView Get a list of seed id's from the wallet and populate the list view with the available seeds
 */
void AddressesDialog::PopulateSeedsView()
{
    std::set<uint256> setSeedId = m_wallet->GetSeedIds();
    ui->listwidgetSeeds->clear();
    for (const uint256& id : setSeedId) {
        ui->listwidgetSeeds->addItem(id.GetHex().c_str());
    }
}

/**
 * @brief AddressesDialog::PopulateAccountsView Populate the list of accounts with any accounts that belong to the selected seed.
 */
void AddressesDialog::PopulateAccountsView()
{
    ui->listwidgetAccounts->clear();
    if (m_hashSeedSelected == 0)
        return;

    //Currently the only account that is allowed to be used is account 0, so just populate with that.
    ui->listwidgetAccounts->addItem("0");
    m_nAccountSelected = 0;
}

/**
 * @brief AddressesDialog::PopulateAddressesView Display all addresses that belong to the selected account.
 */
void AddressesDialog::PopulateAddressesView()
{
    std::vector<std::pair<CKeyID, std::string>> vecAddresses = m_wallet->GetAccountAddresses(m_hashSeedSelected, m_nAccountSelected);
    ui->listwidgetAddresses->clear();
    for (const auto& pair : vecAddresses) {
        ui->listwidgetAddresses->addItem(pair.second.c_str());
    }
}
