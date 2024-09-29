import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: body.height + 2 * Theme.paddingLarge

        Column {
            id: body
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.rightMargin: Theme.horizontalPageMargin
            anchors.topMargin: Theme.paddingLarge
            spacing: Theme.paddingMedium

            Label {
                anchors.right: parent.right
                text: qsTr("Time")
                font.family: Theme.fontFamilyHeading
                color: Theme.highlightColor
            }

            TextSwitch {
                id: time
                text: qsTr("Highlight passes close to event time")
                checked: settingsStore.checkTime
                onCheckedChanged: settingsStore.checkTime = checked
            }

            Slider {
                width: parent.width
                enabled: time.checked
                visible: time.checked
                stepSize: 1
                minimumValue: 0
                maximumValue: 8
                value: settingsStore.hoursBefore
                valueText: value + "h"
                label: qsTr("Time before event")
                onDownChanged: if (!down) settingsStore.hoursBefore = value
            }

            Slider {
                width: parent.width
                enabled: time.checked
                visible: time.checked
                stepSize: 1
                minimumValue: 0
                maximumValue: 8
                value: settingsStore.hoursAfter
                valueText: value + "h"
                label: qsTr("Time after event")
                onDownChanged: if (!down) settingsStore.hoursAfter = value
            }

            Label {
                anchors.right: parent.right
                text: qsTr("Distance")
                font.family: Theme.fontFamilyHeading
                color: Theme.highlightColor
            }

            TextSwitch {
                id: distance
                text: qsTr("Highlight passes close to destination")
                checked: settingsStore.checkDistance
                onCheckedChanged: settingsStore.checkDistance = checked
            }

            TextSwitch {
                automaticCheck: distance.checked
                visible: distance.checked
                text: qsTr("Prefer non-satellite position fixing")
                description: qsTr("Saves battery. <b>Requires</b> &quot;Faster position fix&quot; switched on in the system settings.")
                checked: settingsStore.useHere
                onCheckedChanged: settingsStore.useHere = checked
            }

            Slider {
                width: parent.width
                enabled: distance.checked
                visible: distance.checked
                stepSize: 50
                minimumValue: 100
                maximumValue: 2000
                value: settingsStore.maxDistance
                valueText: value + "m"
                label: qsTr("Distance to destination")
                onDownChanged: if (!down) settingsStore.maxDistance = value
            }

            TextSwitch {
                automaticCheck: distance.checked
                visible: distance.checked
                text: qsTr("Allow passes to override distance")
                checked: settingsStore.overrideDistance
                onCheckedChanged: settingsStore.overrideDistance = checked
            }

            Label {
                anchors.right: parent.right
                text: qsTr("Barcode")
                font.family: Theme.fontFamilyHeading
                color: Theme.highlightColor
            }

            TextSwitch {
                text: qsTr("Show barcode fullscreen when tapped")
                checked: settingsStore.barcodeTap
                onCheckedChanged: settingsStore.barcodeTap = checked
            }

        }

        VerticalScrollDecorator {}
    }
}
