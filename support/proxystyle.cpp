/*
 * Cantata
 *
 * Copyright (c) 2011-2018 Craig Drummond <craig.p.drummond@gmail.com>
 *
 * ----
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "proxystyle.h"
#include "acceleratormanager.h"
#include "gtkstyle.h"
#include "utils.h"
#include "monoicon.h"
#include <QMenu>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>

#if !defined Q_OS_MAC
static const char * constAccelProp="managed-accel";
#endif
const char * ProxyStyle::constModifyFrameProp="mod-frame";

ProxyStyle::ProxyStyle(int modView)
    : modViewFrame(modView)
{
    #if !defined Q_OS_WIN && !defined Q_OS_MAC
    editClearIcon=MonoIcon::icon(FontAwesome::timescircle, QColor(128, 128, 128), QColor(128, 128, 128));
    #endif
}

void ProxyStyle::polish(QWidget *widget)
{
    #if !defined Q_OS_MAC
    if (widget && qobject_cast<QMenu *>(widget) && !widget->property(constAccelProp).isValid()) {
        AcceleratorManager::manage(widget);
        widget->setProperty(constAccelProp, true);
    }
    #endif
    baseStyle()->polish(widget);
}

int ProxyStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    return SH_DialogButtonBox_ButtonsHaveIcons == hint ? false : baseStyle()->styleHint(hint, option, widget, returnData);
}

int ProxyStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    return PM_MessageBoxIconSize==metric ? 64 : baseStyle()->pixelMetric(metric, option, widget);
}

void ProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    baseStyle()->drawPrimitive(element, option, painter, widget);
    if (modViewFrame && PE_Frame==element && widget) {
        QVariant v=widget->property(constModifyFrameProp);
        if (v.isValid()) {
            int mod=v.toInt();
            const QRect &r=option->rect;
            if (option->palette.base().color()==Qt::transparent) {
                painter->setPen(QPen(QApplication::palette().color(QPalette::Base), 1));
            } else {
                painter->setPen(QPen(option->palette.base(), 1));
            }
            if (mod&VF_Side && modViewFrame&VF_Side) {
                if (Qt::LeftToRight==option->direction) {
                    painter->drawLine(r.topRight()+QPoint(0, 1), r.bottomRight()+QPoint(0, -1));
                } else {
                    painter->drawLine(r.topLeft()+QPoint(0, 1), r.bottomLeft()+QPoint(0, -1));
                }
            }
            if (mod&VF_Top && modViewFrame&VF_Top) {
                painter->drawLine(r.topLeft()+QPoint(1, 0), r.topRight());
            }
        }
    }
}

#if !defined Q_OS_WIN && !defined Q_OS_MAC
QPixmap ProxyStyle::standardPixmap(StandardPixmap sp, const QStyleOption *opt, const QWidget *widget) const
{
    QPixmap pixmap=baseStyle()->standardPixmap(sp, opt, widget);
    if (SP_LineEditClearButton==sp) {
        return editClearIcon.pixmap(pixmap.size());
    }
    return pixmap;
}

QIcon ProxyStyle::standardIcon(StandardPixmap sp, const QStyleOption *opt, const QWidget *widget) const
{
    if (SP_LineEditClearButton==sp) {
        return editClearIcon;
    }
    return baseStyle()->standardIcon(sp, opt, widget);
}
#endif
