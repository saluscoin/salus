// Copyright (c) 2019 The Veil developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef VEIL_MNEMONICWALLETINIT_H
#define VEIL_MNEMONICWALLETINIT_H

#include <wallet.h>

class MnemonicWalletInit
{
private:
    uint512 m_seed;
public:
    bool Open(bool& fNewSeed);

    const uint512& Seed() const;
};

#endif //VEIL_MNEMONICWALLETINIT_H
