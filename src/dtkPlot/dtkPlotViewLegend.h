/* dtkPlotViewLegend.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Fri Jun  8 12:52:22 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Aug 29 12:20:56 2013 (+0200)
 *           By: Julien Wintz
 *     Update #: 57
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKPLOTVIEWLEGEND_H
#define DTKPLOTVIEWLEGEND_H

#include "dtkPlotExport.h"

#include <QtCore/QObject>

#include <qwt_global.h>

class QwtPlotItem;
class dtkPlotView;
class dtkPlotViewLegendPrivate;

class DTKPLOT_EXPORT dtkPlotViewLegend : public QObject
{
    Q_OBJECT

public:
     dtkPlotViewLegend(dtkPlotView *parent);
    ~dtkPlotViewLegend(void);

public:
    void   activate(void);
    void deactivate(void);

public:
    void setPosition(int position);

protected slots:
#if QWT_VERSION >= 0x060100
    void legendChecked(const QVariant& itemInfo, bool value);
#endif
    void showCurve(QwtPlotItem *item, bool value);

private:
    dtkPlotViewLegendPrivate *d;
};

#endif
