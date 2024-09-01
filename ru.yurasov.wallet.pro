TARGET = ru.yurasov.wallet

CONFIG += auroraapp
CONFIG += auroraapp_i18n

LIBS += -lpng
#LIBS += -lz
#LIBS += -lbz2
#LIBS += -llzma

#QT += network
#QT += sql
#QT += positioning
#QT += dbus

QT += multimedia

SOURCES += src/wallet.cpp \
        src/barcodeimageprovider.cpp \
        src/camerahelper.cpp \
        src/qmlimage.cpp \
        src/resizeimage.cpp

HEADERS += \
    src/ColorGenerator.h \
    src/ImageProcessor.h \
    src/barcodeimageprovider.h \
    src/imageconverter.h \
    src/camerahelper.h \
    src/qmlimage.h \
    src/resizeimage.h
#    src/zipfile.h \
#    src/zipfileimageprovider.h


DISTFILES += qml/wallet.qml \
    qml/cover/CoverPage.qml \
    qml/pages/About.qml \
    qml/pages/AddBarcodePage.qml \
    qml/pages/BarcodeDisplayPage.qml \
    qml/pages/MainPage.qml \
    qml/pages/ScanBarcodePage.qml \
    qml/pages/ViewPkpassPage.qml \
    qml/pages/WizardPage.qml \
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
    qml/logo.js \
    qml/db.js \
    README.md

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172

TRANSLATIONS = translations/ru.yurasov.wallet-ru.ts

DISTFILES += \
    qml/currencies.json


VERSION = $$system( egrep "^Version:\|^Release:" rpm/ru.yurasov.wallet.spec |tr -d "[A-Z][a-z]: " | tr "\\\n" "-" | sed "s/\.$//g"| tr -d "[:space:]")

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += BUILD_YEAR=$$system(date '+%Y')

include(src/zint/zint.pri)
include(src/FileIO/FileIO.pri)


message(AURORA_SDK_VERSION: $$AURORA_SDK_VERSION)
DEFINES += \
    AURORA_SDK_VERSION=\\\"$${AURORA_SDK_VERSION}\\\"
