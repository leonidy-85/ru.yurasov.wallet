import QtQuick 2.2
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0
import Sailfish.Pickers 1.0
import ru.yurasov.wallet.FileIO 1.0
import ru.yurasov.wallet.imageConverter 1.0
import "../db.js" as DB

Page {
    id: wizardPage

    Component.onCompleted: {
        dataModel.model.clear()
        DB.marketList('')

    }

    function readMarketList(name, icon, type) {
        dataModel.model.append({
                                   "barcode_name": name,
                                   "barcode_type": type,
                                   "barcode_icon": icon
                               })
    }
    Column {
        id: col_first
        anchors.fill: parent
        anchors.topMargin: 20

        SilicaFlickable {
            anchors.fill: parent

            PageHeader {
                objectName: "pageHeader"
                title: qsTr("Wizard add card")
            }

            Separator {
                id: separator
                color: Theme.primaryColor
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Qt.AlignHCenter
            }

            TextField {
                id: searchField
                placeholderText: qsTr("Search...")
                // focus: true
                anchors.top: separator.bottom
                anchors.topMargin: 150
                anchors.horizontalCenter: parent.horizontalCenter
                onTextChanged: {
                    dataModel.model.clear()
                    var searchTerm = "%" + text + "%";
                    DB.marketList(searchTerm);
                }
            }

            SilicaListView {
                id: defaultCol
                anchors.top: searchField.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 20
                model: ListModel {
                    ListElement {
                        name: "Добавить вручную"
                        image: "../icons/shablon.svg" }

                }
                delegate: ListItem {

                    contentHeight: Theme.itemSizeSmall
                    Item {
                        anchors.verticalCenter: parent.verticalCenter
                        height: Theme.itemSizeSmall


                        Row {
                            spacing: Theme.paddingMedium

                            Rectangle {
                                id: imgrad2
                                implicitWidth: 100
                                implicitHeight: 60
                                color: "transparent"

                                Image {
                                    id: iconImage2
                                    source: image
                                    visible: false
                                    fillMode: Image.PreserveAspectCrop
                                    onStatusChanged: {
                                        if (status === Image.Ready) {
                                            canvas3.requestPaint();
                                        }
                                    }
                                }
                                Canvas {
                                    id: canvas3
                                    width: 100
                                    height: 80
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
                                        if (iconImage2.width > 0 && iconImage2.height > 0) {
                                            ctx.drawImage(iconImage2, 0, 0, width, height);
                                        }
                                    }
                                }
                            }
                            Label {
                                id: barcode2
                                text: name
                                verticalAlignment: Text.AlignVCenter
                                anchors.verticalCenter: imgrad.verticalCenter
                                font.pixelSize: Theme.fontSizeMedium
                                color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                                anchors.leftMargin: 10
                            }
                        }
                    }
                    onClicked: {
                        pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {
                                              "barcode_name":  "",
                                              "barcode_type": "EAN",
                                              "barcode_icon": "",
                                              "type": 0
                                          })
                    }
                 }
                }

                SilicaListView {
                    id: dataModel
                    anchors.top: defaultCol.bottom
                    anchors.topMargin: 100
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    VerticalScrollDecorator {}

                    model: ListModel {}
                    delegate: ListItem {
                        id: listItem

                        contentHeight: Theme.itemSizeSmall
                        Item {
                            anchors.verticalCenter: parent.verticalCenter
                            height: Theme.itemSizeSmall

                            Row {
                                spacing: Theme.paddingMedium

                                Rectangle {
                                    id: imgrad
                                    implicitWidth: 120
                                    implicitHeight: 80
                                    color: "transparent"

                                    Image {
                                        id: iconImage
                                        source: barcode_icon !== "" ? barcode_icon : ""
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
                                        width: 120
                                        height: 80
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
                                    anchors.leftMargin: 10
                                }
                            }
                        }
                        onClicked: {
                            pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {
                                                  "barcode_name":  dataModel.model.get(
                                                                       index).barcode_name,
                                                  "barcode_type":  dataModel.model.get(
                                                                       index).barcode_type,
                                                  "barcode_icon":  dataModel.model.get(
                                                                       index).barcode_icon,
                                                  "type": 1
                                 })
                        }
                    }
                }
            }
        }
    }

