/* dtkComposerEvaluator.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Mon Jan 30 11:30:36 2012 (+0100)
 * Version: $Id$
 * Last-Updated: jeu. sept. 20 23:29:39 2012 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 25
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSEREVALUATOR_H
#define DTKCOMPOSEREVALUATOR_H

#include "dtkComposerExport.h"

#include <QtCore/QObject>

class dtkComposerGraph;
class dtkComposerEvaluatorPrivate;

class DTKCOMPOSER_EXPORT dtkComposerEvaluator : public QObject
{
    Q_OBJECT

public:
     dtkComposerEvaluator(QObject *parent = 0);
    ~dtkComposerEvaluator(void);

signals:
    void evaluationStarted(void);
    void evaluationStopped(void);

signals:
    void runMainThread(void);

public:
    void setGraph(dtkComposerGraph *graph);

public:
    void  run(bool run_concurrent = false);
    void cont(bool run_concurrent = false);
    bool step(bool run_concurrent = false);
    void next(bool run_concurrent = false);
    void stop();
    void reset();

public:
    void  logStack(void);

private:
    dtkComposerEvaluatorPrivate *d;
};

#endif
