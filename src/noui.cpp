// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2019 The Veil developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ui_interface.h"
#include "init.h"

#include <string>

static int noui_ThreadSafeMessageBox(const std::string& message, const std::string& caption, unsigned int style)
{
    std::string strCaption;
    bool fLog = true;
    // Check for usage of predefined caption
    switch (style) {
    case CClientUIInterface::MSG_ERROR:
        strCaption += _("Error");
        break;
    case CClientUIInterface::MSG_WARNING:
        strCaption += _("Warning");
        break;
    case CClientUIInterface::MSG_INFORMATION:
        strCaption += _("Information");
        break;
    case CClientUIInterface::MSG_PRIVATE:
        strCaption += _("Private");
        fLog = false;
    default:
        strCaption += caption; // Use supplied caption
    }

    //Some things (like seed words) do not belong saved to debug log
    if (fLog) {
        LogPrintf("%s: %s\n", caption, message);
    }
    fprintf(stderr, "%s: %s\n", strCaption.c_str(), message.c_str());
    return 4;
}

static bool noui_ThreadSafeQuestion(const std::string& /* ignored interactive message */, const std::string& message, const std::string& caption, unsigned int style)
{
    if (noui_ThreadSafeMessageBox(message, caption, style) == 4)
        return false;
    return true;
}

static bool noui_ThreadSafeAskFee(int64_t nFeeRequired, const std::string& strCaption)
{
    return true;
}

static void noui_InitMessage(const std::string &message)
{
    LogPrintf("init message: %s\n", message);
}

static bool noui_DisplayWalletMnemonic(std::string& mnemonic)
{
    noui_ThreadSafeMessageBox(mnemonic, " WARNING BACKUP THESE WORDS SECURELY ALL OF YOUR SALUS CRYPTOCURRENCY IS TIED TO THESE SEED WORDS!! Seed", CClientUIInterface::MSG_PRIVATE);
    return true;
}

static bool noui_InitNewWallet(std::string& mnemonic, unsigned int& flag)
{
    switch (flag) {
        case MnemonicWalletInitFlags::SELECT_LANGUAGE:
        case MnemonicWalletInitFlags::PROMPT_MNEMONIC:
        {
            return noui_ThreadSafeQuestion("", "To import existing seed phrase restart wallet with -importseed=\"\" . To generate a new seed start wallet with -generateseed=1 ", "New Wallet Load Detected: ", -1);
        }
        case MnemonicWalletInitFlags::NEW_MNEMONIC:
        {
            noui_DisplayWalletMnemonic(mnemonic);
            return true;
        }
        case MnemonicWalletInitFlags::IMPORT_MNEMONIC:
        {
            //Should never reach this spot
            noui_ThreadSafeMessageBox("Error with seed import.", "", CClientUIInterface::MSG_ERROR);
            return false;
        }
        case MnemonicWalletInitFlags::INVALID_MNEMONIC:
        {
            noui_ThreadSafeMessageBox("Failed to import mnemonic seed, format is invalid.", "", CClientUIInterface::MSG_ERROR);
            return false;
        }
        default:
            return false;
    }
    return false;
}

void noui_connect()
{
    // Connect bitcoind signal handlers
    uiInterface.ThreadSafeMessageBox.connect(noui_ThreadSafeMessageBox);
    uiInterface.ThreadSafeAskFee.connect(noui_ThreadSafeAskFee);
    uiInterface.InitMessage.connect(noui_InitMessage);
    uiInterface.InitWallet.connect(noui_InitNewWallet);
}
