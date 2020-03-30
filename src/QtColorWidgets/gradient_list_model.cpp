/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2013-2020 Mattia Basaglia
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

#include "QtColorWidgets/gradient_list_model.hpp"

#include <QMap>
#include <QPainter>
#include <QPixmap>

using namespace color_widgets;

class GradientListModel::Private
{
public:
    QMap<QString, int> indices;
    QVector<QLinearGradient> gradients;
    QSize icon_size{48, 32};
    QBrush background;
    ItemEditMode edit_mode = EditNone;

    Private()
    {
        background.setTexture(QPixmap(QStringLiteral(":/color_widgets/alphaback.png")));
    }

    bool acceptable(const QModelIndex& index) const
    {
        return acceptable(index.row());
    }

    bool acceptable(int row) const
    {
        return row >= 0 && row <= gradients.size();
    }

    QPixmap preview(const QLinearGradient& grad)
    {
        QPixmap out (icon_size);
        QPainter painter(&out);
        QRect r({0, 0}, icon_size);
        painter.fillRect(r, background);
        painter.fillRect(r, grad);
        return out;
    }

    QLinearGradient make_gradient(const QGradientStops& gradient_stops)
    {
        QLinearGradient gradient(0, 0, 1, 0);
        gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
        gradient.setSpread(QGradient::RepeatSpread);
        gradient.setStops(gradient_stops);
        return gradient;
    }
};

GradientListModel::GradientListModel(QObject *parent)
    : QAbstractListModel(parent), d(new Private())
{
}

GradientListModel::~GradientListModel() = default;


int color_widgets::GradientListModel::count() const
{
    return d->gradients.size();
}

void color_widgets::GradientListModel::clear()
{
    beginResetModel();
    d->gradients.clear();
    endResetModel();
}

QSize color_widgets::GradientListModel::iconSize() const
{
    return d->icon_size;
}

void color_widgets::GradientListModel::setIconSize ( const QSize& iconSize )
{
    d->icon_size = iconSize;
    Q_EMIT iconSizeChanged(d->icon_size);
}

int color_widgets::GradientListModel::setGradient ( const QString& name, const QGradient& gradient )
{
    return setGradient(name, gradient.stops());
}


int color_widgets::GradientListModel::setGradient ( const QString& name, const QGradientStops& gradient_stops )
{
    auto iter = d->indices.find(name);
    if ( iter != d->indices.end() )
    {
        return setGradient(*iter, gradient_stops);
    }

    int index = d->gradients.size();
    beginInsertRows(QModelIndex(), index, index);
    d->gradients.push_back(d->make_gradient(gradient_stops));
    d->indices[name] = index;
    endInsertRows();
    return index;
}

bool color_widgets::GradientListModel::setGradient(int index, const QGradient& gradient)
{
    return setGradient(index, gradient.stops());
}

bool color_widgets::GradientListModel::setGradient(int index, const QGradientStops& gradient_stops)
{
    if ( index < 0 || index > d->gradients.size() )
        return false;

    d->gradients[index].setStops(gradient_stops);
    QModelIndex mindex = createIndex(index, 0);
    Q_EMIT dataChanged(mindex, mindex, {Qt::DecorationRole, Qt::ToolTipRole});
    return true;
}



QGradientStops color_widgets::GradientListModel::gradientStops ( const QString& name ) const
{
    auto iter = d->indices.find(name);
    if ( iter != d->indices.end() )
        return d->gradients[*iter].stops();
    return {};
}

QGradientStops color_widgets::GradientListModel::gradientStops ( int index ) const
{
    if ( index > 0 && index < d->gradients.size() )
        return d->gradients[index].stops();
    return {};
}

const QLinearGradient & color_widgets::GradientListModel::gradient ( int index ) const
{
    return d->gradients[index];
}

const QLinearGradient & color_widgets::GradientListModel::gradient ( const QString& name ) const
{
    return d->gradients[d->indices[name]];
}

int color_widgets::GradientListModel::indexFromName ( const QString& name ) const
{
    auto iter = d->indices.find(name);
    if ( iter != d->indices.end() )
        return *iter;
    return -1;
}

