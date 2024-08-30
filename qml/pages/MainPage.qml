import QtQuick 2.5
import Sailfish.Silica 1.0
import "../db.js" as DB
import "../components"

Page {
    id: mainPage

    AppBarMenu {}

    function appendBarcode(id, name, type, description, code, zintcode, icon, sel) {
        barcodeList.model.append({
                                     "barcode_id": id,
                                     "barcode_name": name,
                                     "barcode_type": type,
                                     "barcode_description": description,
                                     "barcode_code": code,
                                     "zint_code": zintcode,
                                     "barcode_icon": icon,
                                     "barcode_sel": sel,
                                 })

    }
    function appendBarcodeFavorits(id, name, type, description, code, zintcode, icon) {
        barcodeFavorits.model.append({
                                         "fav_barcode_id": id,
                                         "fav_barcode_name": name,
                                         "fav_barcode_type": type,
                                         "fav_barcode_description": description,
                                         "fav_barcode_code": code,
                                         "fav_zint_code": zintcode,
                                         "fav_barcode_icon": icon,
                                     })

    }



    Timer {
        id: waitTimer
        interval: 500
        repeat: true
        running: true
        onTriggered: {
            if(mainapp.barcodeUpdate!==false){

                barcodeList.model.clear()
                barcodeFavorits.model.clear()
                DB.readBarcodes()
                DB.readFavorites()
                mainapp.barcodeUpdate=false
            }
        }
    }


    Component.onCompleted: {
        DB.initializeDB()
        mainapp.barcodeUpdate=true
        barcodeList.model.clear()
        barcodeFavorits.model.clear()
        DB.readBarcodes()
        DB.readFavorites()
        mainapp.iconSource=""
    }

    Column {
        id: col_first
        anchors.fill: parent
        anchors.topMargin: 120

        SilicaFlickable {
            width: parent.width
            height: barcodeFavorits.count<4 ? 280 : 450
            visible: barcodeFavorits.count>0


            TextField {
                id: searchField
                placeholderText:qsTr("Search...")
                focus: mainapp.search ? true : false
                width: parent.width*0.8
                visible : mainapp.search
                onTextChanged: {
                    barcodeList.model.clear()
                    barcodeList.anchors.topMargin=150
                    var searchTerm = "%" + text + "%";
                    DB.readBarcodes(searchTerm)
                }
            }


            SectionHeader {
                text: qsTr("Favorits")
                visible : !mainapp.search
                font.pixelSize: Theme.fontSizeMedium
                // visible: isPortrait || (largeScreen && Screen.width > 1080)
            }
            SilicaGridView {
                id: barcodeFavorits
                cellWidth: width / 3
                cellHeight: 160
                anchors.fill: parent
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
                anchors.topMargin: 120
                anchors.leftMargin: parent.width  * 0.033
                model: ListModel {}
                delegate: ListItem {
                    id: gridItem
                    width: parent.width  * 0.3

                    Rectangle {
                        anchors.fill: parent
                        color: "transparent"

                        Image {
                            id: imageElement
                            source: fav_barcode_icon !== "" ?  fav_barcode_icon : "../icons/shablon.svg"
                            visible: false
                            fillMode: Image.PreserveAspectCrop
                            onStatusChanged: {
                                if (status === Image.Ready) {
                                    canvas.requestPaint();
                                }
                            }
                        }

                        Canvas {
                            id: canvas
                            height: 140
                            width: parent.width
                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.clearRect(0, 0, width, height);
                                ctx.beginPath();
                                ctx.moveTo(20, 0);
                                ctx.lineTo(width - 20, 0);
                                ctx.quadraticCurveTo(width, 0, width, 20);
                                ctx.lineTo(width, height - 20);
                                ctx.quadraticCurveTo(width, height, width - 20, height);
                                ctx.lineTo(20, height);
                                ctx.quadraticCurveTo(0, height, 0, height - 20);
                                ctx.lineTo(0, 20);
                                ctx.quadraticCurveTo(0, 0, 20, 0);
                                ctx.closePath();
                                ctx.clip();
                                if (imageElement.width > 0 && imageElement.height > 0) {
                                    ctx.drawImage(imageElement, 0, 0, width, height);
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onPressAndHold: menu2.active ? menu2.hide() : menu2.open(gridItem)
                                onClicked: {
                                    pageStack.push(Qt.resolvedUrl("BarcodeDisplayPage.qml"), {
                                                       "barcode_name":  barcodeFavorits.model.get(
                                                                            index).fav_barcode_name,
                                                       "zint_code":  barcodeFavorits.model.get(
                                                                         index).fav_zint_code,
                                                       "barcode_description":  barcodeFavorits.model.get(
                                                                                   index).fav_barcode_description,
                                                       "barcode_icon":  barcodeFavorits.model.get(
                                                                            index).fav_barcode_icon,
                                                       "barcode_code":  barcodeFavorits.model.get(
                                                                            index).fav_barcode_code
                                                   })
                                }
                            }
                        }
                    }

                    menu: ContextMenu {
                        id: menu2
                        MenuItem {
                            text: qsTr("Remove ") +" "+ fav_barcode_name + qsTr(" from favorites")
                            onClicked:  DB.changeFavorites(fav_barcode_id,0)
                        }
                    }


                    Item {
                        Image {
                            id: iconImageIcon
                            source: DB.img_src(fav_barcode_icon, fav_zint_code, 1)
                            sourceSize: Qt.size(Theme.itemSizeSmall,
                                                Theme.itemSizeSmall)
                            width: 58
                            fillMode: Image.PreserveAspectFit
                            anchors.left: parent.left
                            anchors.leftMargin: parent.width  * 0.33
                            visible: fav_barcode_icon===""
                            smooth: true
                        }
                        Label {
                            id: barcodeGrid
                            text: fav_barcode_name
                            height: 40
                            visible: fav_barcode_icon===""
                            anchors.top: iconImageIcon.bottom
                            font.pixelSize: Theme.fontSizeSmall
                            color:  Theme.highlightDimmerColor
                            elide: Text.ElideRight
                            anchors.right: parent.right
                            anchors.rightMargin: -190
                        }

                    }

                }
            }
        }


        Separator {
            color: Theme.primaryColor
            width: parent.width
            visible:  barcodeFavorits.count>0
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
        }

        SilicaFlickable {
            width: parent.width
            height:   barcodeFavorits.count>0 ? barcodeFavorits.count>0 && barcodeFavorits.count<4 ? 900 : 700 : 1200

            SectionHeader {
                id: allLabel
                text: qsTr("All cards")
                font.pixelSize: Theme.fontSizeMedium
            }



            SilicaListView {
                id: barcodeList
                anchors.fill: parent
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
                anchors.topMargin: 90

                VerticalScrollDecorator {}

                model: ListModel {}
                delegate: ListItem {
                    id: listItem

                    contentHeight: Theme.itemSizeSmall
                    Item {
                        anchors.verticalCenter: parent.verticalCenter
                        height: Theme.itemSizeSmall

                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: Theme.paddingMedium // Расстояние между изображением и текстом

                            Rectangle {
                                id: imgrad
                                implicitWidth: 100 // Установите нужную ширину
                                implicitHeight: 80 // Установите нужную высоту
                                color: "transparent"

                                Image {
                                    id: iconImage
                                    source: barcode_icon !== "" ?  barcode_icon : DB.img_src(barcode_icon, zint_code, 0)
                                    visible: false
                                    fillMode: Image.PreserveAspectCrop
                                    onStatusChanged: {
                                        if (status === Image.Ready) {
                                            canvas2.requestPaint();
                                        }
                                    }
                                }

                                Canvas {
                                    id: canvas2
                                    width: 100
                                    height: 80

                                    onPaint: {
                                        var ctx = getContext("2d");
                                        ctx.clearRect(0, 0, width, height);
                                        ctx.clearRect(0, 0, width, height);
                                        ctx.beginPath();
                                        ctx.moveTo(20, 0);
                                        ctx.lineTo(width - 20, 0);
                                        ctx.quadraticCurveTo(width, 0, width, 20);
                                        ctx.lineTo(width, height - 20);
                                        ctx.quadraticCurveTo(width, height, width - 20, height);
                                        ctx.lineTo(20, height);
                                        ctx.quadraticCurveTo(0, height, 0, height - 20);
                                        ctx.lineTo(0, 20);
                                        ctx.quadraticCurveTo(0, 0, 20, 0);
                                        ctx.closePath();
                                        ctx.clip();
                                        if (iconImage.width > 0 && iconImage.height > 0) {
                                            ctx.drawImage(iconImage, 0, 0, width, height);
                                        }
                                    }
                                }
                            }

                            Label {
                                id: barcode
                                text: barcode_name
                                verticalAlignment: Text.AlignVCenter
                                anchors.verticalCenter: imgrad.verticalCenter
                                font.pixelSize: Theme.fontSizeMedium
                                color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                            }
                        }


                    }
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("BarcodeDisplayPage.qml"), {
                                           "barcode_name":  barcodeList.model.get(
                                                                index).barcode_name,
                                           "zint_code":  barcodeList.model.get(
                                                             index).zint_code,
                                           "barcode_description":  barcodeList.model.get(
                                                                       index).barcode_description,
                                           "barcode_icon":  barcodeList.model.get(
                                                                index).barcode_icon,
                                           "barcode_code":  barcodeList.model.get(
                                                                index).barcode_code
                                       })
                    }

                    onPressAndHold: menu.active ? menu.hide() : menu.open(listItem)

                    menu: ContextMenu {
                        id: menu

                        MenuItem {
                            text: qsTr("Edit")
                            onClicked: pageStack.push(Qt.resolvedUrl(
                                                          "EditBarcodePage.qml"), {
                                                          "barcode_id": barcodeList.model.get(
                                                                            index).barcode_id,
                                                          "barcode_name": barcodeList.model.get(
                                                                              index).barcode_name,
                                                          "barcode_description": barcodeList.model.get(
                                                                                     index).barcode_description,
                                                          "barcode_type": barcodeList.model.get(
                                                                              index).barcode_type,
                                                          "barcode_code": barcodeList.model.get(
                                                                              index).barcode_code,
                                                          "barcode_icon": barcodeList.model.get(
                                                                              index).barcode_icon
                                                      })
                        }
                        MenuItem {
                            visible: barcode_sel!==1
                            text: qsTr("Add ")+ barcode_name + qsTr(" to favorites")
                            onClicked:  DB.changeFavorites(barcode_id,1)

                        }
                        MenuItem {
                            text: qsTr("Remove ") +" "+ barcode_name
                            onClicked: Remorse.itemAction(listItem, qsTr("Deleting ") +barcode_name,
                                                          function () {
                                                              DB.removeBarcode(barcode_id)
                                                          })
                        }
                    }
                }

            }




            ViewPlaceholder {
                enabled: barcodeList.count === 0
                Column {
                    anchors.centerIn: parent // Центрируем Column внутри ViewPlaceholder

                    Text {
                        text: mainapp.search ? qsTr("No card with this name found") : qsTr("No barcodes defined")
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: Theme.secondaryHighlightColor
                        font.pixelSize: Theme.fontSizeExtraLarge
                        wrapMode: Text.Wrap
                    }

                    Text {
                       // id: textPlaceholder
                        maximumLineCount: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: Theme.fontSizeMedium
                        color: Theme.secondaryColor
                        width: parent.width - Theme.paddingLarge
                        wrapMode: Text.Wrap
                        text: !mainapp.search ? qsTr("Choose \"Add card\" from the menu or press of plus.") : ""
                    }


                    SectionHeader {
                        text: ""
                    }
                    Rectangle {
                        visible:  barcodeList.count==0 && !mainapp.search
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 300
                        height: 150
                        radius: 20
                        color: Qt.rgba(1, 1, 1, 0)
                        border.color: "white"
                        border.width: 1
                        IconButton {
                            icon.source: "image://theme/icon-m-add?" + (pressed
                                         ? Theme.highlightColor
                                         : Theme.primaryColor)

                            anchors.centerIn: parent
                            onClicked: pageStack.push(Qt.resolvedUrl("../pages/WizardPage.qml"))
                        }
                    }
                }
            }



        }
    }
}
