/* dtkAbstractViewInteractor.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Jan 29 14:36:41 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Feb  9 22:50:50 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 80
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef DTKABSTRACTVIEWINTERACTOR_H
#define DTKABSTRACTVIEWINTERACTOR_H

#include <dtkCore/dtkAbstractObject.h>

class dtkAbstractViewInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class DTKCORE_EXPORT dtkAbstractViewInteractor : public dtkAbstractObject
{
    Q_OBJECT

public:
             dtkAbstractViewInteractor(void);
    virtual ~dtkAbstractViewInteractor(void);

    virtual QString description(void) const = 0;
    virtual QStringList handled(void) const = 0;

            bool enabled(void);
    virtual void  enable(void);
    virtual void disable(void);

    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual dtkAbstractData *data(void);
    virtual dtkAbstractView *view(void);

    virtual dtkAbstractData *output (void);
    virtual dtkAbstractData *output (int channel);
    virtual dtkAbstractData *output (int channel, int frame);

    virtual void  predraw(void);
    virtual void     draw(void);
    virtual void postdraw(void);

signals:
    void updated(void);

private:
    dtkAbstractViewInteractorPrivate *d;
};

#endif
