// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import QtQml.Models 2.2
import Sailfish.Silica 1.0
import "Util.js" as Util

SilicaControl {
    id: root

    property Flickable flickable
    property bool allowDeletion: true
    readonly property bool isCurrentItem: _tabContainer && _tabContainer.PagedView.isCurrentItem

    property Item _tabContainer: root
    readonly property real _yOffset: flickable && flickable.pullDownMenu
            ? flickable.contentY - flickable.originY
            : 0
    property alias _cacheExpiry: cleanupTimer.interval

    implicitWidth: _tabContainer ? _tabContainer.PagedView.contentWidth : 0
    implicitHeight: {
        if (!_tabContainer) {
            return 0;
        } else {
            var view = flickable && flickable.pullDownMenu ? _tabContainer.PagedView.view : null;
            return view && !view.hasFooter ? view.height : _tabContainer.PagedView.contentHeight;
        }
    }

    clip: !flickable || !flickable.pullDownMenu

    Component.onCompleted: {
        if (_tabContainer)
            _tabContainer.DelegateModel.inPersistedItems = true;
    }

    Timer {
        id: cleanupTimer

        running: root.allowDeletion && root._tabContainer && !root._tabContainer.PagedView.exposed
        interval: 30000

        onTriggered: root._tabContainer.DelegateModel.inPersistedItems = false
    }
}
