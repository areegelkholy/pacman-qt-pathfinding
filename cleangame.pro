QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

# Sources
SOURCES += \
    source/constants.cpp \
    source/core/gamemanager.cpp \
    source/core/level.cpp \
    source/entities/blade.cpp \
    source/entities/entity.cpp \
    source/entities/thief.cpp \
    source/items/item.cpp \
    source/items/simpleitem.cpp \
    source/items/trapitem.cpp \
    source/main.cpp \
    source/entities/pacman.cpp \
    source/entities/ghost.cpp \
    source/strategy/chasing_strategy.cpp \
    source/ui/mainwindow.cpp \
    source/ui/MenuScene.cpp \
    source/ui/StoreDialog.cpp \
    source/ui/spritepickerdialog.cpp

# Headers
HEADERS += \
    source/constants.h \
    source/core/gamemanager.h \
    source/core/level.h \
    source/entities/blade.h \
    source/entities/entity.h \
    source/entities/ghost.h \
    source/entities/pacman.h \
    source/entities/thief.h \
    source/items/item.h \
    source/items/simpleitem.h \
    source/items/trapitem.h \
    source/strategy/chasing_strategy.h \
    source/ui/mainwindow.h \
    source/ui/MenuScene.h \
    source/ui/StoreDialog.h \
    source/ui/spritepickerdialog.h

# UI forms
FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PWD/source
INCLUDEPATH += $$PWD/source/core
INCLUDEPATH += $$PWD/source/entities
INCLUDEPATH += $$PWD/source/ui
INCLUDEPATH += $$PWD/source/strategy


# Resources
RESOURCES += \
    resources.qrc

# Deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
