/* dtkComposerTransmitterReceiver.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Tue Feb 14 11:39:15 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Wed May 16 11:59:19 2012 (+0200)
 *           By: tkloczko
 *     Update #: 79
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERTRANSMITTERRECEIVER_H
#define DTKCOMPOSERTRANSMITTERRECEIVER_H

#include "dtkComposerTransmitter.h"

#include <dtkCore/dtkContainerVector.h>
#include <dtkCore/dtkContainerList.h>

#include <QtCore>

template <typename T> class dtkComposerTransmitterEmitter;
class dtkComposerTransmitterVariant;

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterReceiver declaration
// /////////////////////////////////////////////////////////////////

template <typename T> class DTKCOMPOSER_EXPORT dtkComposerTransmitterReceiver : public dtkComposerTransmitter
{
public:
     dtkComposerTransmitterReceiver(dtkComposerNode *parent = 0);
    ~dtkComposerTransmitterReceiver(void);

public:
          T& data(void);
    const T& data(void) const;

public:
          dtkAbstractContainer& container(void);
    const dtkAbstractContainer& container(void) const;

          dtkContainerVector<T>& vector(void);
    const dtkContainerVector<T>& vector(void) const;

          dtkContainerList<T>& list(void);
    const dtkContainerList<T>& list(void) const;


public:
    bool isEmpty(void) const;

public:
    Kind kind(void) const;

    QString kindName(void) const;

public:
    bool    connect(dtkComposerTransmitter *transmitter);
    bool disconnect(dtkComposerTransmitter *transmitter);

public:
    void setActiveEmitter(dtkComposerTransmitter *emitter);

public:
    LinkMap rightLinks(dtkComposerTransmitter *transmitter, dtkComposerTransmitterLinkList list);

private:
    QList<dtkComposerTransmitterEmitter<T> *> emitters;
    QList<dtkComposerTransmitterVariant *>    variants;

private:
    dtkComposerTransmitterEmitter<T> *active_emitter;
    dtkComposerTransmitterVariant    *active_variant;

private:
    T m_data;

    dtkContainerVector<T> m_vector;
    dtkContainerList<T>   m_list;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterReceiver implementation
// /////////////////////////////////////////////////////////////////

#include "dtkComposerTransmitterReceiver.tpp"

#endif
