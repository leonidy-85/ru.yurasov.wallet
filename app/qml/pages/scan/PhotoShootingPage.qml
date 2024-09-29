// SPDX-FileCopyrightText: Copyright 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.6
import QtMultimedia 5.6
import ru.yurasov.wallet.QmlImage 1.0
import ru.yurasov.wallet.imageProcessor 1.0
import ru.yurasov.wallet.imageConverter 1.0
import ru.yurasov.wallet.resizeImage 1.0

import Sailfish.Silica 1.0
import ru.yurasov.wallet 1.0

Dialog {
    id: photoShootingPage

    property var document
    property string source: ""
    property bool state: true

    objectName: "photoShootingPage"
    allowedOrientations: Orientation.Portrait
    forwardNavigation: !photoEditor.pointIsCaptured && photoRecognizer.isPhoto && !photoRecognizer.processing
    backNavigation: !photoEditor.pointIsCaptured
    showNavigationIndicator: !photoEditor.pointIsCaptured

//    Timer {
//         id: delayTimer
//         interval: 1000
//         repeat: true
//         onTriggered: {
//             var document_new =   imageConverter.imageToBase64(source)
//             if(document_new!=='error'){
//                mainapp.base64  =       "data:image/png;base64," +imageProcessor.scaleImage(document_new, 530, 320);
//                // var document_new =   "data:image/png;base64," + document_new
//               pageStack.pop();
//             }
//         }
//     }


    onAccepted: {
        var page = DSHelper.createPage();
        page.frame = photoAligner.frame;

        var needCreate = !document;
        if (needCreate) {
           document = DSHelper.createDocument();
        }

        document.insertPage(document.pagesCount, page);
        page.cache = false;


        if (needCreate){
         mainapp.base64  = Storage.insertDocument(Storage.documentsCount,  document);
         pageStack.pop();
   //  pageStack.replace(Qt.resolvedUrl("../EditBarcodePage.qml"), { source:  source })
        }
        else
            pageStack.pop();
    }

    DialogHeader {
        id: pageHeader

        objectName: "pageHeader"
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        cancelText: qsTr("Back")
        acceptText: qsTr("Accept")
        spacing: 0
        visible: opacity > 0.0
        opacity: !photoRecognizer.photoInProcess ? 1.0 : 0.0

        Behavior on opacity {
            objectName: "behaviorOnOpacity"

            FadeAnimation {
                objectName: "fadeAnimation"
                duration: 100
            }
        }
    }

    DSPhotoCamera {
        id: photoCamera
        objectName: "photoCamera"
    }

    Camera {
        id: camera
        objectName: "camera"
        Component.onCompleted: photoCamera.qmlCamera = camera
    }

    VideoOutput {
        id: videoOutputeObject
        objectName: "videoOutput"
        source: camera
        anchors {
            top: pageHeader.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        focus: visible // to receive focus and capture key events when visible
    }

    DSPhotoRecognizer {
        id: photoRecognizer

        property bool isPhoto: frameType === DSFrame.Photo
        property bool photoInProcess: isPhoto && processing

        objectName: "photoRecognizer"
        frame: photoCamera.frame
        active: true
    }

    DSPhotoAligner {
        id: photoAligner

        property bool isPhoto: frameType === DSFrame.Photo
        property bool photoInProcess: isPhoto && processing

        objectName: "photoAligner"
        frame: photoEditor.frame
        active: isPhoto
    }

    DSPhotoEditor {
        id: photoEditor

        objectName: "photoEditor"
        anchors {
            top: pageHeader.bottom
            left: parent.left
            right: parent.right
            bottom: videoOutputeObject.bottom
        }
        frame: photoRecognizer.frame
        overlayColor: Theme.rgba(Theme.darkPrimaryColor, Theme.opacityLow)
        pointsColor: Theme.lightPrimaryColor
        pointsHighlightedColor: Theme.highlightColor
        pointsVisible: !photoRecognizer.photoInProcess
        pointsManualMoving: photoRecognizer.isPhoto
        pointsRadius: 50 * Theme.pixelRatio
        zoomCircleHeight: 200 * Theme.pixelRatio
        zoomCircleWidth: 200 * Theme.pixelRatio
        Rectangle {
            x: photoEditor.imageFilledRect.x
            y: photoEditor.imageFilledRect.y
            width: photoEditor.imageFilledRect.width
            height: photoEditor.imageFilledRect.height
            color: Theme.rgba(Theme.highlightBackgroundColor, Theme.opacityOverlay)
            visible: opacity > 0.0
            opacity: photoRecognizer.photoInProcess ? 1.0 : 0.0

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
                        running: photoRecognizer.photoInProcess
                    }

                    Label {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        horizontalAlignment: Text.AlignHCenter
                        color: Theme.highlightColor
                        text: qsTr("Photo processing")
                    }
                }
            }
        }
    }

    Item {
        id: pageFooter

        objectName: "pageFooter"
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: shootBtn.height + Theme.horizontalPageMargin
        visible: opacity > 0.0
        opacity: !photoRecognizer.photoInProcess ? 1.0 : 0.0

        Behavior on opacity {
            objectName: "behaviorOnOpacity"

            FadeAnimation {
                objectName: "fadeAnimation"
                duration: 100
            }
        }

        Button {
            id: rotateLeftBtn

            objectName: "rotateLeftBtn"
            anchors {
                left: parent.left
                bottom: parent.bottom
                margins: Theme.horizontalPageMargin
            }
            width: height
            border {
                color: Theme.rgba(color, Theme.opacityFaint)
                highlightColor: Theme.rgba(highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            }
            icon.source: Qt.resolvedUrl("../../icons/icon-s-leftrotate.svg")
            visible: photoRecognizer.isPhoto && !photoRecognizer.processing

            onClicked: photoEditor.rotateLeft()
        }

        Button {
            id: rotateRightBtn

            objectName: "rotateRightBtn"
            anchors {
                right: parent.right
                bottom: parent.bottom
                margins: Theme.horizontalPageMargin
            }
            width: height
            border {
                color: Theme.rgba(color, Theme.opacityFaint)
                highlightColor: Theme.rgba(highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            }
            icon.source: Qt.resolvedUrl("../../icons/icon-s-rightrotate.svg")
            visible: photoRecognizer.isPhoto && !photoRecognizer.processing

            onClicked: photoEditor.rotateRight()
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


        Button {
            id: shootBtn

            objectName: "shootBtn"
            anchors {
                left: photoRecognizer.isPhoto ? rotateLeftBtn.right : parent.left
                right: photoRecognizer.isPhoto ? rotateRightBtn.left : parent.right
                bottom: parent.bottom
                margins: Theme.horizontalPageMargin
            }
            border {
                color: Theme.rgba(color, Theme.opacityFaint)
                highlightColor: Theme.rgba(highlightBackgroundColor, Theme.highlightBackgroundOpacity)
            }
            backgroundColor: photoRecognizer.isPhoto ? "transparent" : Theme.rgba(color, Theme.opacityFaint)
            text: photoRecognizer.isPhoto ? qsTr("Reshoot") : qsTr("Shoot")

            onClicked: photoRecognizer.isPhoto ? photoCamera.clearPhoto() : photoCamera.catchPhoto()
        }
//        Button {
//            id: shootBtn1

//            objectName: "shootBtn"

//            border {
//                color: Theme.rgba(color, Theme.opacityFaint)
//                highlightColor: Theme.rgba(highlightBackgroundColor, Theme.highlightBackgroundOpacity)
//            }
//            backgroundColor: "transparent"
//            text: qsTr("Save")

//            onClicked:{
//                var page = DSHelper.createPage();
//                page.frame = photoAligner.frame;

//                var needCreate = !document;
//                if (needCreate) {
//                   document = DSHelper.createDocument();
//                }

//                document.insertPage(document.pagesCount, page);
//                page.cache = false;


//              source = Storage.insertDocument(Storage.documentsCount,  document);
//              console.log('source= ', source )
//                      delayTimer.start()
//            }
//        }
    }
}
