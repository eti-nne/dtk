/* dtkComposerNodeMetaScalarArray.cpp --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Fri Jul 13 16:06:48 2012 (+0200)
 * Version: $Id$
 * Last-Updated: mer. nov.  7 15:03:50 2012 (+0100)
 *           By: Nicolas Niclausse
 *     Update #: 24
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerNodeMetaScalarArray.h"

#include <dtkComposer/dtkComposerTransmitterEmitter.h>

#include <dtkContainer/dtkAbstractContainerWrapper.h>
#include <dtkContainer/dtkContainerVector.h>

#include <dtkLog/dtkLog>

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeMetaScalarArrayPrivate interface
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeMetaScalarArrayPrivate
{
public:
    dtkComposerTransmitterEmitterVector<dtkContainerVector<qreal> *> emitter_arrays;

    dtkContainerVector< dtkContainerVector<qreal> *> arrays;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerNodeMetaScalarArray implementation
// /////////////////////////////////////////////////////////////////

dtkComposerNodeMetaScalarArray::dtkComposerNodeMetaScalarArray(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeMetaScalarArrayPrivate)
{
    d->emitter_arrays.setData(&d->arrays);

    this->appendEmitter(&d->emitter_arrays);
}

dtkComposerNodeMetaScalarArray::~dtkComposerNodeMetaScalarArray(void)
{
    delete d;
    
    d = NULL;
}

QString dtkComposerNodeMetaScalarArray::outputLabelHint(int port)
{    
    switch(port) {
    case 0:
        return "arrays";
        break;
    default:
        break;
    }

    return "port";
}

void dtkComposerNodeMetaScalarArray::run(void)
{
    d->arrays.clear();
}