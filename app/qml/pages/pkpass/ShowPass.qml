import QtQuick 2.0
import Sailfish.Silica 1.0
import Aurora.Controls 1.0

Page {
    id: page
    //allowedOrientations: Orientation.All

    property string name: ''
    property string jsondata: ''
    property string path: ''
    property int barcodeType: 13
    property bool updateable: false

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: pass.item.height + Theme.paddingLarge * 2

        AppBar {
            id: topAppBar

            headerText: qsTr("Full View")


            headerClickable: false
            visible: opacity > 0
            Behavior on opacity { FadeAnimation {} }

                   AppBarSpacer {}

                   IconButton {
                       icon.source: "image://theme/icon-m-date?" + (pressed
                                    ? Theme.highlightColor
                                    : Theme.primaryColor)
                       icon.width: 50
                       icon.height: 50
                       enabled: pass.item.relevantDate !== ""
                       onClicked: {
                           passHandler.createCalendarEntry(name, pass.item.relevantDate);
                          }
                   }

                IconButton {
                       icon.source: "image://theme/icon-m-note?" + (pressed
                                    ? Theme.highlightColor
                                    : Theme.primaryColor)
                       icon.width: 50
                       icon.height: 50
                       onClicked: {
                           var properties = { path: pass.item.path, jsondata: pass.item.jsondata };
                           pageStack.push(Qt.resolvedUrl("ShowSimple.qml"), properties);

                          }
                   }
                IconButton {
                       icon.source: "image://theme/icon-m-refresh?" + (pressed
                                    ? Theme.highlightColor
                                    : Theme.primaryColor)
                       icon.width: 50
                       icon.height: 50
                       enabled: updateable
                       onClicked: {
                          passHandler.updatePass(page.path);
                          }
                   }


                    AppBarSpacer {
                    fixedWidth : 10
                    }

              }

        Loader {
            id: pass
            width: Math.min(parent.width - 2 * Theme.horizontalPageMargin, Theme.fontSizeMedium * 20)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
             anchors.topMargin: Theme.fontSizeExtraLarge * 3.2
            source: Qt.resolvedUrl("Pass.qml")
            onLoaded: {
                item.path = path
                item.jsondata = jsondata
            }
        }

        VerticalScrollDecorator {}
    }
}
