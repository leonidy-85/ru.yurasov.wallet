import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    //allowedOrientations: Orientation.All

    property string jsondata: ''
    property string path: ''
    property string name: ''

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: pass.item.height + Theme.paddingLarge * 2

        Loader {
            id: pass
            width: Math.min(parent.width - 2 * Theme.horizontalPageMargin, Theme.fontSizeMedium * 20)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingLarge
            source: Qt.resolvedUrl("Back.qml")
            onLoaded: {
                item.jsondata = jsondata
            }
        }

        VerticalScrollDecorator {}
    }
}
