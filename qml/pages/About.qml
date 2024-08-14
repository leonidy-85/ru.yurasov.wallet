import QtQuick 2.5
import Sailfish.Silica 1.0
import ru.yurasov.wallet.Launcher 1.0

Page {
    id: aboutPage
    property bool largeScreen: Screen.width > 540

    App {
        id: bar
    }

    SilicaFlickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: col.height

        VerticalScrollDecorator {}

        Column {
            id: col
            spacing: 20
            width: parent.width
            PageHeader {
                objectName: "pageHeader"
            }

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
                text: qsTr("Author")
            }

            Label {
                text: qsTr("© Leonid Yurasov") + buildyear
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Wallet is open source software licensed under the terms of the GNU General Public License.")
            }
                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width - 2*Theme.horizontalPageMargin
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.Wrap
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.secondaryColor
                            text: qsTr("For suggestions, bugs and ideas visit ")
                        }
                        Button {
                            text: "GitHub"
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: Qt.openUrlExternally("https://github.com/leonidy-85/ru.yurasov.wallet")
                        }
            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }
            Label {
                x: Theme.paddingLarge
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeTiny
                text: qsTr("Using ") + "<a href=\"https://github.com/zint/zint\">Zint</a>" + qsTr(
                          " as backend")
                linkColor: Theme.highlightColor
                onLinkActivated: Qt.openUrlExternally(link)
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Based on ")+ "<a href=\"https://github.com/a-dekker/BarWal\">BarWal</a>"
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2*Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: qsTr("Thanks to Arno Dekker for the idea and inspiration")
            }

        }
    }
}
