/* dtkMatrixSquared.h --- 
 * 
 * Author: Thibaud Kloczko
 * Copyright (C) 2008 - Thibaud Kloczko, Inria.
 * Created: Tue Jun  8 14:08:32 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Sep 20 08:35:06 2012 (+0200)
 *           By: tkloczko
 *     Update #: 45
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKMATRIXSQUARED_H
#define DTKMATRIXSQUARED_H

#include "dtkMatrix.h"

template <typename T = double> class dtkMatrixSquared : public dtkMatrix<T>
{
public:
     dtkMatrixSquared(void): dtkMatrix<T>() {};
     dtkMatrixSquared(unsigned crowInit): dtkMatrix<T>( crowInit, crowInit) {};
     dtkMatrixSquared(const dtkMatrix<T> &mat): dtkMatrix<T>(mat) {};
     dtkMatrixSquared(const dtkMatrixSquared &matSquared): dtkMatrix<T>(matSquared) {};
     dtkMatrixSquared(const dtkMatrix<T> &, unsigned, unsigned, unsigned);
     dtkMatrixSquared(const dtkMatrixSquared<T> &, unsigned, unsigned, unsigned);
    ~dtkMatrixSquared(void) {};

public:
    QString identifier(void) const;

public:
    void allocate(unsigned crowInit) {
        dtkMatrix<T>::allocate(crowInit, crowInit);
    }

    void mapInto(const dtkMatrixSquared &, unsigned, unsigned, unsigned);

    unsigned size(void) const { return this->getRows(); };

    dtkMatrixSquared operator +(const dtkMatrixSquared &) const;
    dtkMatrixSquared operator -(const dtkMatrixSquared &) const;
    dtkMatrixSquared operator -(void) const;
    dtkMatrixSquared operator *(const T &) const;
    dtkMatrixSquared operator *(const dtkMatrixSquared &) const;
    dtkMatrixSquared operator /(const T &value) const {
        T tTmp = dtkUnity<T>();
        tTmp /= value;
        return (*this)*tTmp;
    }
    dtkMatrixSquared operator /(const dtkMatrixSquared &) const;

    dtkMatrixSquared & operator =(const dtkMatrixSquared &matSquared);
    dtkMatrixSquared & operator +=(const dtkMatrixSquared &matSquared);
    dtkMatrixSquared & operator -=(const dtkMatrixSquared &matSquared);
    dtkMatrixSquared & operator *=(const T &value);
    dtkMatrixSquared & operator *=(const dtkMatrixSquared &);
    dtkMatrixSquared & operator /=(const T &value);
    dtkMatrixSquared & operator /=(const dtkMatrixSquared &);

    void storeAdjoint(const dtkMatrixSquared &);
    void storeInverse(const dtkMatrixSquared &);

    void makeAdjoint(void);
    void makeInverse(void);
    void makeUnity(void);
};

template <typename T, unsigned crow> class dtkMatSquared: public T
{
public:
    dtkMatSquared(void): T(crow) {}

    T & operator =(const T &smtx) { return T::operator=(smtx); }
};

template <typename T, unsigned crow> class dtkUnity< dtkMatSquared<dtkMatrixSquared<T>, crow> >: public dtkMatSquared<dtkMatrixSquared<T>, crow>
{
public:
    dtkUnity(void) { this->makeUnity(); }
};

template <typename T, unsigned crow> class dtkZero< dtkMatSquared<dtkMatrixSquared<T>, crow> >: public dtkMatSquared<dtkMatrixSquared<T>, crow>
{
public:
    dtkZero(void) { fill(dtkZero<T>()); }
};

// /////////////////////////////////////////////////////////////////
// Implementation of the template class dtkMatrixSquared's methods
// /////////////////////////////////////////////////////////////////

#include "dtkMatrixSquared.tpp"

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

#include <QtCore>

typedef dtkMatrixSquared<qreal> dtkMatrixSquareReal;

Q_DECLARE_METATYPE(dtkMatrixSquareReal);
Q_DECLARE_METATYPE(dtkMatrixSquareReal *);

#endif

// /////////////////////////////////////////////////////////////////
// Credits
// /////////////////////////////////////////////////////////////////

// $Id: smatrix.h 184 2008-05-10 19:43:17Z hkuiper $

// CwMtx matrix and vector math library
// Copyright (C) 1999-2001  Harry Kuiper
// Copyright (C) 2000  Will DeVore (template conversion)

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
