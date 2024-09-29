// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.4
import Sailfish.Silica 1.0
import "Util.js" as Util

PagedView {
    id: root

    property Component header
    property Component footer
    property bool hasFooter: footer
    property alias tabBarItem: tabBarLoader.item
    property real tabBarHeight: tabBarItem ? tabBarItem.height : 0

    property string sourceProperty: "modelData"

    property real yOffset: currentItem && currentItem._yOffset || 0
    property alias _headerBackgroundVisible: backgroundRectangle.visible

    property Item _page: Util.findPage(root)

    property int __silica_tab_view

    verticalAlignment: hasFooter ? PagedView.AlignTop : PagedView.AlignBottom
    cacheSize: 0

    contentItem {
        y: root.hasFooter ? 0 : tabBarLoader.height
        height: root.height - tabBarLoader.height
    }

    Loader {
        id: tabBarLoader

        sourceComponent: root.hasFooter ? root.footer : root.header
        width: parent.width
        z: root.yOffset < 0 && !root.hasFooter ? -1 : 1
        y: root.hasFooter ? root.height - tabBarLoader.height : Math.max(0, -root.yOffset)

        Rectangle {
            id: backgroundRectangle
            anchors.fill: parent
            anchors.topMargin: (root.yOffset > Theme.paddingSmall) || root.hasFooter ? 0 : Theme.paddingSmall
            anchors.rightMargin: Theme.paddingSmall
            color: __silica_applicationwindow_instance._backgroundColor
        }
    }

    delegate: Loader {
        // tab container
        id: tabLoader

        readonly property bool isCurrentItem: PagedView.isCurrentItem
        readonly property real _yOffset: item && item._yOffset || 0

        property bool loading: Qt.application.active && isCurrentItem && status === Loader.Loading
        property bool animate

        sourceComponent: model[root.sourceProperty]

        asynchronous: true

        width: item ? item.implicitWidth : root.contentItem.width
        height: item ? item.implicitHeight : root.contentItem.height

        onLoaded: {
            if (item) {
                item.opacity = 0
                item.focus = true
                if (item.hasOwnProperty("_tabContainer")) {
                    item._tabContainer = tabLoader;
                }

                tabFadeAnimation.restart()
                if (!animate) {
                    tabFadeAnimation.complete();
                }
            }
        }

        SequentialAnimation {
            id: tabFadeAnimation

            FadeAnimation {
                target: busyIndicator
                from: 1
                to: 0
            }
            FadeAnimation {
                target: tabLoader.item
                from: 0
                to: 1
            }
        }

        BusyIndicator {
            id: busyIndicator

            running: !delayBusy.running && loading

            x: (tabLoader.width - width) / 2
            y: root.height / 3 - height / 2 - tabBarLoader.height

            size: BusyIndicatorSize.Large

            Timer {
                id: delayBusy

                onTriggered: tabLoader.animate = true

                interval: 800
                running: tabLoader.loading
            }
        }
    }
}
