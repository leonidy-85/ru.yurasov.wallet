import QtQuick 2.6
import Sailfish.Silica 1.0
import "../tabview" as Tabs
import "../components"

Page {
    id: tabMainPage
    objectName: "mainPage"
    allowedOrientations: Orientation.All

    AppBarMenu {}

    property bool footerPosition: true // set here true of false to change tabs postions from top to bottom
    property bool noTitle
    property bool noIcon
    property int display: 8

    Tabs.TabView {
        id: tabs

        property var _viewModel: [walletPage, pkpassPage, aboutPage]
        width: parent.width
        height: tabMainPage.height

        header: tabMainPage.footerPosition ? null : tabBar
        footer: tabMainPage.footerPosition ? tabBar : null

        model: _viewModel.slice(0, tabMainPage.display)

        Component {
            id: tabBar
            Tabs.TabBar {
                model: tabModel
            }
        }

        Component {
            id: walletPage
            WalletPage {
            }
        }

        Component {
            id: pkpassPage
            PkpassPage {
            }
        }
        Component {
            id: aboutPage
            AboutPage {
            }
        }

    }


    Component.onCompleted: {
        prepareModel()
    }

    function prepareModel() {
        if (tabMainPage.display < tabModel.count) {
            tabModel.remove(tabMainPage.display, tabModel.count - tabMainPage.display)
        }
        for (var i = 0; i < tabModel.count; i++) {
            if (tabMainPage.noTitle) {
                tabModel.setProperty(i, "title", "")
            }
            if (tabMainPage.noIcon) {
                tabModel.setProperty(i, "icon", "")
            }
        }
    }

    SilicaGridView {
        model: ListModel {
            id: tabModel

            ListElement {
                title: qsTr("Wallet")
                icon: "../icons/wallet.png"
                count: 0
            }
            ListElement {
                title: qsTr("Pkpass")
                icon: "../icons/pkpass.png"
                count: 0
            }

            ListElement {
                title: qsTr("About")
                icon: "../icons/about.png"
                count: 0
            }
        }
    }
}
