/* @(#)dtkComposerGraphNodeSetConditions.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2012 - Nicolas Niclausse, Inria.
 * Created: 2012/02/14 13:55:56
 * Version: $Id$
 * Last-Updated: mar. févr. 14 16:49:55 2012 (+0100)
 *           By: Nicolas Niclausse
 *     Update #: 28
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef DTKCOMPOSERGRAPHNODESETCONDITIONS_H
#define DTKCOMPOSERGRAPHNODESETCONDITIONS_H

#include "dtkComposerNode.h"
#include "dtkComposerGraphNode.h"

class dtkComposerGraphNodeSetConditionsPrivate;

// /////////////////////////////////////////////////////////////////
// dtkComposerGraphNodeSetConditions
// /////////////////////////////////////////////////////////////////

class dtkComposerGraphNodeSetConditions : public dtkComposerGraphNode
{

public:
     dtkComposerGraphNodeSetConditions(dtkComposerNode *node);


public:
    void eval(void);

private:

    dtkComposerGraphNodeSetConditionsPrivate *d;
};


#endif /* DTKCOMPOSERGRAPHNODESETCONDITIONS_H */

