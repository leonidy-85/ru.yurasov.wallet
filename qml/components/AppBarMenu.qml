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

//               IconButton {
//                   icon.source: "../icons/grid_b.svg" + (pressed
//                                ? Theme.highlightColor
//                                : Theme.primaryColor)
//                   height: 64
//                   onClicked: mainapp.view=1
//               }
//             IconButton {
//                   icon.source: "../icons/list_b.svg" + (pressed
//                                ? Theme.highlightColor
//                                : Theme.primaryColor)
//                   height: 76
//                   onClicked: mainapp.view=0
//               }

                AppBarButton {
                  id: appBarMenuButton
                  icon.source: "image://theme/icon-m-menu"
                  onClicked: mainPopup.open()

                  PopupMenu {
                      id: mainPopup

                          PopupMenuItem {
                              text: qsTr("About")
                              onClicked: pageStack.push(Qt.resolvedUrl("../pages/About.qml"))
                          }

                          PopupMenuItem {
                              text: qsTr("Settings")
                              onClicked: pageStack.push(Qt.resolvedUrl("../pages/Settings.qml"))
                          }


                          PopupMenuItem {
                              text: qsTr("Add code")
//                              onClicked: pageStack.push(Qt.resolvedUrl("../pages/AddBarcodePage.qml"))
                              onClicked: pageStack.push(Qt.resolvedUrl("../pages/ScanBarcodePage.qml"))
                          }

                  }

              }
                AppBarSpacer {
                fixedWidth : 10
                }

          }


