// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.6
import Sailfish.Silica 1.0
import "Util.js" as Util

SilicaControl {
    id: root

    property int tabIndex: (model && model.index !== undefined) ? model.index : -1
    property int tabCount: parent && parent.tabCount || 0
    property bool isCurrentTab: _tabView && _tabView.currentIndex >= 0 && _tabView.currentIndex === tabIndex

    property alias title: titleLabel.text
    property alias icon: highlightImage

    property int titleFontSize: parent && parent.buttonFontSize || Theme.fontSizeLarge
    property int count

    property Item _tabView: parent && parent.tabView || null

    readonly property Item _page: _tabView ? _tabView._page : null
    readonly property bool _portrait: _page && _page.isPortrait

    readonly property Item _tabItem: _tabView ? (_tabView.exposedItems, _tabView.itemAt(tabIndex)) : null
    property alias contentItem: contentColumn
    property real _extraMargin: parent && parent.extraMargin || 0
    // contentWidth is used to calculate TabButtonRow width except of extraMargin
    property real contentWidth: 2 * Theme.paddingLarge + contentColumn.implicitWidth
                                + (bubble.active && highlightImage.width === 0 ? bubble.width : 0)
    implicitWidth: contentWidth
                + (root.tabIndex === 0 ? _extraMargin : 0)
                + (root.tabIndex === root.tabCount - 1 ? _extraMargin : 0)

    implicitHeight: Math.max(_portrait ? Theme.itemSizeLarge : Theme.itemSizeSmall,
                             contentColumn.implicitHeight + 2 * (_portrait ? Theme.paddingLarge : Theme.paddingMedium))

    highlighted: mouseArea.pressed && mouseArea.containsMouse

    MouseArea {
        id: mouseArea

        anchors.fill: parent


        onClicked: {
            if (root._tabView && root.tabIndex >= 0) {
                root._tabView.moveTo(root.tabIndex)
            }
        }
    }

    Column {
        id: contentColumn

        x: {
            if (root.tabCount > 1 && root.tabIndex === 0) {
                return root.width - width - Theme.paddingLarge;
            } else if (root.tabCount > 1 && root.tabIndex == root.tabCount - 1) {
                return Theme.paddingLarge;
            } else {
                return ((root.width - width) / 2) - (highlightImage.status === Image.Ready ? bubble.width * 0.5 : 0);
            }
        }

        y: (root.height - height) / 2

        HighlightImage {
            id: highlightImage

            anchors.horizontalCenter: parent.horizontalCenter
            highlighted: root.highlighted || root.isCurrentTab
        }

        Label {
            id: titleLabel

            x: (contentColumn.width - width) / 2
            color: highlightImage.highlighted ? palette.highlightColor : palette.primaryColor
            font.pixelSize: highlightImage.status === Image.Ready ? Theme.fontSizeTiny : root.titleFontSize
        }
    }

    Loader {
        id: bubble

        x: highlightImage.status === Image.Ready
                ? (contentColumn.width - width + highlightImage.width)/1.2
                : contentColumn.x + contentColumn.width + Theme.dp(4)
        y: Theme.paddingLarge
        active: root.count > 0
        asynchronous: true
        opacity: root.highlighted ? 0.8 : 1.0

        sourceComponent: Component {
            Rectangle {
                color: Theme.highlightBackgroundColor
                width: bubbleLabel.text ? Math.max(bubbleLabel.implicitWidth + Theme.paddingSmall * 2, height) : Theme.paddingMedium + Theme.paddingSmall
                height: bubbleLabel.text ? bubbleLabel.implicitHeight : Theme.paddingMedium + Theme.paddingSmall
                radius: Theme.dp(2)

                Label {
                    id: bubbleLabel

                    text: {
                        if (root.count < 0) {
                            return "";
                        } else if (root.count > 99) {
                            return "99+";
                        } else {
                            return root.count;
                        }
                    }

                    anchors.centerIn: parent
                    font.pixelSize: Theme.fontSizeTiny
                    font.bold: true
                }
            }
        }
    }
}
