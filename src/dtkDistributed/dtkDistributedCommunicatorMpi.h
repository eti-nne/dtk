/* dtkDistributedCommunicatorMpi.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Feb 15 16:50:54 2010 (+0100)
 * Version: $Id$
 * Last-Updated: lun. nov. 28 16:51:27 2011 (+0100)
 *           By: Nicolas Niclausse
 *     Update #: 50
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKDISTRIBUTEDCOMMUNICATORMPI_H
#define DTKDISTRIBUTEDCOMMUNICATORMPI_H

#include "dtkDistributedExport.h"
#include "dtkDistributedCommunicator.h"

class dtkDistributedCommunicatorMpiPrivate;

class DTKDISTRIBUTED_EXPORT dtkDistributedCommunicatorMpi : public dtkDistributedCommunicator
{
    Q_OBJECT

public:
     dtkDistributedCommunicatorMpi(void);
    ~dtkDistributedCommunicatorMpi(void);

    void   initialize(void);
    void uninitialize(void);

    double time(void);
    double tick(void);

    int rank(void);
    int size(void);

    QString name(void) const;

    void   barrier(void);
    void      send(void *data,             qint64 size, DataType dataType, quint16 target, int tag);
    void   receive(void *data,             qint64 size, DataType dataType, quint16 source, int tag);
    void   receive(void *data,             qint64 size, DataType dataType, quint16 source, int tag, int& from);
    void broadcast(void *data,             qint64 size, DataType dataType, quint16 source);
    void    gather(void *send, void *recv, qint64 size, DataType dataType, quint16 target, bool all = false);
    void   scatter(void *send, void *recv, qint64 size, DataType dataType, quint16 source);
    void    reduce(void *send, void *recv, qint64 size, DataType dataType, OperationType operationType, quint16 target, bool all = false);

    void    send(dtkAbstractData *data, quint16 target, int tag);
    void receive(dtkAbstractData *data, quint16 source, int tag);


private:
    dtkDistributedCommunicatorMpiPrivate *d;
};

#endif
