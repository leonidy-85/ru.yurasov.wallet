import QtQuick 2.6
import Sailfish.Silica 1.0
import QtMultimedia 5.6
import ru.yurasov.wallet.QmlImage 1.0
import ru.yurasov.wallet.imageProcessor 1.0
import ru.yurasov.wallet.imageConverter 1.0
import ru.yurasov.wallet.resizeImage 1.0

import "../db.js" as DB

Dialog {
    id: takePhotoPage
    anchors.fill: parent

    Item {
        anchors.fill: parent
        anchors.bottom: save.top

        Camera {
            id: camera
            objectName: "camera"
            captureMode: Camera.CaptureStillImage
            Component.onCompleted: cameraHelper.qcamera = camera
            focus {
                focusMode: CameraFocus.FocusContinuous
                focusPointMode: CameraFocus.FocusPointAuto
            }
        }

        VideoOutput {
            id: videoOutput
            objectName: "videoOutput"
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            source: camera

            Rectangle {
                anchors.fill: parent
                color: "transparent"

                Rectangle {
                    id: overlaySquare
                    //                    width: 530
                    //                    height: 320

                    width: videoOutput.width * 0.5 // 50% ширины
                    height: videoOutput.height * 0.3 // 30% высоты
                    color: Qt.rgba(1, 1, 1, 0)
                    border.color: Qt.rgba(0, 255, 51, 0.8)
                    border.width: 1

                    // Центрирование
                    x: (videoOutput.width - width) / 2
                    y: (videoOutput.height - height) / 2
                    //anchors.bottomMargin: height * 0.3
                    Canvas {
                        id: canvas
                        width: overlaySquare.width
                        height: overlaySquare.height

                        onPaint: {
                            var ctx = canvas.getContext("2d");
                            ctx.clearRect(0, 0, canvas.width, canvas.height);

                            ctx.drawImage(
                                        videoOutput,
                                        overlaySquare.x,
                                        overlaySquare.y,
                                        overlaySquare.width,
                                        overlaySquare.height,
                                        0,
                                        0,
                                        overlaySquare.width,
                                        overlaySquare.height
                                        );
                        }
                    }
                    onVisibleChanged: {
                        if (visible) {
                            canvas.requestPaint();
                        }
                    }
                }
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

        QmlImage {
            id: qmlImage
            objectName: "qmlImage"
            anchors {
                left: parent.left
                bottom: parent.bottom
            }
            //            width: Math.min(parent.width, parent.height) * 0.3
            //            height: Math.min(parent.width, parent.height) * 0.3

            Component.onCompleted: {
                cameraHelper.imageChanged.connect(function (newImage) {
                    setImage2(newImage);

                });
            }

            function setImage2(image) {
                //var scale_data= resizeImage.imageResize(image, overlaySquare.x, overlaySquare.y, overlaySquare.width, overlaySquare.height);
                mainapp.base64 = "data:image/png;base64," +imageProcessor.scaleImage(image, 530, 320);
                //console.log(overlaySquare.x, overlaySquare.y, overlaySquare.width, overlaySquare.height)
                  //iconImageGrid.source= mainapp.base64
                //setImage( scale_data );
                pageStack.pop();
            }
        }


//        Image {
//            id: iconImageGrid
//            source: "../icons/shablon.svg"
//            width: 300
//            height: implicitHeight
//              anchors.bottom: save.top
//            fillMode: Image.PreserveAspectFit
//            anchors.horizontalCenter: parent.horizontalCenter
//            smooth: false
//        }


        Button {
            id: save
            width: parent.width * 0.8
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 30
            text: qsTr("Take photo")
            onClicked: {

                cameraHelper.capture()

            }


        }


    }
}
