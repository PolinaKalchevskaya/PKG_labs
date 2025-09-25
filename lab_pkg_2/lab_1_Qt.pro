QT += core gui widgets

CONFIG += c++17

TARGET = ImageAnalyzer
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AdvancedImageInfo.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Включение поддержки всех форматов изображений
QT += gui

# Оптимизация для больших файлов
CONFIG += optimize_size

# Отключение предупреждений для удобства
CONFIG += warn_off
