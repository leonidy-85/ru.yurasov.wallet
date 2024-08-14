import QtQuick 2.5
import Sailfish.Silica 1.0

CoverBackground {
    id: cover

    BackgroundItem {
        anchors.fill: parent

        HighlightImage {
            id: coverBgImage
            color: Theme.primaryColor
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: "cover_background.png"
            opacity: 0.2
            horizontalAlignment: Image.AlignHCenter
            verticalAlignment: Image.AlignVCenter
            visible: !mainapp.barcodeDisplayed
        }
    }

    Label {
        id: coverHeader
        text: "Wallet"
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingMedium
        horizontalAlignment: Text.Center
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
    }
    Image {
        id: barcodeIcon
        anchors.top: coverHeader.bottom
        anchors.margins: Theme.paddingMedium
        anchors.horizontalCenter: parent.horizontalCenter
        source: mainapp.iconSource
        sourceSize: Qt.size(Theme.itemSizeSmall, Theme.itemSizeSmall)
        visible: mainapp.barcodeDisplayed
    }
    Label {
        id: barcodeText
        anchors.top: barcodeIcon.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.Center
        text: mainapp.codeDescription
        wrapMode: Text.Wrap
        width: parent.width
        visible: barcodeDisplayed
    }
    Image {
        anchors.top: barcodeText.bottom
        anchors.topMargin: Theme.paddingMedium
        source: mainapp.barcodeDisplayed ? "/tmp/barcode.svg" : "/usr/share/icons/hicolor/128x128/apps/ru.yurasov.wallet.png"
        anchors.horizontalCenter: parent.horizontalCenter
        cache: false
        fillMode: Image.PreserveAspectFit
        width: parent.width / 1.5
        visible: mainapp.barcodeDisplayed
    }
}
