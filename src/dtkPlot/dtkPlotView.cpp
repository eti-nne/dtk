/* dtkPlotView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Wed Jun  1 17:04:01 2011 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Sep 20 17:20:19 2013 (+0200)
 *           By: Selim Kraria
 *     Update #: 489
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkPlotCurve.h"
#include "dtkPlotView.h"
#include "dtkPlotViewPanner.h"
#include "dtkPlotViewPicker.h"
#include "dtkPlotViewZoomer.h"
#include "dtkPlotViewGrid.h"

#include "dtkPlotViewToolBar.h"

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_zoomer.h>

#include <float.h>

class dtkPlotViewPrivate : public QwtPlot
{
public:
    dtkPlotViewPanner *panner;
    dtkPlotViewPicker *picker;
    dtkPlotViewZoomer *zoomer;
    dtkPlotViewGrid *grid;
    dtkPlotViewLegend *legend;

public:
    dtkPlotViewToolBar *toolbar;

public:
    QFrame *frame_view;

public:
    int alphaCurveArea;

public:
    QColor grid_color;
    QColor picking_color;
    QColor zoom_color;
    QColor background_color;
    QColor foreground_color;

public:
    QList<dtkPlotCurve *> curves;
};

dtkPlotView::dtkPlotView(void) : dtkAbstractView(), d(new dtkPlotViewPrivate())
{
    d->panner = NULL;
    d->picker = NULL;
    d->zoomer = NULL;
    d->grid = NULL;
    d->legend = NULL;

    // Colors

    d->grid_color = Qt::black;
    d->picking_color = Qt::black;
    d->zoom_color = Qt::black;
    d->background_color = Qt::white;
    d->foreground_color = Qt::black;

    d->alphaCurveArea = 0;

    // Scale

    d->setAxisAutoScale(0, true);
    d->setAxisAutoScale(1, true);

    reinterpret_cast<QwtPlotCanvas *>(d->canvas())->setFrameStyle(QFrame::NoFrame);

    d->toolbar = new dtkPlotViewToolBar(this);

    d->frame_view = new QFrame;

    QVBoxLayout *layout = new QVBoxLayout(d->frame_view);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d);
    layout->addWidget(d->toolbar);

    this->setStyleSheet(dtkReadFile(":dtkPlot/dtkPlotView.qss"));
    this->readSettings();
}

dtkPlotView::~dtkPlotView(void)
{
    delete d;

    d = NULL;
}

void dtkPlotView::clear(void)
{
    d->curves.clear();

    foreach (QwtPlotItem *item, d->itemList()) {
        item->detach();
    }

    this->update();
}

QList<dtkPlotCurve *> dtkPlotView::curves(void)
{
    return d->curves;
}

void dtkPlotView::activatePanning(void)
{
    if(!d->panner)
        d->panner = new dtkPlotViewPanner(this);

    d->panner->activate();
}

void dtkPlotView::updateAxes()
{
    QRectF rect;

    foreach(dtkPlotCurve *curve, d->curves) {
      if (((QwtPlotCurve *)(curve->d))->isVisible()) {
          rect = rect.united(curve->boundingRect());
      }
    }

    qreal xmin = rect.left();
    qreal xmax = rect.right();
    qreal ymin = rect.top();
    qreal ymax = rect.bottom();

    if (xmin < xmax) {
        this->setAxisScaleX(xmin, xmax);
        this->setAxisScaleY(ymin, ymax);
    }

    d->updateAxes();

    if(d->zoomer)
        d->zoomer->updateBase(rect);

    this->update();
}

void dtkPlotView::deactivatePanning(void)
{
    if(!d->panner)
        d->panner = new dtkPlotViewPanner(this);

    d->panner->deactivate();
}

void dtkPlotView::activatePicking(void)
{
    if(!d->picker) {
        d->picker = new dtkPlotViewPicker(this);
        d->picker->setColor(d->picking_color);
    }

    d->picker->activate();
}

void dtkPlotView::deactivatePicking(void)
{
    if(!d->picker)
        d->picker = new dtkPlotViewPicker(this);

    d->picker->deactivate();
}

void dtkPlotView::setPickingColor(const QColor& color)
{
    if(d->picker)
        d->picker->setColor(color);

    d->picking_color = color;

    this->writeSettings();
}

QColor dtkPlotView::pickingColor(void) const
{
    return d->picking_color;
}

void dtkPlotView::activateZooming(void)
{
    if(!d->zoomer) {
        d->zoomer = new dtkPlotViewZoomer(this);
        this->setZoomColor(d->zoom_color);
    }

    d->zoomer->activate();
}

void dtkPlotView::deactivateZooming(void)
{
    if(!d->zoomer)
        d->zoomer = new dtkPlotViewZoomer(this);

    d->zoomer->deactivate();
}

bool dtkPlotView::isZoomActivated(void) const
{
    bool value = false;

    if(d->zoomer)
        value = d->zoomer->isActivated();

    return value;
}

void dtkPlotView::setZoomColor(const QColor& color)
{
    if(d->zoomer)
        d->zoomer->setColor(color);

    d->zoom_color = color;

    this->writeSettings();
}

QColor dtkPlotView::zoomColor(void) const
{
    return d->zoom_color;
}

void dtkPlotView::activateGrid(void)
{
    if(!d->grid) {
        d->grid = new dtkPlotViewGrid(this);
        d->grid->setColor(d->grid_color);
    }

    this->update();
}

void dtkPlotView::deactivateGrid(void)
{
    if(d->grid) {
        delete d->grid;
        d->grid = NULL;
    }

    this->update();
}

void dtkPlotView::setGridColor(const QColor& color)
{
    if(d->grid)
        d->grid->setColor(color);

    d->grid_color = color;

    this->writeSettings();
}

QColor dtkPlotView::gridColor(void) const
{
    return d->grid_color;
}

void dtkPlotView::activateLegend(void)
{
    if(!d->legend)
        d->legend = new dtkPlotViewLegend(this);

    this->update();
}

void dtkPlotView::deactivateLegend(void)
{
    if(d->legend) {
      delete d->legend;
      d->legend = NULL;
    }

    this->update();
}

void dtkPlotView::setTitle(const QString& title)
{
    d->setTitle(title);
}

void dtkPlotView::setTitleSize(const int& size)
{
    QFont font;
    font.setFamily("verdana");
    font.setBold(false);
    font.setPointSize(size);

    QwtText title = d->title();
    title.setFont(font);

    d->setTitle(title);
}

QString dtkPlotView::title(void) const
{
    return d->title().text();
}

int dtkPlotView::titleSize(void) const
{
    return d->title().font().pointSize();
}

void dtkPlotView::setAxisTitleX(const QString& title)
{
    d->setAxisTitle(QwtPlot::xBottom, title);
}

QString dtkPlotView::axisTitleX(void) const
{
    return d->axisWidget(QwtPlot::xBottom)->title().text();
}

void dtkPlotView::setAxisTitleY(const QString& title)
{
    d->setAxisTitle(QwtPlot::yLeft, title);
}

QString dtkPlotView::axisTitleY(void) const
{
    return d->axisWidget(QwtPlot::yLeft)->title().text();
}

void dtkPlotView::setAxisTitleSizeX(const int& size)
{
    int axisId = QwtPlot::xBottom;

    QFont font = d->title().font();
    font.setPointSize(size);

    QwtText title = d->axisTitle(axisId);
    title.setFont(font);

    d->setAxisTitle(axisId, title);
}

void dtkPlotView::setAxisTitleSizeY(const int& size)
{
    int axisId = QwtPlot::yLeft;

    QFont font = d->title().font();
    font.setPointSize(size);

    QwtText title = d->axisTitle(axisId);
    title.setFont(font);

    d->setAxisTitle(axisId, title);
}

int dtkPlotView::axisTitleSizeX(void) const
{
    return d->axisWidget(QwtPlot::xBottom)->title().font().pointSize();
}

int dtkPlotView::axisTitleSizeY(void) const
{
    return d->axisWidget(QwtPlot::yLeft)->title().font().pointSize();
}

void dtkPlotView::setAxisScaleX(double min, double max)
{
    d->setAxisScale(QwtPlot::xBottom, min, max);
}

void dtkPlotView::setAxisScaleY(double min, double max)
{
    d->setAxisScale(QwtPlot::yLeft, min, max);
}

void dtkPlotView::setAxisScaleX(dtkPlotView::Scale scale)
{
    if(scale == dtkPlotView::Linear)
        d->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    
    if(scale == dtkPlotView::Logarithmic) {
#if QWT_VERSION >= 0x060100
        d->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
#else
        d->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
#endif
    }

    this->update();
}

void dtkPlotView::setAxisScaleY(dtkPlotView::Scale scale)
{
    if(scale == dtkPlotView::Linear)
        d->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    
    if(scale == dtkPlotView::Logarithmic) {
#if QWT_VERSION >= 0x060100
        d->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
#else
        d->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
#endif
    }

    this->update();
}

void dtkPlotView::setLegendPosition(LegendPosition position)
{
    if (d->legend) {
        d->legend->setPosition(position);
    }
}

void dtkPlotView::setBackgroundColor(int red, int green, int blue)
{
    this->setBackgroundColor(QColor(red, green, blue));
}

void dtkPlotView::setBackgroundColor(double red, double green, double blue)
{
    this->setBackgroundColor(QColor(red, green, blue));
}

void dtkPlotView::setBackgroundColor(const QColor& color)
{
    d->background_color = color;

    d->setCanvasBackground(color);

    this->updateColors();
}

QColor dtkPlotView::backgroundColor() const
{
    return d->canvasBackground().color();
}

void dtkPlotView::setForegroundColor(const QColor& color)
{
    d->foreground_color = color;

    this->updateColors();
}

QColor dtkPlotView::foregroundColor() const
{
    return d->foreground_color;
}

void dtkPlotView::updateColors(void)
{
    QString sheet = "background: " + d->background_color.name() + "; color: " + d->foreground_color.name() + ";";

    d->setStyleSheet(sheet);

    this->update();

    this->writeSettings();
}

void dtkPlotView::setDark(bool dark)
{
    d->toolbar->setDark(dark);
}

void dtkPlotView::fillCurveArea(int alpha)
{
    if (alpha < 0) {
        alpha = 0;
    } else if (alpha > 255) {
        alpha = 255;
    }

    d->alphaCurveArea = alpha;

    foreach (dtkPlotCurve *curve, d->curves) {
        QColor color = curve->color();
        color.setAlphaF(alpha/255.);
        curve->setColorArea(color);
    }

    this->update();
}

int dtkPlotView::alphaCurveArea(void) const
{
    return d->alphaCurveArea;
}

void dtkPlotView::setStyleSheet(const QString& sheet)
{
    d->setStyleSheet(sheet);
    d->toolbar->setStyleSheet(sheet);
}

dtkPlotView& dtkPlotView::operator<<(dtkPlotCurve *curve)
{
    QwtPlotCurve *c = ((QwtPlotCurve *)(curve->d));

    if (c->plot() != d)
        d->curves << curve;

    if (c->plot() != d)
        c->attach((QwtPlot *)d);

    QColor c_color = curve->color();
    QColor v_color = this->backgroundColor();

    if (c_color.value() - v_color.value() < 30) {
	int r = c_color.red();
	int g = c_color.green();
	int b = c_color.blue();
	curve->setColor(QColor(255 - r, 255 - g, 255 - b));
    }

    return *(this);
}

void dtkPlotView::update(void)
{
    d->updateLayout();

    d->replot();

    emit updated();
}

QWidget *dtkPlotView::widget(void)
{
    return d->frame_view;
}

QWidget *dtkPlotView::plotWidget(void)
{
    return d;
}

void dtkPlotView::readSettings(void)
{
    QSettings settings("inria", "dtk");
    settings.beginGroup("plot");
    d->grid_color = settings.value("grid_color", Qt::black).value<QColor>();
    d->picking_color = settings.value("picking_color", Qt::black).value<QColor>();
    d->zoom_color = settings.value("zoom_color", Qt::black).value<QColor>();
    d->background_color = settings.value("background_color", Qt::white).value<QColor>();
    d->foreground_color = settings.value("forergound_color", Qt::black).value<QColor>();
    settings.endGroup();

    this->updateColors();
}

void dtkPlotView::writeSettings(void)
{
    QSettings settings("inria", "dtk");
    settings.beginGroup("plot");
    settings.setValue("grid_color", d->grid_color);
    settings.setValue("picking_color", d->picking_color);
    settings.setValue("zoom_color", d->zoom_color);
    settings.setValue("background_color", d->background_color);
    settings.setValue("forergound_color", d->foreground_color);
    settings.endGroup();
}

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

dtkAbstractView *createPlotView(void)
{
    return new dtkPlotView;
}
