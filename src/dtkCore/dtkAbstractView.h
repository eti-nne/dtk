/* dtkAbstractView.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 16:00:26 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Feb 13 20:54:40 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 229
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef DTKABSTRACTVIEWER_H
#define DTKABSTRACTVIEWER_H

#include <dtkCore/dtkAbstractObject.h>

#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QWidget>

class dtkAbstractViewPrivate;
class dtkAbstractViewAnimator;
class dtkAbstractViewNavigator;
class dtkAbstractViewInteractor;

class dtkAbstractData;

class DTKCORE_EXPORT dtkAbstractView : public dtkAbstractObject
{
    Q_OBJECT

public:
             dtkAbstractView(      dtkAbstractView *parent = 0);
	     dtkAbstractView(const dtkAbstractView& view);
    virtual ~dtkAbstractView(void);

    virtual QString description(void) const { return ""; }

    friend QDebug operator<<(QDebug debug, const dtkAbstractView& viewer);
    friend QDebug operator<<(QDebug debug,       dtkAbstractView *viewer);

signals:
    void selected(dtkAbstractData *);

    void clickedScreenCoordinates(int, int);

    void clickedObjectCoordinates(double, double);
    void clickedObjectCoordinates(double, double, double);

    void clickedWorldCoordinates(double, double);
    void clickedWorldCoordinates(double, double, double);

public slots:
    virtual void   link(dtkAbstractView *other);
    virtual void unlink(dtkAbstractView *other);

    virtual void   select(dtkAbstractData *data);
    virtual void unselect(dtkAbstractData *data);

    virtual void setView(void *view);

    virtual void setData(dtkAbstractData *data);
    virtual void setData(dtkAbstractData *data, int inputId);

    virtual void setBackgroundColor(int red, int green, int blue);
    virtual void setBackgroundColor(double red, double green, double blue);

    virtual void *view(void);
    virtual void *data(void);
    virtual void *data(int channel);

    virtual void  clear(void);
    virtual void  reset(void);
    virtual void update(void);

    virtual QWidget *widget(void);

    void showFullScreen(void);
    void showMinimized(void);
    void showMaximized(void);
    void showNormal(void);

    void addAnimator  (dtkAbstractViewAnimator   *animator);
    void addNavigator (dtkAbstractViewNavigator  *navigator);
    void addInteractor(dtkAbstractViewInteractor *interactor);

    void  enableAnimator(QString animator);
    void disableAnimator(QString animator);

    void  enableNavigator(QString navigator);
    void disableNavigator(QString navigator);

    void  enableInteractor(QString interactor);
    void disableInteractor(QString interactor);

    dtkAbstractViewAnimator   *animator  (QString type);
    dtkAbstractViewNavigator  *navigator (QString type);
    dtkAbstractViewInteractor *interactor(QString type);

    QList<dtkAbstractViewAnimator   *> animators(void);
    QList<dtkAbstractViewNavigator  *> navigators(void);
    QList<dtkAbstractViewInteractor *> interactors(void);

    virtual void   initialize(void);
    virtual void uninitialize(void);

    virtual void setupCameraLookAt(const QVector3D& eye, const QVector3D& center, const QVector3D& up);
    virtual void setupCameraFrustum(double left, double right, double bottom, double top, double near, double far);

private:
    dtkAbstractViewPrivate *d;
};

QDebug operator<<(QDebug debug, const dtkAbstractView& viewer);
QDebug operator<<(QDebug debug,       dtkAbstractView *viewer);

#endif
