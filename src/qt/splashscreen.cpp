// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2018 The Phore developers
// Copyright (c) 2018 The ODIN developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "guiconstants.h"
#include "splashscreen.h"

#include "clientversion.h"
#include "init.h"
#include "networkstyle.h"
#include "ui_interface.h"
#include "util.h"
#include "version.h"

#ifdef ENABLE_WALLET
#include "wallet.h"
#endif

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QPainter>

SplashScreen::SplashScreen(Qt::WindowFlags f, const NetworkStyle* networkStyle) : QWidget(0, f), curAlignment(0)
{
    // set reference point, paddings
    int paddingLeft = 14;
    int paddingTop = 760;
    int titleVersionVSpace = 25;
    int titleCopyrightVSpace = 32;

    float fontFactor = 1.0;

    // define text to place
    QString titleText = tr("ODIN Blockchain Core");
    QString versionText = QStringLiteral("Version %1").arg(QString::fromStdString(FormatFullVersion()));
    QString networkParam = networkStyle->getTitleAddText();

    QString copyrightTextBtc = QChar(0xA9) + QString(" 2009-%1 ")
        .arg(COPYRIGHT_YEAR) + QString(tr("The Bitcoin Core developers"));
    QString copyrightTextDash = QChar(0xA9) + QString(" 2014-%1 ")
        .arg(COPYRIGHT_YEAR) + QString(tr("The Dash Core developers"));
    QString copyrightTextPIVX = QChar(0xA9) + QString(" 2015-%1 ")
        .arg(COPYRIGHT_YEAR) + QString(tr("The PIVX Core developers"));
    QString copyrightTextPhore = QChar(0xA9) + QString(" 2017-%1 ")
        .arg(COPYRIGHT_YEAR) + QString(tr("The Phore Core developers"));
    QString copyrightTextODIN = QChar(0xA9) + QString(" 2018-%1 ")
        .arg(COPYRIGHT_YEAR) + QString(tr("The ODIN Core developers"));

    int qfontId = QFontDatabase::addApplicationFont(":/fonts/Rubik-Light");
    QString family = QFontDatabase::applicationFontFamilies(qfontId).at(0);

    // load the bitmap for writing some text over it
    pixmap = networkStyle->getSplashImage();
    // pixmap = pixmap.scaled(QSize(400,400), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter pixPaint(&pixmap);
    pixPaint.setRenderHint(QPainter::Antialiasing);
    pixPaint.setRenderHint(QPainter::HighQualityAntialiasing);
    pixPaint.setPen(QColor(65, 192, 209));

    // check font size and drawing with
    pixPaint.setFont(QFont(family, 28 * fontFactor));
    QFontMetrics fm = pixPaint.fontMetrics();

    pixPaint.setFont(QFont(family, 28 * fontFactor));
    fm = pixPaint.fontMetrics();

    // draw titleText
    pixPaint.drawText(paddingLeft, paddingTop, titleText);

    // draw versionText
    pixPaint.setFont(QFont(family, 15 * fontFactor));
    pixPaint.drawText(paddingLeft, paddingTop + titleVersionVSpace, versionText);

    // draw copyrights
    // pixPaint.setFont(QFont(family, 10 * fontFactor));
    // pixPaint.drawText(paddingLeft, paddingTop + titleCopyrightVSpace, copyrightTextBtc);
    // pixPaint.drawText(paddingLeft, paddingTop + titleCopyrightVSpace + 12, copyrightTextDash);
    // pixPaint.drawText(paddingLeft, paddingTop + titleCopyrightVSpace + 24, copyrightTextPIVX);
    // pixPaint.drawText(paddingLeft, paddingTop + titleCopyrightVSpace + 36, copyrightTextPhore);
    // pixPaint.drawText(paddingLeft, paddingTop + titleCopyrightVSpace + 48, copyrightTextODIN);

    // draw additional text if special network
    if (!networkParam.isEmpty()) {
        pixPaint.setFont(QFont(family, 12 * fontFactor));
        fm = pixPaint.fontMetrics();
        int titleAddTextWidth = fm.width(networkParam);
        pixPaint.drawText(pixmap.width() - titleAddTextWidth - 10, pixmap.height() - 25, networkParam);
    }

    pixPaint.end();

    // Set window title
    setWindowTitle(QStringLiteral("%1 %2").arg(titleText).arg(networkParam));

    // Resize window and move to center of desktop, disallow resizing
    QRect r(QPoint(), pixmap.size());
    resize(r.size());
    setFixedSize(r.size());
    move(QApplication::desktop()->screenGeometry().center() - r.center());

    subscribeToCoreSignals();
}

SplashScreen::~SplashScreen()
{
    unsubscribeFromCoreSignals();
}

void SplashScreen::slotFinish(QWidget* mainWin)
{
    Q_UNUSED(mainWin);
    hide();
}

static void InitMessage(SplashScreen* splash, const std::string& message)
{
    QMetaObject::invokeMethod(splash, "showMessage",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(message)),
        Q_ARG(int, Qt::AlignBottom | Qt::AlignRight),
        Q_ARG(QColor, QColor(252, 249, 241)));
}

static void ShowProgress(SplashScreen* splash, const std::string& title, int nProgress)
{
    InitMessage(splash, title + strprintf("%d", nProgress) + "%");
}

#ifdef ENABLE_WALLET
static void ConnectWallet(SplashScreen* splash, CWallet* wallet)
{
    wallet->ShowProgress.connect(boost::bind(ShowProgress, splash, _1, _2));
}
#endif

void SplashScreen::subscribeToCoreSignals()
{
    // Connect signals to client
    uiInterface.InitMessage.connect(boost::bind(InitMessage, this, _1));
    uiInterface.ShowProgress.connect(boost::bind(ShowProgress, this, _1, _2));
#ifdef ENABLE_WALLET
    uiInterface.LoadWallet.connect(boost::bind(ConnectWallet, this, _1));
#endif
}

void SplashScreen::unsubscribeFromCoreSignals()
{
    // Disconnect signals from client
    uiInterface.InitMessage.disconnect(boost::bind(InitMessage, this, _1));
    uiInterface.ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
#ifdef ENABLE_WALLET
    if (pwalletMain)
        pwalletMain->ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
#endif
}

void SplashScreen::showMessage(const QString& message, int alignment, const QColor& color)
{
    curMessage = message;
    curAlignment = alignment;
    curColor = color;
    update();
}

void SplashScreen::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    QRect r = rect().adjusted(5, 5, -5, -5);
    painter.setPen(curColor);
    painter.drawText(r, curAlignment, curMessage);
}

void SplashScreen::closeEvent(QCloseEvent* event)
{
    StartShutdown(); // allows an "emergency" shutdown during startup
    event->ignore();
}
