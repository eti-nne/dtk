/* dtkComposerTransmitterEmitter.tpp --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Tue Feb 14 10:37:37 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Sep  7 14:34:06 2012 (+0200)
 *           By: tkloczko
 *     Update #: 282
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERTRANSMITTEREMITTER_TPP
#define DTKCOMPOSERTRANSMITTEREMITTER_TPP

#include "dtkComposerTransmitter_p.h"
#include "dtkComposerTransmitterVariant.h"

#include <dtkCore/dtkGlobal.h>

#include <dtkContainer/dtkContainerVectorWrapper.h>

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterEmitter implementation
// /////////////////////////////////////////////////////////////////

//! Constructs an empty emitter.
/*! 
 *  
 */
template <typename T> inline dtkComposerTransmitterEmitter<T>::dtkComposerTransmitterEmitter(dtkComposerNode *parent) : dtkComposerTransmitter(parent)
{
    T t;
    d->type.setValue(t);

    d->variant.setValue(m_data);
};

//! Destroys the emitter.
/*! 
 *  
 */
template <typename T> inline dtkComposerTransmitterEmitter<T>::~dtkComposerTransmitterEmitter(void)
{

};

//! Sets the data with \a data.
/*! 
 *  
 */
template <typename T> inline void dtkComposerTransmitterEmitter<T>::setData(T *data)
{
    m_data = data;
    d->variant.setValue(m_data);
};

//! Returns the data as a modifiable reference.
/*! 
 *  
 */
template <typename T> inline T *dtkComposerTransmitterEmitter<T>::data(void)
{
    return m_data;
};

//! Returns the data as a modifiable reference.
/*! 
 *  
 */
template <typename T> inline bool dtkComposerTransmitterEmitter<T>::enableCopy(void)
{
    return (d->receivers.count() > 1);
};

//! Returns.
/*! 
 *  
 */
template <typename T> inline dtkComposerTransmitter::Kind dtkComposerTransmitterEmitter<T>::kind(void) const
{
    return dtkComposerTransmitter::Emitter;
};

template <typename T> inline QString dtkComposerTransmitterEmitter<T>::kindName(void) const
{
    return "Emitter";
};

template <typename T> dtkComposerTransmitter::LinkMap dtkComposerTransmitterEmitter<T>::leftLinks(dtkComposerTransmitter *transmitter, dtkComposerTransmitterLinkList list)
{
    DTK_UNUSED(transmitter);

    LinkMap link_map;
    foreach(dtkComposerTransmitterLink *l, list)
        link_map.insert(this, l);

    return link_map;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterEmitterVector implementation
// /////////////////////////////////////////////////////////////////

template <typename T> inline dtkComposerTransmitterEmitterVector<T>::dtkComposerTransmitterEmitterVector(dtkComposerNode *parent) : dtkComposerTransmitterEmitter<T>(parent)
{
    dtkAbstractContainerWrapper w;
    d->type.setValue(w);

    d->variant.setValue(d->container);
};

template <typename T> inline dtkComposerTransmitterEmitterVector<T>::~dtkComposerTransmitterEmitterVector(void)
{

};

template <typename T> inline dtkComposerTransmitter::Kind dtkComposerTransmitterEmitterVector<T>::kind(void) const
{
    return dtkComposerTransmitter::EmitterVector;
};

template <typename T> inline QString dtkComposerTransmitterEmitterVector<T>::kindName(void) const
{
    return "EmitterContainer";
};

template <typename T> inline void dtkComposerTransmitterEmitterVector<T>::setData(dtkContainerVector<T> *vector)
{
    m_vector = vector;
    if (!d->container)
        d->container = new dtkContainerVectorWrapper<T>(m_vector);
    else
        d->container->setVector(m_vector);
    d->variant.setValue(d->container);
};

template <typename T> inline dtkContainerVector<T> *dtkComposerTransmitterEmitterVector<T>::data(void)
{
    return m_vector;
};

//! Returns true when the emitter is connected to more than one
//! receiver.
/*! 
 *  When several receivers are connected to the emitter, some nodes
 *  receiving the data can modify it while others only read
 *  it. According to the order of such operations, the data can be
 *  modified before it is read leading to unexpected behaviors.
 *
 *  To circumvent this issue, the emitter informs the receivers that
 *  they must copy the data if they modify it ensuring that the
 *  original data is not corrupted for the other nodes.
 */
template <typename T> inline bool dtkComposerTransmitterEmitterVector<T>::enableCopy(void)
{
    return (d->receivers.count() > 1);
};

#endif
