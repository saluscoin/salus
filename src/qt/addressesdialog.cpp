#include <util.h>
#include "addressesdialog.h"
#include "ui_addressesdialog.h"
#include "walletmodel.h"
#include "mnemonic/mnemonicwalletinit.h"

#include <QMessageBox>

AddressesDialog::AddressesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressesDialog)
{
    ui->setupUi(this);
    connect(ui->listwidgetSeeds, &QListWidget::itemSelectionChanged, this, &AddressesDialog::SeedSelectionChanged);
}

AddressesDialog::~AddressesDialog()
{
    delete ui;
}

void AddressesDialog::ResetSelections()
{
    if (!m_wallet->IsHdWallet()) {
        //HD wallet is not yet enabled
        QMessageBox box;
        box.setText("This wallet has not yet been upgraded to an HD Wallet. To upgrade use the Create New Seed Button.");
        box.exec();
        return;
    }

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
 * @brief AddressesDialog::SeedSelectionChanged The selection in the seeds list widget has changed. Update the current selected seed
 * and the views.
 */
void AddressesDialog::SeedSelectionChanged()
{
    QString qstrSelection = ui->listwidgetSeeds->currentItem()->text();
    uint256 hashSelection = uint256S(qstrSelection.toStdString());
    if (m_wallet->GetSeedIds().count(hashSelection)) {
        m_hashSeedSelected = hashSelection;
    }

    PopulateAccountsView();
    PopulateAddressesView();
}

/**
 * @brief AddressesDialog::PopulateSeedsView Get a list of seed id's from the wallet and populate the list view with the available seeds
 */
void AddressesDialog::PopulateSeedsView()
{
    std::set<uint256> setSeedId = m_wallet->GetSeedIds();
    ui->listwidgetSeeds->clear();

    //If the wallet does not currently have an HD seed, then do not add anything
    if (!m_wallet->IsHdWallet())
        return;

    for (const uint256& id : setSeedId) {
        QListWidgetItem* item = new QListWidgetItem(id.GetHex().c_str());

        ui->listwidgetSeeds->addItem(item);

        if (id == m_hashSeedSelected) {
            ui->listwidgetSeeds->blockSignals(true);
            ui->listwidgetSeeds->setCurrentItem(item);
            ui->listwidgetSeeds->blockSignals(false);
        }
    }
}

/**
 * @brief AddressesDialog::PopulateAccountsView Populate the list of accounts with any accounts that belong to the selected seed.
 */
void AddressesDialog::PopulateAccountsView()
{
    ui->listwidgetAccounts->clear();
    if (m_hashSeedSelected == 0 || !m_wallet->IsHdWallet())
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
    if (!m_wallet->IsHdWallet())
        return;

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

void AddressesDialog::on_buttonAddSeed_clicked()
{
    //If this is a new wallet, then create the seed words and import them.
    MnemonicWalletInit walletInit;
    bool fNewSeed = false;
    if (!walletInit.Open(fNewSeed)) {
        QMessageBox box;
        box.setText("Failed to add new seed.");
        box.exec();
    } else {
        ResetSelections();
    }
}
