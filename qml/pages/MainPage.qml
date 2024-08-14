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
        anchors.fill: parent
        spacing: 5
        anchors.topMargin: 120

        SilicaFlickable {
            width: parent.width
            height: barcodeFavorits.count<4 ? 260 : 450
            visible: barcodeFavorits.count>0
            SectionHeader {
                text: qsTr("Favorits")
                visible: isPortrait || (largeScreen && Screen.width > 1080)
            }
            SilicaGridView {
                id: barcodeFavorits
                cellWidth: width / 3
//                cellHeight: cellWidth
                cellHeight: 180
                anchors.fill: parent
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
                anchors.topMargin: 120
                model: ListModel {}
                delegate: ListItem {
                    id: gridItem
                    width: 200

                    Image {
                        id: iconImageGrid
                       // source: img_src(fav_barcode_icon,fav_zint_code)
                        source: fav_barcode_icon!=="" ? "data:image/png;base64," +fav_barcode_icon : "../icons/shablon.svg"
//                        source:   "../icons/shablon.svg"
                        sourceSize: Qt.size(Theme.itemSizeSmall,
                                            Theme.itemSizeSmall)
                        height: 140
                        width: parent.width
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.verticalCenter: parent.verticalCenter
                        smooth: false
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
                             anchors.leftMargin: 20
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
                            anchors.rightMargin: -210
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
                text: qsTr("All cards")
            }

            SilicaListView {
                id: barcodeList
                //            visible: mainapp.list=1
                anchors.fill: parent
                leftMargin: Theme.horizontalPageMargin
                rightMargin: Theme.horizontalPageMargin
                anchors.topMargin: 40

                VerticalScrollDecorator {}

                model: ListModel {}
                delegate: ListItem {
                    id: listItem

                    contentHeight: Theme.itemSizeSmall
                    Item {
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }
                        height: Theme.itemSizeSmall



                        Image {
                            id: iconImage
                            source: barcode_icon!=="" ?"data:image/png;base64," +barcode_icon : DB.img_src(barcode_icon,zint_code,0)
                            x: Theme.paddingLarge
                            sourceSize: Qt.size(Theme.itemSizeSmall, Theme.itemSizeSmall)
                            anchors.verticalCenter: parent.verticalCenter
                            width: 58
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                        }

                        Label {
                            id: barcode
                            text: barcode_name
                            anchors.left: iconImage.right
                            anchors.leftMargin: Theme.paddingMedium
                            anchors.verticalCenter: iconImage.verticalCenter // Центрируем Label по вертикали относительно Image
                            font.pixelSize: Theme.fontSizeMedium
                            color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                          //  visible: barcode_icon===""
                        }
                        //Label {
//                            id: descLabel
//                            anchors.bottom: barcode.top
//                            text: barcode_description
//                            height: parent.height
//                            font.pixelSize: Theme.fontSizeSmall
//                            color: listItem.highlighted ? Theme.highlightColor : Theme.secondaryColor
//                            anchors.leftMargin: Theme.paddingMedium
//                            anchors.left: iconImage.right
//                        }
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
                text: qsTr("No barcodes defined")
                hintText: qsTr("Choose \"Add code\" from the menu.")
            }
        }
    }
}