TARGET = ru.yurasov.wallet

TEMPLATE = app

CONFIG += auroraapp
CONFIG += auroraapp_i18n

LIBS += -lz
LIBS += -lbz2
LIBS += -llzma
LIBS += -lnemonotifications-qt5
LIBS += -lpng


SUBDIRS = \
    libs \

QT += multimedia core concurrent dbus sql positioning network


DEFINES += \
    DBUS_SERVICE=\\\"ru.yurasov.wallet\\\" \
    DBUS_PATH=\\\"/ru/yurasov/wallet\\\" \
    DBUS_INTERFACE=\\\"ru.yurasov.wallet\\\" \


INCLUDEPATH += \
    $$PWD/../libs/opencv/modules/core/include \
    $$PWD/../libs/opencv/modules/imgproc/include \
#    $$PWD/../libs/opencv/modules/objdetect/include \
    $$OUT_PWD/../libs/opencv \

DEPENDPATH += \
    $$PWD/../libs/opencv/modules/core/include \
    $$PWD/../libs/opencv/modules/imgproc/include \
#    $$PWD/../libs/opencv/modules/objdetect/include \
    $$OUT_PWD/../libs/opencv \

LIBS += \
    -L$$OUT_PWD/../libs/opencv/lib -lopencv_core -lopencv_imgproc



    SOURCES += src/wallet.cpp \
            src/generator/barcodeimageprovider.cpp \
            src/camerahelper.cpp \
            src/qmlimage.cpp \
            src/image/resizeimage.cpp \
            src/scan/helper.cpp \
            src/scan/cornerpoint.cpp \
            src/scan/holedoverlay.cpp \
            src/scan/photoeditor.cpp \
            src/scan/photocamera.cpp \
            src/scan/photorecognizer.cpp \
            src/scan/photoaligner.cpp \
            src/scan/page.cpp \
            src/scan/pagemodel.cpp \
            src/scan/document.cpp \
            src/scan/documentmodel.cpp \
            src/scan/storage.cpp \
            src/scan/service.cpp \
            src/scan/types.cpp \
            src/scan/zoomview.cpp \
            src/pkpass/homewatcher.cpp \
            src/pkpass/settingsstore.cpp \
            src/pkpass/zipfile.cpp \
            src/pkpass/zipfileimageprovider.cpp \
            src/pkpass/datetimeformat.cpp \
            src/pkpass/currencyformat.cpp \
            src/pkpass/passhandler.cpp \
            src/pkpass/homescanner.cpp \
            src/pkpass/passdb.cpp \
            src/pkpass/passinfo.cpp \
            src/pkpass/notificator.cpp

HEADERS += \
    src/image/ColorGenerator.h \
    src/image/ImageProcessor.h \
    src/generator/barcodeimageprovider.h \
    src/image/imageconverter.h \
    src/camerahelper.h \
    src/qmlimage.h \
    src/image/resizeimage.h\
    src/scan/helper.h \
    src/scan/cornerpoint.h \
    src/scan/holedoverlay.h \
    src/scan/photoeditor.h \
    src/scan/photocamera.h \
    src/scan/photorecognizer.h \
    src/scan/photoaligner.h \
    src/scan/page.h \
    src/scan/pagemodel.h \
    src/scan/document.h \
    src/scan/documentmodel.h \
    src/scan/storage.h \
    src/scan/service.h \
    src/scan/types.h \
    src/scan/zoomview.h \
    src/pkpass/homewatcher.h \
    src/pkpass/settingsstore.h \
    src/pkpass/zint/gs1.h \
    src/pkpass/zipfile.h \
    src/pkpass/zipfileimageprovider.h \
    src/pkpass/datetimeformat.h \
    src/pkpass/currencyformat.h \
    src/pkpass/passhandler.h \
    src/pkpass/homescanner.h \
    src/pkpass/passdb.h \
    src/pkpass/passinfo.h \
    src/pkpass/notificator.h

DISTFILES += qml/wallet.qml \
    qml/cover/CoverPage.qml \
    qml/pages/*.* \
    qml/components/FancyPageHeader.qml \
    qml/components/AppBarMenu.qml \
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
    qml/currencies.json \
    pkpass/*.*

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172

TRANSLATIONS = translations/ru.yurasov.wallet-ru.ts

VERSION = $$system( egrep "^Version:\|^Release:" ../rpm/ru.yurasov.wallet.spec |tr -d "[A-Z][a-z]: " | tr "\\\n" "-" | sed "s/\.$//g"| tr -d "[:space:]")

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += BUILD_DATE=$$system(date '+%d%m%Y')

include(src/zint/zint.pri)
include(src/FileIO/FileIO.pri)

include (src/utils/utils.pri)

message(AURORA_SDK_VERSION: $$AURORA_SDK_VERSION)
DEFINES += \
    AURORA_SDK_VERSION=\\\"$${AURORA_SDK_VERSION}\\\"
