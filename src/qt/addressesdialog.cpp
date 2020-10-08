#include <util.h>
#include "addressesdialog.h"
#include "ui_addressesdialog.h"
#include "walletmodel.h"

AddressesDialog::AddressesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressesDialog)
{
    ui->setupUi(this);
}

AddressesDialog::~AddressesDialog()
{
    delete ui;
}

void AddressesDialog::ResetSelections()
{
    //Set selected seed to the main seed
    m_hashSeedSelected = m_wallet->GetActiveSeedId();

    PopulateSeedsView();
    PopulateAccountsView();
    PopulateAddressesView();
}

void AddressesDialog::SetWalletModel(WalletModel *model)
{
    m_wallet = model;
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
    std::map<std::string, std::pair<CKeyID, std::string>> mapAddresses = m_wallet->GetAccountAddresses(m_hashSeedSelected, m_nAccountSelected);
    ui->listwidgetAddresses->clear();
    std::map<int, QString> mapAddressSorted;
    for (const auto& pair : mapAddresses) {
        const auto& strAddress = pair.second.second;
        QStringList ql = QString(pair.first.c_str()).split("/");
        mapAddressSorted.emplace(ql.back().toInt(), strAddress.c_str());
    }

    for (const auto& pair : mapAddressSorted) {
        ui->listwidgetAddresses->addItem(pair.second);
    }
}

void AddressesDialog::on_buttonNewAccount_clicked()
{

}

void AddressesDialog::on_buttonNewAddress_clicked()
{
    std::string strAddress;
    m_wallet->GenerateNewAddress(m_nAccountSelected, strAddress);
    PopulateAddressesView();
}
