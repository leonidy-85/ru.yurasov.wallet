TARGET = ru.yurasov.wallet

CONFIG += auroraapp
CONFIG += auroraapp_i18n

LIBS += -lpng

SOURCES += src/wallet.cpp \
    src/barcodeimageprovider.cpp \
    src/osread.cpp \
    src/settings.cpp

HEADERS += \
    src/barcodeimageprovider.h \
    src/imageconverter.h \
    src/osread.h \
    src/settings.h


DISTFILES += qml/wallet.qml \
    qml/cover/CoverPage.qml \
    qml/pages/About.qml \
    qml/pages/AddBarcodePage.qml \
    qml/pages/BarcodeDisplayPage.qml \
    qml/pages/MainPage.qml \
    qml/pages/ScanBarcodePage.qml \
    qml/pagesEditBarcodeGroupPage.qml \
    qml/pages/EditBarcodePage.qml \
    qml/components/FancyPageHeader.qml \
    qml/components/AppBarMenu.qml \
    ru.yurasov.wallet.desktop \
    qml/icons/aztec-icon.png \
    qml/icons/barcode-icon.png \
    qml/icons/datamatrix-icon.png \
    qml/icons/qrcode-icon.png \
    qml/icons/aztec-icon-black.png \
    qml/icons/barcode-icon-black.png \
    qml/icons/datamatrix-icon-black.png \
    qml/icons/qrcode-icon-black.png \
    qml/icons/shablon.png \
    qml/icons/shablon.svg \
    qml/cover/cover_background.png \
    rpm/ru.yurasov.wallet.changes.in \
    rpm/ru.yurasov.wallet.spec \
    qml/db.js

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172

TRANSLATIONS = translations/ru.yurasov.wallet-ru.ts



VERSION = $$system( egrep "^Version:\|^Release:" rpm/ru.yurasov.wallet.spec |tr -d "[A-Z][a-z]: " | tr "\\\n" "-" | sed "s/\.$//g"| tr -d "[:space:]")

#VERSION= 1.3-4

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += BUILD_YEAR=$$system(date '+%Y')

include(src/zint/zint.pri)
include(src/FileIO/FileIO.pri)


