# Copyright (C) 2013-2019 Mattia Basaglia
#
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Color Widgets.  If not, see <http://www.gnu.org/licenses/>.

CONFIG += c++11

INCLUDEPATH += $$PWD/src $$PWD/include

SOURCES += \
    $$PWD/src/QtColorWidgets/color_2d_slider.cpp \
    $$PWD/src/QtColorWidgets/color_delegate.cpp \
    $$PWD/src/QtColorWidgets/color_dialog.cpp \
    $$PWD/src/QtColorWidgets/color_line_edit.cpp \
    $$PWD/src/QtColorWidgets/color_list_widget.cpp \
    $$PWD/src/QtColorWidgets/color_names.cpp \
    $$PWD/src/QtColorWidgets/color_palette.cpp \
    $$PWD/src/QtColorWidgets/color_palette_model.cpp \
    $$PWD/src/QtColorWidgets/color_palette_widget.cpp \
    $$PWD/src/QtColorWidgets/color_preview.cpp \
    $$PWD/src/QtColorWidgets/color_selector.cpp \
    $$PWD/src/QtColorWidgets/color_utils.cpp \
    $$PWD/src/QtColorWidgets/color_wheel.cpp \
    $$PWD/src/QtColorWidgets/gradient_editor.cpp \
    $$PWD/src/QtColorWidgets/gradient_slider.cpp \
    $$PWD/src/QtColorWidgets/hue_slider.cpp

HEADERS += \
    $$PWD/include/QtColorWidgets/color_utils.hpp \
    $$PWD/include/QtColorWidgets/AbstractWidgetList \
    $$PWD/include/QtColorWidgets/abstract_widget_list.hpp \
    $$PWD/include/QtColorWidgets/BoundColorSelector \
    $$PWD/include/QtColorWidgets/bound_color_selector.hpp \
    $$PWD/include/QtColorWidgets/color_2d_slider.hpp \
    $$PWD/include/QtColorWidgets/ColorDelegate \
    $$PWD/include/QtColorWidgets/color_delegate.hpp \
    $$PWD/include/QtColorWidgets/ColorDialog \
    $$PWD/include/QtColorWidgets/color_dialog.hpp \
    $$PWD/include/QtColorWidgets/color_line_edit.hpp \
    $$PWD/include/QtColorWidgets/ColorListWidget \
    $$PWD/include/QtColorWidgets/color_list_widget.hpp \
    $$PWD/include/QtColorWidgets/color_names.hpp \
    $$PWD/include/QtColorWidgets/color_palette.hpp \
    $$PWD/include/QtColorWidgets/color_palette_model.hpp \
    $$PWD/include/QtColorWidgets/color_palette_widget.hpp \
    $$PWD/include/QtColorWidgets/ColorPreview \
    $$PWD/include/QtColorWidgets/color_preview.hpp \
    $$PWD/include/QtColorWidgets/ColorSelector \
    $$PWD/include/QtColorWidgets/color_selector.hpp \
    $$PWD/include/QtColorWidgets/color_utils.hpp \
    $$PWD/include/QtColorWidgets/ColorWheel \
    $$PWD/include/QtColorWidgets/color_wheel.hpp \
    $$PWD/include/QtColorWidgets/colorwidgets_global.hpp \
    $$PWD/include/QtColorWidgets/GradientEditor \
    $$PWD/include/QtColorWidgets/gradient_editor.hpp \
    $$PWD/include/QtColorWidgets/gradient_helper.hpp \
    $$PWD/include/QtColorWidgets/GradientSlider \
    $$PWD/include/QtColorWidgets/gradient_slider.hpp \
    $$PWD/include/QtColorWidgets/HueSlider \
    $$PWD/include/QtColorWidgets/hue_slider.hpp

FORMS += \
    $$PWD/src/QtColorWidgets/color_dialog.ui \
    $$PWD/src/QtColorWidgets/color_palette_widget.ui

RESOURCES += \
    $$PWD/resources/QtColorWidgets/color_widgets.qrc

