#ifndef ADDRESSESDIALOG_H
#define ADDRESSESDIALOG_H

#include <QDialog>
#include <uint256.h>

namespace Ui {
class AddressesDialog;
}

class CWallet;

class AddressesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddressesDialog(CWallet* pwallet, QWidget *parent = nullptr);
    ~AddressesDialog();

private:
    Ui::AddressesDialog *ui;
    CWallet* m_wallet;
    uint256 m_hashSeedSelected;
    uint32_t m_nAccountSelected;
    uint32_t m_nAddressSelected;

    void PopulateSeedsView();
    void PopulateAccountsView();
    void PopulateAddressesView();
};

#endif // ADDRESSESDIALOG_H
