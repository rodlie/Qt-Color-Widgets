/**
 * \file gradient_editor.cpp
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2013-2019 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "QtColorWidgets/gradient_editor.hpp"

#include <QPainter>
#include <QStyleOptionSlider>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QApplication>

#include "QtColorWidgets/gradient_helper.hpp"

namespace color_widgets {

class GradientEditor::Private
{
public:
    QGradientStops stops;
    QBrush back;
    Qt::Orientation orientation;
    int highlighted = -1;
    QLinearGradient gradient;
    bool dragging = false;

    Private() :
        back(Qt::darkGray, Qt::DiagCrossPattern)
    {
        back.setTexture(QPixmap(QStringLiteral(":/color_widgets/alphaback.png")));
        gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
        gradient.setSpread(QGradient::RepeatSpread);
    }

    void refresh_gradient()
    {
        gradient.setStops(stops);
    }

    int closest(QMouseEvent *ev, GradientEditor* owner)
    {
        if ( stops.empty() )
            return -1;
        if ( stops.size() == 1 || owner->geometry().width() <= 5 )
            return 0;
        qreal pos = move_pos(ev, owner);

        int i = 1;
        for ( ; i < stops.size()-1; i++ )
            if ( stops[i].first >= pos )
                break;

        if ( stops[i].first - pos < pos - stops[i-1].first )
            return i;
        return i-1;
    }

    qreal move_pos(QMouseEvent *ev, GradientEditor* owner)
    {
        return (owner->geometry().width() > 5) ?
            qMax(qMin(static_cast<qreal>(ev->pos().x() - 2.5) / (owner->geometry().width() - 5), 1.0), 0.0)
            : 0;
    }
};

GradientEditor::GradientEditor(QWidget *parent) :
    GradientEditor(Qt::Horizontal, parent)
{}

GradientEditor::GradientEditor(Qt::Orientation orientation, QWidget *parent) :
    QWidget(parent), p(new Private)
{
    p->orientation = orientation;
    setMouseTracking(true);
    resize(sizeHint());
}

GradientEditor::~GradientEditor()
{
    delete p;
}

void GradientEditor::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton )
    {
        ev->accept();
        p->dragging = false;
        qreal pos = p->move_pos(ev, this);
        auto info = gradientBlendedColorInsert(p->stops, pos);
        p->stops.insert(info.first, info.second);
        p->highlighted = info.first;
        p->refresh_gradient();
        update();
    }
    else
    {
        QWidget::mousePressEvent(ev);
    }
}

void GradientEditor::mousePressEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton )
    {
        ev->accept();
        p->dragging = true;
        p->highlighted = p->closest(ev, this);
        update();
    }
    else
    {
        QWidget::mousePressEvent(ev);
    }
}

void GradientEditor::mouseMoveEvent(QMouseEvent *ev)
{
    if ( ev->buttons() & Qt::LeftButton && p->highlighted != -1 )
    {
        ev->accept();
        qreal pos = p->move_pos(ev, this);
        if ( p->highlighted > 0 && pos < p->stops[p->highlighted-1].first )
        {
            std::swap(p->stops[p->highlighted], p->stops[p->highlighted-1]);
            p->highlighted--;
        }
        else if ( p->highlighted < p->stops.size()-1 && pos > p->stops[p->highlighted+1].first )
        {
            std::swap(p->stops[p->highlighted], p->stops[p->highlighted+1]);
            p->highlighted++;
        }
        p->stops[p->highlighted].first = pos;
        p->refresh_gradient();
        update();
    }
    else
    {
        p->highlighted = p->closest(ev, this);
        update();
    }
}

void GradientEditor::mouseReleaseEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton && p->highlighted != -1 )
    {
        ev->accept();
        if ( !rect().contains(ev->localPos().toPoint()) )
        {
            p->stops.remove(p->highlighted);
            p->highlighted = -1;
            p->refresh_gradient();
        }
        p->dragging = false;
        emit stopsChanged(p->stops);
        update();
    }
    else
    {
        QWidget::mousePressEvent(ev);
    }
}

void GradientEditor::leaveEvent(QEvent*)
{
    p->dragging = false;
    p->highlighted = -1;
    update();
}


QBrush GradientEditor::background() const
{
    return p->back;
}

void GradientEditor::setBackground(const QBrush &bg)
{
    p->back = bg;
    update();
    Q_EMIT backgroundChanged(bg);
}

QGradientStops GradientEditor::stops() const
{
    return p->stops;
}

void GradientEditor::setStops(const QGradientStops &colors)
{
    p->highlighted = -1;
    p->dragging = false;
    p->stops = colors;
    p->refresh_gradient();
    emit stopsChanged(p->stops);
    update();
}

QLinearGradient GradientEditor::gradient() const
{
    return p->gradient;
}

void GradientEditor::setGradient(const QLinearGradient &gradient)
{
    setStops(gradient.stops());
}

Qt::Orientation GradientEditor::orientation() const
{
    return p->orientation;
}

void GradientEditor::setOrientation(Qt::Orientation orientation)
{
    p->orientation = orientation;
    update();
}


void GradientEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QStyleOptionFrame panel;
    panel.initFrom(this);
    panel.lineWidth = 1;
    panel.midLineWidth = 0;
    panel.state |= QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_Frame, &panel, &painter, this);
    QRect r = style()->subElementRect(QStyle::SE_FrameContents, &panel, this);
    painter.setClipRect(r);


    if(orientation() == Qt::Horizontal)
        p->gradient.setFinalStop(1, 0);
    else
        p->gradient.setFinalStop(0, -1);

    painter.setPen(Qt::NoPen);
    painter.setBrush(p->back);
    painter.drawRect(1,1,geometry().width()-2,geometry().height()-2);
    painter.setBrush(p->gradient);
    painter.drawRect(1,1,geometry().width()-2,geometry().height()-2);

    int i = 0;
    for ( const QGradientStop& stop : p->stops )
    {
        qreal pos = stop.first * (geometry().width() - 5);
        QColor color = stop.second;
        Qt::GlobalColor border_color = Qt::black;
        Qt::GlobalColor core_color = Qt::white;

        if (color.valueF() >= 0.5 && color.alphaF() <= 0.5)
            std::swap(core_color, border_color);

        QPointF p1 = QPointF(2.5, 2.5) + QPointF(pos, 0);
        QPointF p2 = p1 + QPointF(0, geometry().height() - 5);
        if ( i == p->highlighted )
        {
            painter.setPen(QPen(border_color, 5));
            painter.drawLine(p1, p2);
            painter.setPen(QPen(core_color, 3));
            painter.drawLine(p1, p2);
        }
        else
        {
            painter.setPen(QPen(border_color, 3));
            painter.drawLine(p1, p2);
        }

        i++;
    }

}

QSize GradientEditor::sizeHint() const
{
    QStyleOptionSlider opt;
    opt.orientation = p->orientation;

    int w = style()->pixelMetric(QStyle::PM_SliderThickness, &opt, this);
    int h = std::max(84, style()->pixelMetric(QStyle::PM_SliderLength, &opt, this));
    if ( p->orientation == Qt::Horizontal )
    {
        std::swap(w, h);
    }
    return style()->sizeFromContents(QStyle::CT_Slider, &opt, QSize(w, h), this)
        .expandedTo(QApplication::globalStrut());
}


} // namespace color_widgets
