import QtQuick 2.2
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import Sailfish.Pickers 1.0
import ru.yurasov.wallet.FileIO 1.0
import ru.yurasov.wallet.imageProcessor 1.0
import ru.yurasov.wallet.imageConverter 1.0
import ru.yurasov.wallet.resizeImage 1.0
import "../db.js" as DB

Page {
    id: editCodePage

    property string barcode_name: ""
    property int barcode_id: 0
    property int barcode_index: 0

    property int barcode_clear: 0

    property string barcode_description: ""
    property string barcode_code: ""
    property string barcode_type: ""
    property string barcode_icon: ""

    property string source: ""

    property bool ready: false

    property var dataModel: ListModel {}

    property string mode: "import"
    property QtObject parentPage: null

    function fillNum(num) {
        if (num < 10) {
            return("0"+num);
        } else {
            return(num)
        }
    }


    Rectangle {
            id: overlay
            anchors.fill: parent
            color: Theme.rgba(Theme.highlightBackgroundColor, Theme.opacityOverlay)
            opacity: 1
            visible: ready

        Behavior on opacity {
            objectName: "behaviorOnOpacity"

            FadeAnimation {
                objectName: "fadeAnimation"
                duration: 100
            }
        }
    Item {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        height: column.height

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: Theme.paddingMedium

            BusyIndicator {
                size: BusyIndicatorSize.Large
                anchors.horizontalCenter: parent.horizontalCenter
                running: ready
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                visible: ready
                horizontalAlignment: Text.AlignHCenter
                color: Theme.highlightColor
                text: qsTr("Image loading...")
            }
        }
     }
    }

    Timer {
        id: waitTimer
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            if( mainapp.base64!==''){
                ready=true
                var document_new =   imageConverter.imageToBase64( mainapp.base64)
                if(document_new!=='error'){
                    barcode_icon =       "data:image/png;base64," +imageProcessor.scaleImage(document_new, 530, 320);
                    mainapp.base64=''
                    ready=false
                }
            }
              if(mainapp.scan!==""){
                    code.text = mainapp.scan
              }
        }
    }


    function checkFileName(file) {
        if (exportFile.exists(file)) {
            return(true)
        } else {
            DB.banner('Error', qsTr("Given file does not exist!"))
            return(false)
        }
    }

    function checkImageResolution(resolution) {

        var parts = resolution.split("x");
        if (parts.length === 2) {
            var width = parseInt(parts[0]);
            var height = parseInt(parts[1]);

            if(width > 650 || height > 400){
                //DB.banner('Error', qsTr("the image cannot be more than 650x400. Loaded image ")+width+"x"+height)
                return 0
            }else{
                return 1
            }
        }
    }


    function convertImageToBase64() {
        var base64String = imageConverter.imageToBase64(fileName.text);
        if (base64String.length > 0) {
            return base64String
        } else {
            //  DB.banner('Error', qsTr("Error loading image!"))
        }
    }

    function sendData(barcode_id,
                      name,
                      barcodetype,
                      description,
                      code) {
        var icon = null

        if(barcode_clear!==1 && fileName.text!==''){
            var _icon=  convertImageToBase64(fileName.text)
            var resolution=imageConverter.checkImageResolution(fileName.text);
            var check = checkImageResolution(resolution)
            if(check===0){
              icon =  "data:image/png;base64," +imageProcessor.scaleImage(_icon, 600, 380);
            }else{
             icon="data:image/png;base64," +_icon
            }
        }
        if(barcode_icon !== "" && fileName.text==='' && barcode_clear!==1){
            icon = barcode_icon
        }

        DB.updateBarcode(barcode_id,
                         name,
                         barcodetype,
                         description,
                         code,
                         icon
                         )
        save.text = qsTr("Update")
        pageStack.pop();
    }

    FileIO {
        id: exportFile
        source: fileName.text
        onError: { console.log(msg); }
    }


    function appendBarcodeFromList(name, linecount) {
        dataModel.append({
                             "Name": name,
                             "LineCount": linecount
                         })
        if (name === barcode_type) {
            barcode_index = linecount
        }
    }


    ImageConverter {
        id: imageConverter
    }

    ImageProcessor {
        id: imageProcessor
    }

    ResizeImage {
        id: resizeImage
    }

    Notification {
        id: notification
        itemCount: 1
    }

    Component.onCompleted: {
        if(mainapp.scan!==""){
          barcode_code=mainapp.scan
        }
        DB.readBarcodeList()
        barcodetype.currentIndex = barcode_index
        fileName.text= ""
    }

    SilicaFlickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: dialog.height
        clip: true

        Component {
            id: filePickerPage
            FilePickerPage {
                nameFilters: [ '*.png','*.jpg','*.svg' ]
                onSelectedContentPropertiesChanged: {
                    fileName.text = selectedContentProperties.filePath
                }
            }
        }

        Column {
            id: dialog
            spacing: 10
            width: parent.width

            PageHeader {
                objectName: "pageHeader"
                title: qsTr("Update barcode")
            }

            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }

            ComboBox {
                id: barcodetype
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
                //focus: true
                placeholderText: qsTr("Name")
                label: placeholderText
                text: barcode_name
                validator: RegExpValidator{ regExp: /^(?!\s*$).+/ }
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: code.focus = true
                EnterKey.enabled: !errorHighlight
                onErrorHighlightChanged: { save.enabled = !errorHighlight }
            }
            TextField {
                id: description
                placeholderText: qsTr("Additionally")
                label: placeholderText
                width: parent.width
                text: barcode_description
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: code.focus = true
            }
            Row {
                id: rowCode
                TextField {
                    id: code
                    text: barcode_code
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
                                              "type": 1
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

            SectionHeader {
                text: qsTr("Logo card")
            }
            Row {
                spacing: (width / 2) * 0.1
                width: parent.width
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                anchors.left: parent.left
                anchors.leftMargin: Theme.horizontalPageMargin
                Button {
                    id: removeButton
                    width: parent.width * .85 - barcodeIcon.width
                    text: qsTr("Remove icon")
                    visible: barcode_icon !== "" ||  fileName.text!==""
                    onClicked: {
                        barcodeIcon.source = "";
                        barcode_clear=1
                        barcode_icon = ""
                        fileName.text=""
                    }
                }
                Image {
                    id: barcodeIcon
                    source: barcode_icon !== "" && fileName.text==="" ?  barcode_icon  : fileName.text
                    sourceSize: Qt.size(Theme.itemSizeSmall,
                                        Theme.itemSizeSmall)
                    height: 50
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

                        Row {
                            IconButton {
                                id: buttonIcon
                                icon.source: "image://theme/icon-cover-camera?" + (pressed
                                             ? Theme.highlightColor
                                             : Theme.primaryColor)
                                visible: barcode_icon === "" &&  fileName.text===""
                                onClicked:  pageStack.push(Qt.resolvedUrl("../pages/scan/PhotoShootingPage.qml"))
                            }

                            Label{
                                id: labelIcon
                                text: qsTr("Press icon take photo")
                                verticalAlignment: Text.AlignVCenter
                                anchors.verticalCenter: buttonIcon.verticalCenter
                                font.pixelSize: Theme.fontSizeMedium
                                visible: barcode_icon === "" &&  fileName.text===""
                                color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                            }
                        }




            TextField {
                id: fileName
                width: parent.width
                text:  "";
                label: qsTr("Filename")
                visible: mode != "import"
                focus: true
                EnterKey.enabled: text.length > 0
            }

            ValueButton {
                width: parent.width
                label: qsTr("File to import")
                value: fileName.text? fileName.text: qsTr("None")
                onClicked: pageStack.push(filePickerPage)
            }

            Text {
                id: importIcon
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                width: parent.width - 2*Theme.paddingLarge
                wrapMode: Text.Wrap
                maximumLineCount: 15
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.secondaryColor
                text: qsTr("Here you can download the logo from the file, but not more than 650x400")
            }

            Separator {
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }

            Button {
                id: save
                width: parent.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Update")
                onClicked: {
                    sendData(barcode_id,
                             name.text.trim(),
                             barcodetype.currentItem.text,
                             description.text.trim(),
                             code.text.trim())
                }
            }
        }
    }
}