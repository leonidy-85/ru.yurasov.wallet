// SPDX-FileCopyrightText: 2023 Open Mobile Platform LLC <community@omp.ru>
// SPDX-License-Identifier: BSD-3-Clause

.pragma library

function findFlickable(item) {
    var parentItem = item ? item.parent : null
    while (parentItem) {
        if (parentItem.maximumFlickVelocity && !parentItem.hasOwnProperty('__silica_hidden_flickable')) {
            return parentItem
        }
        parentItem = parentItem.parent
    }
    return null
}

function findParentWithProperty(item, propertyName) {
    var parentItem = item ? item.parent : null
    while (parentItem) {
        if (parentItem.hasOwnProperty(propertyName)) {
            return parentItem
        }
        parentItem = parentItem.parent
    }
    return null
}

function findPageStack(item) {
    return findParentWithProperty(item, '_pageStackIndicator')
}

function findPage(item) {
    return findParentWithProperty(item, '__silica_page')
}

function childAt(parent, x, y) {
    var child = parent.childAt(x, y)
    if (child && child.hasOwnProperty("fragmentShader") && child.source && child.source.layer.effect) {
        // This item is a layer effect of the actual item.
        child = child.source
    }

    return child
}
