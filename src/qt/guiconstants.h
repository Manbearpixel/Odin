// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_GUICONSTANTS_H
#define BITCOIN_QT_GUICONSTANTS_H

/* Milliseconds between model updates */
static const int MODEL_UPDATE_DELAY = 1000;

/* AskPassphraseDialog -- Maximum passphrase length */
static const int MAX_PASSPHRASE_SIZE = 1024;

/* ODIN GUI -- Size of icons in status bar */
static const int STATUSBAR_ICONSIZE = 32;

static const bool DEFAULT_SPLASHSCREEN = true;

/* Brand colors */
static const char *BRAND_COLOR_PRIMARY          = "#41C0D1";
static const char *BRAND_COLOR_DARK             = "#1D2323";
static const char *BRAND_COLOR_BLACK            = "#011612";
static const char *BRAND_COLOR_WHITE            = "#FCF9F1";
static const char *BRAND_COLOR_BLUE             = "#41C0D1";
static const char *BRAND_COLOR_BLUE_DARKER      = "#00A8B6";
static const char *BRAND_COLOR_ORANGE           = "#DD6B40";
static const char *BRAND_COLOR_RED              = "#CC4F49";
static const char *BRAND_COLOR_RED_LIGHTER      = "#CC6666";
static const char *BRAND_COLOR_YELLOW           = "#C4943F";
static const char *BRAND_COLOR_GREEN            = "#619772";
static const char *BRAND_COLOR_GREEN_LIGHTER    = "#88A881";

/* Invalid field background style */
#define STYLE_INVALID "background:#FF8080"

/* Transaction list -- unconfirmed transaction */
#define COLOR_UNCONFIRMED QColor(80, 80, 80)
/* Transaction list -- negative amount */
#define COLOR_NEGATIVE QColor(204, 79, 73)
/* Transaction list -- bare address (without label) */
#define COLOR_BAREADDRESS QColor(140, 140, 140)
/* Transaction list -- TX status decoration - open until date */
#define COLOR_TX_STATUS_OPENUNTILDATE QColor(64, 64, 255)
/* Transaction list -- TX status decoration - offline */
#define COLOR_TX_STATUS_OFFLINE QColor(192, 192, 192)
/* Transaction list -- TX status decoration - default color */
#define COLOR_BLACK QColor(0, 168, 182)
/* Transaction list -- TX status decoration - conflicted */
#define COLOR_CONFLICTED QColor(204, 79, 73)

/* Tooltips longer than this (in characters) are converted into rich text,
   so that they can be word-wrapped.
 */
static const int TOOLTIP_WRAP_THRESHOLD = 80;

/* Maximum allowed URI length */
static const int MAX_URI_LENGTH = 255;

/* QRCodeDialog -- size of exported QR Code image */
#define EXPORT_IMAGE_SIZE 256

/* Number of frames in spinner animation */
#define SPINNER_FRAMES 35

#define QAPP_ORG_NAME "ODIN"
#define QAPP_ORG_DOMAIN "odin.org"
#define QAPP_APP_NAME_DEFAULT "ODIN-Qt"
#define QAPP_APP_NAME_TESTNET "ODIN-Qt-testnet"

#endif // BITCOIN_QT_GUICONSTANTS_H
