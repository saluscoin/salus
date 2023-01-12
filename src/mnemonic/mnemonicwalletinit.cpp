// Copyright (c) 2019 The Veil developers
// Copyright (c) 2021 SaluS developers
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
#include <istream>

//const WalletInitInterface& g_wallet_init_interface = MnemonicWalletInit();

namespace fs = boost::filesystem;

bool MnemonicWalletInit::Open(bool& fNewSeed)
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

    fNewSeed = false;
    fs::path walletPath = fs::absolute(walletFile, strDataDir);
    fNewSeed = true;
    unsigned int initOption = MnemonicWalletInitFlags::INVALID_MNEMONIC;
    /** check startup args because daemon is not interactive **/
    if (GetBoolArg("-generateseed", false))
        initOption = MnemonicWalletInitFlags::NEW_MNEMONIC;
    std::string strSeedPhraseArg = GetArg("-importseed", "");
    if (!strSeedPhraseArg.empty()) {
        initOption = MnemonicWalletInitFlags::IMPORT_MNEMONIC;
        //validate the mnemonic
        std::vector<std::string> words;
        std::istringstream iss(strSeedPhraseArg);
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(words));
        if (!validate_mnemonic(words)) {
            return error("%s: seed phrase is not valid!", __func__);
        }
    }

    /**If no startup args, then launch prompt asking to import a seed or generate new **/
    if (initOption == MnemonicWalletInitFlags::INVALID_MNEMONIC) {
        // Prompt the user to either enter a seed phrase, or generate a new one
        unsigned int ret = 0;
        if (!InitNewWalletPrompt(ret))
            return error("%s: failed to get wallet mnemonic language", __func__);
        initOption = static_cast<MnemonicWalletInitFlags>(ret);
    }

    /** Create a new mnemonic **/
    if (initOption == MnemonicWalletInitFlags::NEW_MNEMONIC) {
        std::string mnemonic;
        veil::GenerateNewMnemonicSeed(mnemonic, "english");
        if (!DisplayWalletMnemonic(mnemonic))
            return false;
        strSeedPhraseArg = mnemonic;
        initOption = MnemonicWalletInitFlags::IMPORT_MNEMONIC;
    }

    /** Convert the mnemonic phrase to the final seed used for the wallet **/
    if (initOption == MnemonicWalletInitFlags::IMPORT_MNEMONIC) {
        // All options should end here.
        // 1: User generates new seed. strSeedPhrase arg should be populated.
        // 2: User selects in GUI to import a seed. strSeedPhrase should not be populated.
        // 3: User uses daemon with -importseed option. strSeedPhrase should be populated
        if (strSeedPhraseArg.empty() && !GetWalletMnemonic(strSeedPhraseArg))
            return false;

        // Do not allow a new seed based on an empty seed phrase
        if (strSeedPhraseArg.empty()) {
            LogPrintf("Cannot import an empty seed phrase!\n");
            return false;
        }

        // Convert the BIP39 mnemonic phrase into the final 512bit wallet seed
        auto hashRet = decode_mnemonic(strSeedPhraseArg);
        memcpy(m_seed.begin(), hashRet.begin(), hashRet.size());
    }

    if (fNewSeed) {
        // Create new keyUser and set as default key
        RandAddSeedPerfmon();

        // Save the seed to the wallet
        assert(m_seed != uint512(0));
        pwalletMain->SetHDSeed_512(m_seed);

        CPubKey newDefaultKey;
        if (pwalletMain->GetKeyFromPool(newDefaultKey)) {
            pwalletMain->SetDefaultKey(newDefaultKey);
            if (!pwalletMain->SetAddressBookName(pwalletMain->vchDefaultKey.GetID(), ""))
                LogPrintf("Cannot write default address\n");
        }
        pwalletMain->SetBestChain(CBlockLocator(pindexBest));
    }

    return true;
}

const uint512& MnemonicWalletInit::Seed() const
{
    return m_seed;
}