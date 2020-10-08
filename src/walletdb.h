// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_WALLETDB_H
#define BITCOIN_WALLETDB_H

#include "db.h"
#include "key.h"

#include <list>
#include <string>
#include <utility>
#include <vector>

class CAccount;
class CAccountingEntry;
class CBlockLocator;
class CKeyPool;
class CMasterKey;
class CScript;
class CWallet;
class CWalletTx;
class uint160;
class uint256;

/** Error statuses for the wallet database */
enum DBErrors
{
    DB_LOAD_OK,
    DB_CORRUPT,
    DB_NONCRITICAL_ERROR,
    DB_TOO_NEW,
    DB_LOAD_FAIL,
    DB_NEED_REWRITE
};

/* simple HD chain data model */
class CHDChain
{
public:
    uint32_t nExternalChainCounter;
    uint32_t nInternalChainCounter;
    CKeyID seed_id; //!< seed hash160
    CKeyID seed_id_r; //!< seed hash160 if the wallet uses 512bit seed saved to two keys

    static const int VERSION_HD_BASE        = 1;
    static const int VERSION_HD_CHAIN_SPLIT = 2;
    static const int CURRENT_VERSION        = VERSION_HD_CHAIN_SPLIT;
    int nVersion;

    CHDChain() { SetNull(); }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        READWRITE(nExternalChainCounter);
        READWRITE(seed_id);
        READWRITE(seed_id_r);
        if (this->nVersion >= VERSION_HD_CHAIN_SPLIT)
            READWRITE(nInternalChainCounter);
    )

    void SetNull()
    {
        nVersion = CHDChain::CURRENT_VERSION;
        nExternalChainCounter = 0;
        nInternalChainCounter = 0;
        seed_id.SetNull();
        seed_id_r.SetNull();
    }

    bool Is512BitSeed() const { return !seed_id_r.IsNull(); }

    uint256 GetId() const;
};

class CKeyMetadata
{
public:
    static const int CURRENT_VERSION=2;
    int nVersion;
    int64_t nCreateTime; // 0 means unknown
    std::string hdKeypath; //optional HD/bip32 keypath
    CKeyID hd_seed_id; //id of the HD seed used to derive this key
    CKeyID hd_seed_id_r; //if the key is derived using 512 bit seed, two keys are needed
    uint32_t nAccount; //Account that the Key belongs to

    CKeyMetadata()
    {
        SetNull();
    }
    CKeyMetadata(int64_t nCreateTime_)
    {
        nVersion = CKeyMetadata::CURRENT_VERSION;
        nCreateTime = nCreateTime_;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nCreateTime);
        if (nVersion > 1) {
            READWRITE(hdKeypath);
            READWRITE(hd_seed_id);
            READWRITE(hd_seed_id_r);
            READWRITE(nAccount);
        }
    )

    void SetNull()
    {
        nVersion = CKeyMetadata::CURRENT_VERSION;
        nCreateTime = 0;
        hdKeypath.clear();
        hd_seed_id.SetNull();
        hd_seed_id_r.SetNull();
        nAccount = 0;
    }
};


/** Access to the wallet database (wallet.dat) */
class CWalletDB : public CDB
{
public:
    CWalletDB(const std::string& strFilename, const char* pszMode = "r+") : CDB(strFilename, pszMode)
    {
    }
private:
    CWalletDB(const CWalletDB&);
    void operator=(const CWalletDB&);
public:
    bool WriteName(const std::string& strAddress, const std::string& strName);

    bool EraseName(const std::string& strAddress);

    bool WriteTx(uint256 hash, const CWalletTx& wtx);
    bool EraseTx(uint256 hash);

    bool WriteKey(const CPubKey& vchPubKey, const CPrivKey& vchPrivKey, const CKeyMetadata &keyMeta);
    bool WriteCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret, const CKeyMetadata &keyMeta);
    bool WriteMasterKey(unsigned int nID, const CMasterKey& kMasterKey);
    bool WriteHDChain(const CHDChain& chain);
    bool LoadHDChain(CHDChain& chain);

    bool WriteCScript(const uint160& hash, const CScript& redeemScript);

    bool WriteBestBlock(const CBlockLocator& locator);
    bool ReadBestBlock(CBlockLocator& locator);

    bool WriteOrderPosNext(int64_t nOrderPosNext);

    bool WriteDefaultKey(const CPubKey& vchPubKey);

    bool ReadPool(int64_t nPool, CKeyPool& keypool);
    bool WritePool(int64_t nPool, const CKeyPool& keypool);
    bool ErasePool(int64_t nPool);

    bool WriteMinVersion(int nVersion);

    bool ReadAccount(const std::string& strAccount, CAccount& account);
    bool WriteAccount(const std::string& strAccount, const CAccount& account);
private:
    bool WriteAccountingEntry(const uint64_t nAccEntryNum, const CAccountingEntry& acentry);
public:
    bool WriteAccountingEntry(const CAccountingEntry& acentry);
    int64_t GetAccountCreditDebit(const std::string& strAccount);
    void ListAccountCreditDebit(const std::string& strAccount, std::list<CAccountingEntry>& acentries);

    DBErrors ReorderTransactions(CWallet*);
    DBErrors LoadWallet(CWallet* pwallet);
    static bool Recover(CDBEnv& dbenv, std::string filename, bool fOnlyKeys);
    static bool Recover(CDBEnv& dbenv, std::string filename);
};

bool BackupWallet(const CWallet& wallet, const std::string& strDest);

#endif // BITCOIN_WALLETDB_H
