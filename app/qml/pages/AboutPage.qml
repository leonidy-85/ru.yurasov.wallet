import QtQuick 2.5
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0

import "../db.js" as DB

import "../tabview" as Tabs

Tabs.TabItem  {
    id: aboutPage
    property bool largeScreen: Screen.width > 540

    Notification {
        id: notification
        itemCount: 1
    }
    function formatCode(data) {
          const dateString = String(data);
          var day = dateString.slice(0, 2)
          var month = dateString.slice(2, 4)
          var year = dateString.slice(4, 8)
          return day +'.'+month+'.'+year
    }
    SilicaFlickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: col.height
         anchors.topMargin: Theme.fontSizeExtraLarge * 3.5

        VerticalScrollDecorator {}

        Column {
            id: col
            spacing: 20
            width: parent.width

            Label {
                text:  qsTr("Wallet")
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: isLandscape ? (largeScreen ? "/usr/share/icons/hicolor/176x176/apps/ru.yurasov.wallet.png" : "/usr/share/icons/hicolor/86x86/apps/ru.yurasov.wallet.png") : (largeScreen ? "/usr/share/icons/hicolor/172x172/apps/ru.yurasov.wallet.png" : "/usr/share/icons/hicolor/128x128/apps/ru.yurasov.wallet.png")
            }
            Label {
                text: qsTr("Version") + " " + version
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.secondaryHighlightColor
            }
            Label {
                text: qsTr("Date Build") + " " + formatCode(builddate)
                font.pixelSize: Theme.fontSizeSmall
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.secondaryColor
            }
            Label {
                text: qsTr("Your wallet for all your scannable codes")
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width
                horizontalAlignment: Text.Center
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                color: Theme.secondaryColor
            }
            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }

            SectionHeader {
                text: qsTr("Donate")
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text:  qsTr("Your help allows us to make this project better.")
            }

            Label {
                id: ymoney
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("If you like the app, you can donate to the author of the app via ")+ "<a href=\"https://forms.yandex.ru/u/66d272b8068ff021f89c2953/\">ЮMoney </a>" + qsTr(" or ")+ "<a href=\"https://boosty.to/ub3gad/donate\"> Boosty</a>"
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                id: textBTC
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: "BTC: 1BS637NuDU37T8tfAwVMr5xSDD9vfg1mwT"
                MouseArea {
                  anchors.fill: parent
                    onClicked:{
                        var textToCopy = textBTC.text.substring(4);
                        Clipboard.text = textToCopy
                        DB.banner('OK', qsTr("The address has been copied to the clipboard."))
                    }
                }
            }

            Label {
                id: textTON
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
              // readOnly: true
                text: "TON: UQA0lae0x_zHXTeBjL4MfxKT6173nQBzq0R5CMo5owLvsKn3"
                MouseArea {
                  anchors.fill: parent
                    onClicked:{
                        var textToCopy = textTON.text.substring(4);
                        Clipboard.text = textToCopy
                        DB.banner('OK', qsTr("The address has been copied to the clipboard."))
                    }
                }
            }


            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }
            SectionHeader {
                text: qsTr("Author")
            }

            Label {
                text: qsTr("© Leonid Yurasov ")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Using ") + "<a href=\"https://github.com/zint/zint\">Zint</a>" + qsTr(
                          " as backend")
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                linkColor: Theme.highlightColor
                text: qsTr("Uses ")+ "<a href=\"https://gitlab.com/omprussia/examples/DocumentScanner\"> Document Scanner</a>"             + qsTr(" to get card covers")
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                linkColor: Theme.highlightColor
                text: qsTr("Based on ")+ "<a href=\"https://github.com/a-dekker/BarWal\">BarWal</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                linkColor: Theme.highlightColor
                text: qsTr("pkpass format support is taken from ")+ "<a href=\"https://bitbucket.org/christof_buergi/harbour-passviewer\">Pass Viewer</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
//            Label {
//                anchors.horizontalCenter: parent.horizontalCenter
//                width: parent.width - 2*Theme.horizontalPageMargin
//                horizontalAlignment: Text.AlignHCenter
//                wrapMode: Text.Wrap
//                font.pixelSize: Theme.fontSizeSmall
//                color: Theme.secondaryColor
//                text: qsTr("Thanks to Arno Dekker for the idea and inspiration")
//            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Thanks to ")  + "<a href=\"https://cactux.ru\">" + qsTr("Andrey Surganov") +"</a>" + qsTr(
                               " for store cards for the card adding wizard")
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Thanks to Alexander for the owner and the breakdown of the card number by 4 characters")
            }
            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }
            SectionHeader {
                text: qsTr("Licensed")
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Wallet is open source software licensed under the terms of the MIT/BSD.")
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("For suggestions, bugs, ideas or financial support visit ")
            }
            Button {
                text: "GitHub"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: Qt.openUrlExternally("https://github.com/leonidy-85/ru.yurasov.wallet/tree/dev_5")
            }
            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }

        }
    }
}