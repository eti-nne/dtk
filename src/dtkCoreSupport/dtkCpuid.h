/* @(#)dtkCpuid.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2012 - Nicolas Niclausse, Inria.
 * Created: 2012/04/25 13:06:29
 * Version: $Id$
 * Last-Updated: mer. avril 25 15:57:26 2012 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 36
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef DTKCPUID_H
#define DTKCPUID_H

#include "dtkCoreSupportExport.h"
#include <QtCore>

class dtkCpuidPrivate;

class DTKCORESUPPORT_EXPORT dtkCpuid
{
public:
             dtkCpuid(void);
    virtual ~dtkCpuid(void);

public:
  QString vendor(void);

private:
  dtkCpuidPrivate *d;

private:
  void load(unsigned i);
};

#endif /* DTKCPUID_H */

