/* dtkComposerTransmitterVariant.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Fri Mar  2 16:19:20 2012 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Oct  5 13:15:22 2012 (+0200)
 *           By: tkloczko
 *     Update #: 115
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERTRANSMITTERVARIANT_H
#define DTKCOMPOSERTRANSMITTERVARIANT_H

#include <dtkConfig.h>

#include "dtkComposerExport.h"
#include "dtkComposerTransmitter.h"

#include <dtkCore/dtkGlobal>

template <typename T> class dtkContainerVector;

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterVariant declaration
// /////////////////////////////////////////////////////////////////

class dtkComposerTransmitterVariantPrivate;

class DTKCOMPOSER_EXPORT dtkComposerTransmitterVariant : public dtkComposerTransmitter
{
public:
     dtkComposerTransmitterVariant(dtkComposerNode *parent = 0);
    ~dtkComposerTransmitterVariant(void);

public:
    void clearData(void);

public:
    template <typename T> inline void setData(T *data);
    template <typename T> inline void setData(dtkContainerVector<T> *data);

public:
    void setData(const QVariant& data);

    void setData(dtkAbstractContainerWrapper *data);

public:
    void setDataFrom(dtkComposerTransmitterVariant *source);

    void setDataFrom(QByteArray& array);

public:
    template <typename T> T *data(void);

public:
    QByteArray dataToByteArray(void);

public:
    QVariant& variant(void);

    dtkAbstractContainerWrapper *containerFromEmitter(void);
    dtkAbstractContainerWrapper *container(void);

public:
    dtkAbstractObject *object(void);

public:
    int dataType(void);

    QString  dataIdentifier(void);
    QString dataDescription(void);

public:
    QList<int> allDataType(void);

    QStringList  allDataIdentifier(void);
    QStringList allDataDescription(void);

public:
    void setActive(bool active);

public:
    void activateEmitter(dtkComposerTransmitter        *emitter);
    void activateEmitter(dtkComposerTransmitterVariant *emitter);

public:
    bool enableCopy(void);

    void reset(void);

public:
    bool isEmpty(void) const;

public:
    void setTwin(dtkComposerTransmitterVariant *twin);

    dtkComposerTransmitterVariant *twin(void);
    
    void setTwinned(bool twinned);

public:
    Kind kind(void) const;

    QString kindName(void) const;

public:
    void setDataTypes(const QList<int>& data_types);

    QList<int> dataTypes(void);

public:
    bool    connect(dtkComposerTransmitter *transmitter);
    bool disconnect(dtkComposerTransmitter *transmitter);

public:
    LinkMap  leftLinks(dtkComposerTransmitter *transmitter, dtkComposerTransmitterLinkList list);
    LinkMap rightLinks(dtkComposerTransmitter *transmitter, dtkComposerTransmitterLinkList list);

protected:
    dtkComposerTransmitterVariantPrivate *e;
};

#include "dtkComposerTransmitterVariant.tpp"

#endif
