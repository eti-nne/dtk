/* @(#)dtkDistributedCommunicator.cpp ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2013 - Nicolas Niclausse, Inria.
 * Created: 2013/02/07 13:59:41
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "dtkDistributedCommunicator.h"
#include "dtkDistributedWorker.h"

// /////////////////////////////////////////////////////////////////
// dtkDistributedCommunicatorPrivate
// /////////////////////////////////////////////////////////////////

class dtkDistributedCommunicatorPrivate
{
public:
    QMap<qlonglong, void *> buffer_map;

public:
    qlonglong id;

public:
    bool initialized;
};


// /////////////////////////////////////////////////////////////////
// dtkDistributedCommunicator
// /////////////////////////////////////////////////////////////////


dtkDistributedCommunicator::dtkDistributedCommunicator(void) : QObject(), d(new dtkDistributedCommunicatorPrivate)
{
    d->id = 0;
    d->initialized = false;
}

dtkDistributedCommunicator::~dtkDistributedCommunicator(void)
{
    delete d;

    d = NULL;
}

dtkDistributedCommunicator::dtkDistributedCommunicator(const dtkDistributedCommunicator& other)
{

}

dtkDistributedCommunicator& dtkDistributedCommunicator::operator = (const dtkDistributedCommunicator& other)
{
    return *this;

}

void dtkDistributedCommunicator::initialize(void)
{
    d->initialized = true;
}

bool dtkDistributedCommunicator::initialized(void)
{
    return d->initialized;
}

void dtkDistributedCommunicator::uninitialize(void)
{
    d->initialized = false;
}

void dtkDistributedCommunicator::spawn(QStringList hostnames, qlonglong np, dtkDistributedWorker worker)
{
}

void dtkDistributedCommunicator::barrier(void)
{

}

int dtkDistributedCommunicator::pid(void)
{
   return 0;
}

int dtkDistributedCommunicator::size(void)
{
   return 1;
}

void *dtkDistributedCommunicator::allocate(qlonglong count, qlonglong size, qlonglong wid, qlonglong& buffer_id)
{
    void *buffer = malloc(size*count);
    d->buffer_map.insert(d->id, buffer);
    buffer_id = (d->id)++;
    return buffer;
}

void dtkDistributedCommunicator::deallocate(qlonglong wid, const qlonglong& buffer_id)
{
    void *buffer = d->buffer_map.take(buffer_id);
    free (buffer);
}

void dtkDistributedCommunicator::get(qint32 from, qlonglong position, void *array, qlonglong buffer_id)
{

    if (d->buffer_map.contains(buffer_id)) {
        char *buffer = static_cast<char*>(d->buffer_map[buffer_id]);
        memcpy ( array, buffer + position * sizeof(qlonglong), sizeof(qlonglong) );
    } else {
        qDebug() <<  "unknown buffer" << buffer_id;
    }
}

void dtkDistributedCommunicator::put(qint32 dest, qlonglong position, void *data, qlonglong buffer_id)
{
    if (d->buffer_map.contains(buffer_id)) {
        char *buffer = static_cast<char*>(d->buffer_map[buffer_id]);
        memcpy ( buffer + position * sizeof(qlonglong), data, sizeof(qlonglong) );

    } else {
        qDebug() <<  "unknown buffer" << buffer_id;
    }
}

