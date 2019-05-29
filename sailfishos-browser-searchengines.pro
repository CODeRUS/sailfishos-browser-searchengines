TARGET = sailfishos-browser-searchengines

QT += network dbus xmlpatterns
CONFIG += sailfishapp

SOURCES += \
    src/sailfishos-browser-searchengines.cpp \
    src/dbusadaptor.cpp \
    src/searchenginesmodel.cpp

HEADERS += \
    src/dbusadaptor.h \
    src/searchenginesmodel.h

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

EXTRA_CFLAGS=-W -Wall -Wextra -Wpedantic -Werror=return-type
QMAKE_CXXFLAGS += $$EXTRA_CFLAGS
QMAKE_CFLAGS += $$EXTRA_CFLAGS

DISTFILES += \
    rpm/sailfishos-browser-searchengines.spec \
    translations/*.ts \
    sailfishos-browser-searchengines.desktop \
    qml/sailfishos-browser-searchengines.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml

dbusService.files = dbus/org.coderus.searchengines.service
dbusService.path = /usr/share/dbus-1/system-services/
INSTALLS += dbusService

dbusConf.files = dbus/org.coderus.searchengines.conf
dbusConf.path = /etc/dbus-1/system.d/
INSTALLS += dbusConf

CONFIG += sailfishapp_i18n

TRANSLATIONS += \
    translations/sailfishos-browser-searchengines-ru.ts

appicon.sizes = \
    86 \
    108 \
    128 \
    256

for(iconsize, appicon.sizes) {
    profile = $${iconsize}x$${iconsize}
    system(mkdir -p $${OUT_PWD}/$${profile})

    appicon.commands += /usr/bin/sailfish_svg2png \
        -s 1 1 1 1 1 1 $${iconsize} \
        $${_PRO_FILE_PWD_}/appicon \
        $${profile}/apps/ &&

    appicon.files += $${profile}
}
appicon.commands += true
appicon.path = /usr/share/icons/hicolor/

INSTALLS += appicon

CONFIG += sailfish-svg2png

INCLUDEPATH += /usr/include
