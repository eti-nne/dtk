/* dtkAbstractDataDeserializer_p.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Tue Apr 24 11:13:02 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Apr 24 16:55:49 2012 (+0200)
 *           By: tkloczko
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTDATADESERIALIZER_P
#define DTKABSTRACTDATADESERIALIZER_P

#include "dtkCoreExport.h"
#include "dtkAbstractObject_p.h"
#include "dtkAbstractDataDeserializer.h"

class dtkAbstractData;

////////////////////////////////////////////////////
// dtkAbstractDataDeserializerPrivate interface
////////////////////////////////////////////////////

class DTKCORE_EXPORT dtkAbstractDataDeserializerPrivate : public dtkAbstractObjectPrivate
{
public:
    dtkAbstractDataDeserializerPrivate(dtkAbstractDataDeserializer *q = 0) : dtkAbstractObjectPrivate(q) {}
    dtkAbstractDataDeserializerPrivate(const dtkAbstractDataDeserializerPrivate& other) : dtkAbstractObjectPrivate(other),
                                                                                          enabled(false),
                                                                                          data(other.data) {}

public:
    virtual ~dtkAbstractDataDeserializerPrivate(void) {}

public:
    bool enabled;

public:
    dtkAbstractData *data;
};

////////////////////////////////////////////////////
// dtkAbstractDataDeserializer protected constructors
////////////////////////////////////////////////////

DTK_IMPLEMENT_PRIVATE(dtkAbstractDataDeserializer, dtkAbstractObject);

#endif