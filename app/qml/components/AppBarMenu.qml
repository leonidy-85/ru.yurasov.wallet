import QtQuick 2.0
import Sailfish.Silica 1.0
import Aurora.Controls 1.0

    AppBar {
        id: topAppBar

        headerText: qsTr("Wallet")


        headerClickable: false
        visible: opacity > 0
        Behavior on opacity { FadeAnimation {} }

               AppBarSpacer {}

               IconButton {
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
            IconButton {
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

//                          PopupMenuItem {
//                              text: qsTr("About")
//                              onClicked: pageStack.push(Qt.resolvedUrl("../pages/AboutPage.qml"))
//                          }

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
                AppBarSpacer {
                fixedWidth : 10
                }

          }


