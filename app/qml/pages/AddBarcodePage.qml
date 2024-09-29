import QtQuick 2.5
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import Aurora.Controls 1.0

import "../db.js" as DB

Page {
    id: addCodePage
    property string codeValue
    property string barcode_name : ""
    property string barcode_type : ""
    property string barcode_icon : ""
    property string codeFormat : ""
    //   property string barcode_icon : ""

    property int barcode_index: 0

    property int type: 0

    property var dataModel: ListModel {}


    function readBarcodeList(name, linecount) {
        dataModel.append({
                             "Name": name,
                             "LineCount": linecount
                         })

        if (name === barcode_type) {
            barcode_index = linecount
        }
    }



    Timer {
        id: waitTimer
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            if(mainapp.scan!==""){
                barcode_type=mainapp.type
                dataModel.clear();
                DB.readBarcodeList(1)
                code.text = mainapp.scan
                if(type===0){
                    barcodetype.currentIndex = barcode_index
                }
                mainapp.type=""
                mainapp.scan=""
            }
        }
    }
    Component.onCompleted: {
        DB.readBarcodeList(1)
        name.text=barcode_name
        barcodetype.currentIndex = barcode_index
    }

    Notification {
        id: notification
        itemCount: 1
    }

    function saveCard() {
        if(name.text.trim()===""){
            DB.banner("ERROR", qsTr("the name field cannot be empty!"))
            return
        }
        if(code.text.trim()===""){
            DB.banner("ERROR", qsTr("the code field cannot be empty!"))
            return
        }
        var result = DB.writeBarcode(name.text.trim(),
                                     barcodetype.currentItem.text,
                                     description.text.trim(), code.text.trim(), barcode_icon, 0, '')
        if (result === "ERROR") {
            DB.banner("ERROR", qsTr("Could not add barcode!"))
            return
        }
        DB.banner("Ok", qsTr("barcode successfully added!"))
        pageStack.pop();


        mainapp.barcodesChanged = true
    }

    PageHeader {
        objectName: "pageHeader"
        title: qsTr("Add card")
    }

    SilicaFlickable {
        anchors.fill: parent
        contentWidth: parent.width
        anchors.topMargin: 90
        clip: true

        ScrollDecorator {}

        Column {
            id: col
            spacing: 5
            width: parent.width

            ComboBox {
                id: barcodetype
                anchors.topMargin: 40
                label: qsTr("Barcode type")
                menu: ContextMenu {
                    Repeater {
                        model: dataModel
                        MenuItem {
                            text: model.Name
                        }
                    }
                }
            }

            Image {
                id: barcodeIcon
                source: barcode_icon !== "" ?  barcode_icon  : "../icons/shablon.svg"
                sourceSize: Qt.size(Theme.itemSizeSmall,
                                    Theme.itemSizeSmall)
                width: 120
                height: 80
                anchors.leftMargin: 20
                anchors.topMargin: 40
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TextField {
                id: name
                //focus: true
                placeholderText: qsTr("Name")
                label: type ===1 ? "" : placeholderText
                width: parent.width
                validator: RegExpValidator{ regExp: /^(?!\s*$).+/ }
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: code.focus = true
                EnterKey.enabled: !errorHighlight
                onErrorHighlightChanged: { saveButton.enabled = !errorHighlight }
            }

            TextField {
                id: description
                placeholderText: qsTr("Additionally")
                label: placeholderText
                width: parent.width
                text: ""
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: code.focus = true
            }


            Row {
                id: rowCode
                TextField {
                    id: code
                    text: ""
                    placeholderText: qsTr("Code")
                    label: placeholderText
                    width: parent.width*0.85
                    EnterKey.onClicked: accept()
                }

                IconButton {
                    icon.source: "image://theme/icon-cover-camera?" + (pressed
                                                                       ? Theme.highlightColor
                                                                       : Theme.primaryColor)
                    icon.width: 50
                    icon.height: 50
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("../pages/ScanBarcodePage.qml"),{
                                           "type": type,
                                           //                                              "barcode_name" : barcode_name,
                                           //                                              "barcode_type" : barcode_type,
                                           //                                              "barcode_icon" : barcode_icon
                                       })
                    }
                }
            }


            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }

            Button {
                id: saveButton
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                // anchors.top: rowCode.bottom
                // anchors.topMargin: 80
                text: qsTr("Save")
                onClicked: saveCard()
            }
        }
    }
}
