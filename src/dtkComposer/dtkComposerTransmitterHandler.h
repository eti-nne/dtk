// Version: $Id$
// 
// 

// Commentary: 
// 
// 

// Change Log:
// 
// 

// Code:



#pragma once

class dtkComposerTransmitter;

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterHandler
// /////////////////////////////////////////////////////////////////

template <typename T> class dtkComposerTransmitterHandler
{
public:
    static inline void init(dtkComposerTransmitter& t);

public:
    static inline bool enableConnection(dtkComposerTransmitter& t);

public:
    static inline T      data(dtkComposerTransmitter& t);
    static inline T constData(dtkComposerTransmitter& t);
};

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterHandler
// /////////////////////////////////////////////////////////////////

template <typename T> class dtkComposerTransmitterHandler<T *>
{
public:
    static inline void init(dtkComposerTransmitter& t);

public:
    static inline bool enableConnection(dtkComposerTransmitter& t);

public:
    static inline T      *data(dtkComposerTransmitter& t);
    static inline T *constData(dtkComposerTransmitter& t);

protected:
    static T *copy(T * source, QVariant& target, QVariant& swap);
};

/* // ///////////////////////////////////////////////////////////////// */
/* // dtkComposerTransmitterHandler pointer specialization */
/* // ///////////////////////////////////////////////////////////////// */

/* template <typename T, bool U> class dtkComposerTransmitterHandlerHelper {}; */

/* template <typename T, bool U> class dtkComposerTransmitterHandlerHelper<T *, U> */
/* { */
/* public: */
/*     static inline T *copy(T *source, QVariant& target); */
/* }; */

/* // ///////////////////////////////////////////////////////////////// */
/* // dtkComposerTransmitterHandler specialization for dtkCoreObject */
/* // ///////////////////////////////////////////////////////////////// */

/* template <typename T> class dtkComposerTransmitterHandlerHelper<T *, true> */
/* { */
/* public: */
/*     static inline T *copy(T *source, QVariant& target); */
/* }; */

// /////////////////////////////////////////////////////////////////
// dtkComposerTransmitterHandler implementation
// /////////////////////////////////////////////////////////////////

#include "dtkComposerTransmitterHandler.tpp"

// 
// dtkComposerTransmitterHandler.h ends here
