﻿#include "enterlicensedialog.h"
#include "ui_enterlicensedialog.h"
#include "tools.h"
#include "globals.h"
#include "messagedialog.h"
#include "mainwindow.h"

#include <QCryptographicHash>
#include <QTextStream>

#define __LICENSE_TIMESTAMP_FROMAT__ "yyyyMMddhhmmss"

EnterLicenseDialog::EnterLicenseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterLicenseDialog),
    mLicense(false),
    mMainWindow((MainWindow *) parent)
{
    ui->setupUi(this);
    ui->label_payInfo->setText(ui->label_payInfo->text().append(__PurchaseLicenseUrl__));
}

EnterLicenseDialog::~EnterLicenseDialog()
{
    delete ui;
}

void EnterLicenseDialog::on_buttonBox_accepted()
{
    const QString license = ui->plainTextEdit->toPlainText();

    if (license.isEmpty()) {
        return;
    }

    if (checkLicense(license, true)) {
        mMainWindow->menuBar()->initLicenseAction(mLicense);
        (new MessageDialog(mMainWindow))->openMessage(tr(u8"如有任何疑问, 请随时联系: %1").arg(__EmailToUser__), tr(u8"感谢您的购买和支持 (｡･ω･｡)"));
    }
    else {
        (new MessageDialog(mMainWindow))->openMessage(
                tr(u8"请您检查是否输入正确, 包括许可证的开始和结束行哦~\n如确认无误仍然无法激活, 请及时联系: %1").arg(__EmailToUser__),
                tr(u8"该许可证似乎无效 ╮(￣▽￣)╭"));
    }
}

bool EnterLicenseDialog::checkLicense(QString license, bool save)
{
    mLicense = false;

    QString lid, userinfo, startTime, endTime, licenseKeyPart1, licenseKeyPart2 = "";
    QTextStream in(&license);
    QStringList stringList = {};

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }
        stringList.append(line);
    }

    if (stringList.length() != 13) {
        return false;
    }

    for (int i = 0; i < stringList.length(); ++i) {
        if (0 == i) {
            if (u8"—– BEGIN LICENSE —–" != stringList[i]) {
                return false;
            }
            continue;
        }
        else if (12 == i) {
            if (u8"—– END LICENSE —–" != stringList[i]) {
                return false;
            }
            continue;
        }
        else if (1 == i) {
            userinfo = stringList[i];
        }
        else if (2 == i) {
            QStringList list = stringList[i].split("-");
            startTime = list[0];
            endTime = list[1];
        }
        else if (3 == i) {
            if (!stringList[i].startsWith("LID-")) {
                return false;
            }

            lid = stringList[i].mid(4);
        }
        else if (4 <= i && i <= 7) {
            licenseKeyPart1 += stringList[i].replace(" ", "");
        }
        else if (8 <= i && i <= 11) {
            licenseKeyPart2 += stringList[i].replace(" ", "");
        }
    }

    const QString salt = "FsUr*5GDs9u#o0@Zvace5WAyAmVGX09u";
    const QString productType = QSysInfo::kernelType();
    const QString sha512Part1 = QString(QCryptographicHash::hash(QString(startTime + userinfo + salt + lid + salt + productType).toUtf8(), QCryptographicHash::Sha512).toHex()).toUpper();
    const QString sha512Part2 = QString(QCryptographicHash::hash(QString(endTime + lid + salt + userinfo + salt + productType).toUtf8(), QCryptographicHash::Sha512).toHex()).toUpper();

    if (sha512Part1 != licenseKeyPart1 || sha512Part2 != licenseKeyPart2) {
        return false;
    }

    int nowTimestamp = static_cast<int>(QDateTime::currentSecsSinceEpoch());
    int startTimestamp = Tools::timestampFromDateTime(startTime, __LICENSE_TIMESTAMP_FROMAT__);
    int endTimestamp = Tools::timestampFromDateTime(endTime, __LICENSE_TIMESTAMP_FROMAT__);
    endTimestamp = endTimestamp == -1 ? std::numeric_limits<int>::max() : endTimestamp;

    if (nowTimestamp < startTimestamp || nowTimestamp > endTimestamp || startTimestamp >= endTimestamp) {
        return false;
    }

    if (save) {
        Tools::writerFile(__AppLicensePath__, Tools::encrypt(license).toUtf8());
    }

    mLicense = true;

    return true;
}

void EnterLicenseDialog::init()
{
    QString license = Tools::readerFileString(__AppLicensePath__);
    mLicense = license.isEmpty() ? false : checkLicense(Tools::decrypt(license));
    mMainWindow->menuBar()->initLicenseAction(mLicense);
}

void EnterLicenseDialog::showEvent(QShowEvent *showEvent)
{
    QDialog::showEvent(showEvent);

    mMainWindow->gAnalytics()->sendScreenView(objectName());
}
