/* dtkComposerNodePlotView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Tue May 29 14:32:55 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Tue May 29 15:07:54 2012 (+0200)
 *           By: Julien Wintz
 *     Update #: 25
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERNODEPLOTVIEW_H
#define DTKCOMPOSERNODEPLOTVIEW_H

#include "dtkComposerExport.h"
#include "dtkComposerNodeLeaf.h"

#include <QtCore>

class dtkComposerNodePlotViewPrivate;

class DTKCOMPOSER_EXPORT dtkComposerNodePlotView : public QObject, public dtkComposerNodeLeaf
{
    Q_OBJECT

public:
     dtkComposerNodePlotView(void);
    ~dtkComposerNodePlotView(void);

public:
    inline QString type(void) {
        return "dtkPlotView";
    }

    inline QString titleHint(void) {
        return "Plot view";
    }

public:
    inline QString inputLabelHint(int port) {
        switch(port) {
        default:
            return "curve";
        }
    }

public:
    void run(void);

signals:
    void runned(void);

protected slots:
    void onRun(void);

private:
    dtkComposerNodePlotViewPrivate *d;
};

#endif
