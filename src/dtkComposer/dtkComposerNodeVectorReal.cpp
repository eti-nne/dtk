/* dtkComposerNodeVectorReal.cpp ---
 *
 * Author: sblekout
 * Copyright (C) 2011 - babette Lekouta, Inria.
 * Created: Tue May 15 11:35:09 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 24 18:32:17 2012 (+0200)
 *           By: Julien Wintz
 *     Update #: 58
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "dtkComposerNodeVectorReal.h"
#include "dtkComposerTransmitterEmitter.h"
#include "dtkComposerTransmitterReceiver.h"

#include <dtkMath>

#include <dtkLog/dtkLog>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

class dtkComposerNodeVectorRealPrivate
{
public:
    dtkComposerTransmitterReceiver<dtkVectorReal>  receiver_vector;
    dtkComposerTransmitterReceiver<qlonglong>      receiver_size;
    dtkComposerTransmitterReceiver<qreal>          receiver_value;

public:
    dtkComposerTransmitterEmitter<dtkVectorReal>   emitter_vector;
    dtkComposerTransmitterEmitter<qlonglong>       emitter_size;
};

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

dtkComposerNodeVectorReal::dtkComposerNodeVectorReal(void) : dtkComposerNodeLeaf(), d(new dtkComposerNodeVectorRealPrivate)
{
    this->appendReceiver(&d->receiver_vector);
    this->appendReceiver(&d->receiver_size);
    this->appendReceiver(&d->receiver_value);

    this->appendEmitter(&d->emitter_vector);
    this->appendEmitter(&d->emitter_size);
}

dtkComposerNodeVectorReal::~dtkComposerNodeVectorReal(void)
{
    delete d;

    d = NULL;
}

QString dtkComposerNodeVectorReal::inputLabelHint(int port)
{
    switch(port) {
    case 0:
        return "vector";
        break;
    case 1:
        return "size";
        break;
    case 2:
        return "value";
        break;
    default:
        break;
    }

    return "port";
}

QString dtkComposerNodeVectorReal::outputLabelHint(int port)
{
    switch(port) {
    case 0:
        return "vector";
        break;
    case 1:
        return "size";
        break;
    default:
        break;
    }

    return "port";
}

void dtkComposerNodeVectorReal::run(void)
{
    if (!d->receiver_vector.isEmpty()) {

        dtkVectorReal vec(d->receiver_vector.data());

        d->emitter_vector.setData(vec);
        d->emitter_size.setData(vec.getRows());

    } else {

        if (!d->receiver_size.isEmpty()) {

            unsigned int t =  d->receiver_size.data();
            dtkVectorReal vec(t);

            if (t==0)
                dtkWarn() << "The size of the vector is null " ;

            else {
                for(int i = 0 ; i < vec.getRows(); i++)
                    vec[i] = d->receiver_value.data();

                d->emitter_size.setData(vec.getRows());
                d->emitter_vector.setData(vec);
            }
        }
    }

}
