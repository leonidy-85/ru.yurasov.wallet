import QtQuick 2.5
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import "../db.js" as DB

Dialog {
    id: addCodePage
    property string codeValue
    property string codeFormat : "EAN"
    property int barcode_index: 0

    property var dataModel: ListModel {}


    function readBarcodeList(name, linecount) {
        dataModel.append({
                             "Name": name,
                             "LineCount": linecount
                         })
        if (name === codeFormat) {
            barcode_index = linecount
        }
    }

    Component.onCompleted: {
        DB.readBarcodeList(1)
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
                                     "", code.text.trim(), '', 0, '')
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
//        contentHeight: col.height
        anchors.topMargin: 90
        clip: true



        ScrollDecorator {}

        Column {
            id: col
            spacing: 5
            width: parent.width
//            DialogHeader {
//                id: header
////                acceptText: qsTr("Create code")
//            }

            ComboBox {
                id: barcodetype
//                anchors.top: col.bottom
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

            TextField {
                id: name
                focus: true
                placeholderText: qsTr("Name")
                label: placeholderText
                width: parent.width
                text: ""
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: description.focus = true
            }

//            TextField {
//                id: description
//                placeholderText: qsTr("Description")
//                label: placeholderText
//                width: parent.width
//                text: ""
//                EnterKey.iconSource: "image://theme/icon-m-enter-next"
//                EnterKey.onClicked: code.focus = true
//            }

//            Component.onCompleted: {
//                console.log("Detected code format: " + codeFormat)
//                console.log(codeValue)
//            }

            TextField {
                id: code
                text: codeValue
                placeholderText: qsTr("Code")
                label: placeholderText
                width: parent.width
                EnterKey.onClicked: accept()
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
                    text: qsTr("Save")
                    onClicked: saveCard()
                 }


        }
    }
}
