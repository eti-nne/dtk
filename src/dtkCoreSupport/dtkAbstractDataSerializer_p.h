/* dtkAbstractDataSerializer_p.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Tue Apr 24 11:04:31 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Apr 24 16:54:08 2012 (+0200)
 *           By: tkloczko
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */
 
#ifndef DTKABSTRACTDATASERIALIZER_P
#define DTKABSTRACTDATASERIALIZER_P

#include "dtkCoreExport.h"
#include "dtkAbstractObject_p.h"
#include "dtkAbstractDataSerializer.h"

class QByteArray;

////////////////////////////////////////////////////
// dtkAbstractDataSerializerPrivate interface
////////////////////////////////////////////////////

class DTKCORE_EXPORT dtkAbstractDataSerializerPrivate : public dtkAbstractObjectPrivate
{
public:
    dtkAbstractDataSerializerPrivate(dtkAbstractDataSerializer *q = 0) : dtkAbstractObjectPrivate(q) {}
    dtkAbstractDataSerializerPrivate(const dtkAbstractDataSerializerPrivate& other) : dtkAbstractObjectPrivate(other),
                                                                                      enabled(false),
                                                                                      array(other.array) {}

public:
    virtual ~dtkAbstractDataSerializerPrivate(void) {}

public:
    bool enabled;

public:
    QByteArray *array;
};

////////////////////////////////////////////////////
// dtkAbstractDataSerializer protected constructors
////////////////////////////////////////////////////

DTK_IMPLEMENT_PRIVATE(dtkAbstractDataSerializer, dtkAbstractObject);

#endif