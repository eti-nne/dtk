/* dtkAbstractContainer.h --- 
 * 
 * Author: tkloczko
 * Copyright (C) 2011 - Thibaud Kloczko, Inria.
 * Created: Fri Apr 27 15:25:17 2012 (+0200)
 * Version: $Id$
 * Last-Updated: Wed May  9 14:32:50 2012 (+0200)
 *           By: tkloczko
 *     Update #: 72
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKABSTRACTCONTAINER_H
#define DTKABSTRACTCONTAINER_H

#include "dtkCoreExport.h"
#include "dtkAbstractData.h"

// /////////////////////////////////////////////////////////////////
// dtkAbstractContainer interface
// /////////////////////////////////////////////////////////////////

class DTKCORE_EXPORT dtkAbstractContainer : public dtkAbstractData
{
    Q_OBJECT

public:
    enum Type {
        None,
        Vector,
        List
    };

public:
             dtkAbstractContainer(dtkAbstractData *parent = 0);
             dtkAbstractContainer(const dtkAbstractContainer& other);
    virtual ~dtkAbstractContainer(void);

protected:
    void init(void);

public:
    void reset(void);

public:
    dtkAbstractContainer& operator = (const dtkAbstractContainer& other);

public:
    virtual dtkAbstractContainer *clone(void) const;

public:
    QString identifier(void) const;

public:
    virtual Type type(void) const;

public:
    virtual void clear(void);

    virtual void  append(const QVariant& data);
    virtual void prepend(const QVariant& data);
    virtual void  remove(const QVariant& data);

    virtual void  insert(const QVariant& data, dtkxarch_int index);
    virtual void replace(const QVariant& data, dtkxarch_int index);

    virtual void resize(dtkxarch_int size);

public:
    virtual bool  isEmpty(void) const;

    virtual bool contains(const QVariant& data) const;

    virtual dtkxarch_int count(void) const;

    virtual dtkxarch_int indexOf(const QVariant& data, dtkxarch_int from = 0) const;
     
    virtual QVariant    at(dtkxarch_int index) const;
    virtual QVariant first(void) const;
    virtual QVariant  last(void) const;

public:
    bool operator != (const dtkAbstractContainer& other) const;
    bool operator == (const dtkAbstractContainer& other) const;

public:
    virtual bool isEqual(const dtkAbstractContainer& other) const;

public:
    virtual QVariant operator[] (dtkxarch_int index) const;

private:
    dtkAbstractContainer *m_container;
};

Q_DECLARE_METATYPE(dtkAbstractContainer);
Q_DECLARE_METATYPE(dtkAbstractContainer *);

#endif
