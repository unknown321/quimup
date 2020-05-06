###########################################################################################
##
##	Author    : Johan Spee (quimup@coonsden.com)
##	Project   : Quimup
##	FileName  : Quimup.pro
##	Date      : 2015-01-21T10:10:10
##	License   : GPL
##	Comment   : Quimup is a client for MPD
##	Home Page : www.coonsden.com
##
##	This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
##	WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
##
###########################################################################################

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui network widgets

TARGET = quimup
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG = libmpdclient
PKGCONFIG += taglib

BUILD_PATH = ./build

CONFIG(debug, debug|release) {
    #Debug
    CONFIG += console
    unix:TARGET = $$join(TARGET,,,_debug)
    else:TARGET = $$join(TARGET,,,d)
    unix:OBJECTS_DIR = $${BUILD_PATH}/debug/.obj/unix
    win32:OBJECTS_DIR = $${BUILD_PATH}/debug/.obj/win32
    mac:OBJECTS_DIR = $${BUILD_PATH}/debug/.obj/mac
    UI_DIR = $${BUILD_PATH}/debug/.ui
    MOC_DIR = $${BUILD_PATH}/debug/.moc
    RCC_DIR = $${BUILD_PATH}/debug/.rcc
} else {
    #Release
    unix:OBJECTS_DIR = $${BUILD_PATH}/release/.obj/unix
    win32:OBJECTS_DIR = $${BUILD_PATH}/release/.obj/win32
    mac:OBJECTS_DIR = $${BUILD_PATH}/release/.obj/mac
    UI_DIR = $${BUILD_PATH}/release/.ui
    MOC_DIR = $${BUILD_PATH}/release/.moc
    RCC_DIR = $${BUILD_PATH}/release/.rcc
}

SOURCES += src/main.cpp \
    src/qm_clicklabel.cpp \
    src/qm_clickprogressbar.cpp \
    src/qm_config.cpp \
    src/qm_core.cpp \
    src/qm_mpdcom.cpp \
    src/qm_player.cpp \
    src/qm_scroller.cpp \
    src/qm_trayicon.cpp \
    src/qm_settings.cpp \
    src/qm_browser.cpp \
    src/qm_playlistview.cpp \
    src/qm_libraryview.cpp \
    src/qm_itemlist.cpp \
    src/qm_songinfo.cpp \
    src/qm_streamloader.cpp \
    src/qm_colorpicker.cpp \
    src/qtlocalpeer.cpp \
    src/qtsingleapplication.cpp \
    src/qtlockedfile.cpp

HEADERS += src/qm_clicklabel.h \
    src/qm_clickprogressbar.h \
    src/qm_config.h \
    src/qm_core.h \
    src/qm_mpdcom.h \
    src/qm_player.h \
    src/qm_scroller.h \
    src/qm_songinfo.h \
    src/qm_trayicon.h \
    src/qm_widget_ids.h \
    src/qm_settings.h \
    src/qm_browser.h \
    src/qm_playlistview.h \
    src/qm_libraryview.h \
    src/qm_commandlist.h \
    src/qm_itemlist.h \
    src/qm_browser_ids.h \
    src/qm_streamloader.h \
    src/qm_colorpicker.h \
    src/qtlocalpeer.h \
    src/qtsingleapplication.h \
    src/qtlockedfile.h

RESOURCES += src/quimup.qrc

TRANSLATIONS += quimup_nl.ts \
    quimup_cs.ts

