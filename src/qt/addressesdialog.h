#ifndef ADDRESSESDIALOG_H
#define ADDRESSESDIALOG_H

#include <QDialog>
#include <uint256.h>

namespace Ui {
class AddressesDialog;
}

class WalletModel;

class AddressesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddressesDialog(QWidget *parent = nullptr);
    ~AddressesDialog();

    void ResetSelections();
    void SetWalletModel(WalletModel* model);

private slots:


    void on_buttonNewAccount_clicked();

    void on_buttonNewAddress_clicked();

private:
    Ui::AddressesDialog *ui;
    WalletModel* m_wallet;
    uint256 m_hashSeedSelected;
    uint32_t m_nAccountSelected;
    uint32_t m_nAddressSelected;

    void PopulateSeedsView();
    void PopulateAccountsView();
    void PopulateAddressesView();
};

#endif // ADDRESSESDIALOG_H
