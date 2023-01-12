// Copyright (c) 2019 The Veil developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "mnemonicdisplay.h"
#include "ui_mnemonicdisplay.h"

#include <QMessageBox>
#include <sstream>
#include <boost/algorithm/string/join.hpp>
#include <mnemonic/dictionary.h>
#include <mnemonic/mnemonic.h>

MnemonicDisplay::MnemonicDisplay(bool fRetry, QWidget *parent) : QDialog(parent), ui(new Ui::MnemonicDisplay)
{
    shutdown = true;
    ui->setupUi(this);
    if (fRetry) {
        ui->lblExplanation->setText("INVALID SEED: Please try again.");
        ui->lblExplanation->setStyleSheet("QLabel { color : red; }");
    } else
        ui->lblExplanation->setText("Please paste your seed in the box below.");
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

MnemonicDisplay::MnemonicDisplay(QString seed, QWidget *parent) : QDialog(parent), ui(new Ui::MnemonicDisplay)
{
    shutdown = true;
    ui->setupUi(this);
    ui->seedEdit->setText(seed);
    ui->seedEdit->setReadOnly(true);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

MnemonicDisplay::~MnemonicDisplay()
{
    delete ui;
}

void MnemonicDisplay::buttonClicked()
{
    shutdown = false;
    if (ui->seedEdit->toPlainText().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Empty seed phrase entered");
        msgBox.exec();
        return;
    }

    QStringList list1 = ui->seedEdit->toPlainText().split(QLatin1Char(' '));
    if (list1.size() != 24) {
        QMessageBox msgBox;
        msgBox.setText("Seed phrase must have 24 words");
        msgBox.exec();
        return;
    }

    std::vector<std::string> listWords;
    for (int i = 0; i < list1.size(); i++) {
        listWords.emplace_back(list1.at(i).toStdString());
    }
    if (!validate_mnemonic(listWords)) {
        QMessageBox msgBox;
        msgBox.setText("Seed phrase is not valid");
        msgBox.exec();
        return;
    }

    importSeed = ui->seedEdit->toPlainText();
    this->close();
}
