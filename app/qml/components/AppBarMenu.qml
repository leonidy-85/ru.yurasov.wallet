import QtQuick 2.0
import Sailfish.Silica 1.0
import Aurora.Controls 1.0

AppBar {
    id: appBarMenu
    iconContainer{
        imageItem.source:  "../icons/icon.png"
        imageItem.height: Theme.fontSizeExtraLarge
        imageItem.width: Theme.fontSizeExtraLarge

    }
    headerText: qsTr("Wallet")
    headerClickable: false
    AppBarSpacer {}
    AppBarButton {
        icon.source: "image://theme/icon-m-search?" + (pressed
                                                       ? Theme.highlightColor
                                                       : Theme.primaryColor)
        icon.width: 50
        icon.height: 50
        onClicked: {
            if( mainapp.search===true){
                mainapp.search=false
                searchField.text = ""
                barcodeList.model.clear()
                DB.readBarcodes(mainapp.searchTerm)
            }else{
                mainapp.search=true

            }
        }
    }
    AppBarButton {
        icon.source: "image://theme/icon-m-add?" + (pressed
                                                    ? Theme.highlightColor
                                                    : Theme.primaryColor)
        icon.width: 50
        icon.height: 50
        onClicked: {
            onClicked: pageStack.push(Qt.resolvedUrl("../pages/WizardPage.qml"))
        }
    }
    AppBarButton {
        id: appBarMenuButton
        icon.source: "image://theme/icon-m-menu"
        onClicked: mainPopup.open()
        PopupMenu {
            id: mainPopup
            PopupMenuItem {
                text: qsTr("Backup")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/BackupPage.qml"))
            }
            PopupMenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("../pages/SettingsPage.qml"))
            }
        }
    }
}


