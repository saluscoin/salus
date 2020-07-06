// Copyright (c) 2019 The Veil developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mnemonic/mnemonicwalletinit.h>

#include <init.h>
#include <ui_interface.h>
#include <util.h>
#include <wallet.h>
//#include <wallet/walletutil.h>
#include "mnemonic.h"
#include "generateseed.h"
#include "walletinitflags.h"

//const WalletInitInterface& g_wallet_init_interface = MnemonicWalletInit();

namespace fs = boost::filesystem;

bool MnemonicWalletInit::Open() const
{
    if (GetBoolArg("-disablewallet", false)) {
        LogPrintf("Wallet disabled!\n");
        return true;
    }

    std::string strDataDir = GetDataDir().string();
    std::string walletFile = GetArg("-wallet", "wallet.dat");

    // strWalletFileName must be a plain filename without a directory
//    if (strWalletFileName != boost::filesystem::basename(strWalletFileName) + boost::filesystem::extension(strWalletFileName))
//        return InitError(strprintf(_("Wallet %s resides outside data directory %s."), strWalletFileName, strDataDir));

    bool fNewSeed = false;
    uint512 hashMasterKey;
    fs::path walletPath = fs::absolute(walletFile, strDataDir);
    if ((walletFile == "" && !fs::exists(walletPath / "wallet.dat")) || !fs::exists(walletPath)) {
        fNewSeed = true;
        unsigned int initOption = MnemonicWalletInitFlags::INVALID_MNEMONIC;
        /** check startup args because daemon is not interactive **/
        if (GetBoolArg("-generateseed", false))
            initOption = MnemonicWalletInitFlags::NEW_MNEMONIC;

        std::string strSeedPhraseArg = GetArg("-importseed", "");
        if (!strSeedPhraseArg.empty())
            initOption = MnemonicWalletInitFlags::IMPORT_MNEMONIC;

        /**If no startup args, then launch prompt **/
        std::string strMessage = "english";
        if (initOption == MnemonicWalletInitFlags::INVALID_MNEMONIC) {
            // Language only routes to GUI. It returns with the filled out mnemonic in strMessage
            if (!GetWalletMnemonicLanguage(strMessage, initOption))
                return false;
            // The mnemonic phrase now needs to be converted to the final wallet seed (note: different than the phrase seed)
            strSeedPhraseArg = strMessage;
            //LogPrintf("%s: mnemonic phrase: %s\n", __func__, strSeedPhraseArg);
            initOption = MnemonicWalletInitFlags::IMPORT_MNEMONIC;
        }

        /** Create a new mnemonic - this should only be triggered via daemon **/
        if (initOption == MnemonicWalletInitFlags::NEW_MNEMONIC) {
            std::string mnemonic;
            veil::GenerateNewMnemonicSeed(mnemonic, strMessage);
            if (!DisplayWalletMnemonic(mnemonic))
                return false;
            strSeedPhraseArg = mnemonic;
            initOption = MnemonicWalletInitFlags::IMPORT_MNEMONIC;
        }

        /** Convert the mnemonic phrase to the final seed used for the wallet **/
        if (initOption == MnemonicWalletInitFlags::IMPORT_MNEMONIC) {
            // Convert the BIP39 mnemonic phrase into the final 512bit wallet seed
            auto hashRet = decode_mnemonic(strSeedPhraseArg);
            memcpy(hashMasterKey.begin(), hashRet.begin(), hashRet.size());
            //LogPrintf("%s: Staging for loading seed %s\n", __func__, hashMasterKey.GetHex());
        }
    }

    bool fFirstRun = true;
    pwalletMain = new CWallet(walletFile);
    DBErrors nLoadWalletRet = pwalletMain->LoadWallet(fFirstRun);

    std::string strErrors;
    if (nLoadWalletRet != DB_LOAD_OK)
    {
        if (nLoadWalletRet == DB_CORRUPT)
            strErrors += _("Error loading wallet.dat: Wallet corrupted") + "\n";
        else if (nLoadWalletRet == DB_NONCRITICAL_ERROR)
        {
            string msg(_("Warning: error reading wallet.dat! All keys read correctly, but transaction data"
                         " or address book entries might be missing or incorrect."));
            //InitWarning(msg);
        }
        else if (nLoadWalletRet == DB_TOO_NEW)
            strErrors += _("Error loading wallet.dat: Wallet requires newer version of SaluS") + "\n";
        else if (nLoadWalletRet == DB_NEED_REWRITE)
        {
            strErrors += _("Wallet needed to be rewritten: restart SaluS to complete") + "\n";
            LogPrintf("%s", strErrors);
            //return InitError(strErrors);
            return false;
        }
        else
            strErrors += _("Error loading wallet.dat") + "\n";
    }

//    std::shared_ptr<CWallet> pwallet = CWallet::CreateWalletFromFile(walletFile, walletPath, 0, fNewSeed ? &hashMasterKey : nullptr);
//    if (!pwallet) {
//        return false;
//    }

//
//    AddWallet(pwallet);

    return true;
}
