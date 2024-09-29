TEMPLATE = subdirs

SUBDIRS = \
    libs \
    app \

app.depends = libs

OTHER_FILES += \
    rpm/ru.yurasov.wallet.spec \
    LICENSE \
    README.md \