int color_widgets::GradientListModel::rowCount ( const QModelIndex& ) const
{
    return d->gradients.size();
}

bool color_widgets::GradientListModel::hasGradient ( const QString& name ) const
{
    auto iter = d->indices.find(name);
    if ( iter != d->indices.end() )
        return true;
    return false;
}

bool color_widgets::GradientListModel::removeGradient ( int index )
{
    if ( index < 0 || index >= d->indices.size() )
        return false;

    beginRemoveRows(QModelIndex{}, index, index);
    d->gradients.erase(d->gradients.begin() + index);
    for ( auto it = d->indices.begin(); it != d->indices.end(); ++it )
    {
        if ( *it == index )
        {
            d->indices.erase(it);
            break;
        }
    }
    endRemoveRows();
    return true;
}

bool color_widgets::GradientListModel::removeGradient ( const QString& name )
{
    auto iter = d->indices.find(name);
    if ( iter != d->indices.end() )
        return false;

    int index = *iter;
    beginRemoveRows(QModelIndex{}, index, index);
    d->gradients.erase(d->gradients.begin() + index);
    d->indices.erase(iter);
    endRemoveRows();
    return true;
}

QVariant color_widgets::GradientListModel::data ( const QModelIndex& index, int role ) const
{
    if ( !d->acceptable(index) )
        return QVariant();


    auto iter = d->indices.begin();
    while ( *iter != index.row() )
        ++iter;
    const QLinearGradient& gradient = d->gradients[index.row()];
    switch( role )
    {
        case Qt::DisplayRole:
            return iter.key();
        case Qt::DecorationRole:
            return d->preview(gradient);
        case Qt::ToolTipRole:
            return tr("%1 (%2 colors)").arg(iter.key()).arg(gradient.stops().size());
        case Qt::EditRole:
            if ( d->edit_mode == EditGradient )
                return QBrush(gradient);
            else if ( d->edit_mode == EditName )
                return iter.key();
            return {};
    }

    return QVariant();
}

bool color_widgets::GradientListModel::rename(int index, const QString& new_name)
{
    if ( d->indices.contains(new_name) )
        return false;

    for ( auto it = d->indices.begin(); it != d->indices.end(); ++it )
    {
        if ( *it == index )
        {
            d->indices.erase(it);
            d->indices[new_name] = index;
            QModelIndex mindex = createIndex(index, 0);
            Q_EMIT dataChanged(mindex, mindex, {Qt::DisplayRole, Qt::ToolTipRole});
            return true;
        }
    }

    return false;
}

bool color_widgets::GradientListModel::rename(const QString& old_name, const QString& new_name)
{
    if ( d->indices.contains(new_name) )
        return false;

    auto it = d->indices.find(old_name);
    if ( it != d->indices.end() )
    {
        int index = *it;
        d->indices.erase(it);
        d->indices[new_name] = index;
        QModelIndex mindex = createIndex(index, 0);
        Q_EMIT dataChanged(mindex, mindex, {Qt::DisplayRole, Qt::ToolTipRole});
        return true;
    }

    return false;
}

Qt::ItemFlags color_widgets::GradientListModel::flags(const QModelIndex& index) const
{
    auto flags = QAbstractListModel::flags(index);
    if ( d->edit_mode )
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool color_widgets::GradientListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if ( !d->acceptable(index) )
        return false;

    if ( role == Qt::DisplayRole )
    {
        return rename(index.row(), value.toString());
    }
    else if ( role == Qt::EditRole )
    {
        if ( d->edit_mode == EditName )
            return rename(index.row(), value.toString());

        if ( d->edit_mode == EditGradient )
        {
            const QGradient* grad = value.value<QBrush>().gradient();
            if ( !grad )
                return false;
            return setGradient(index.row(), *grad);
        }
    }

    return false;
}

color_widgets::GradientListModel::ItemEditMode color_widgets::GradientListModel::editMode() const
{
    return d->edit_mode;
}

void color_widgets::GradientListModel::setEditMode(color_widgets::GradientListModel::ItemEditMode mode)
{
    d->edit_mode = mode;
    Q_EMIT editModeChanged(mode);
}

