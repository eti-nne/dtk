/* dtkComposerNodeInteger.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jul 11 19:19:23 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jul 11 19:25:14 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerNodeInteger.h"
#include "dtkComposerNodeProperty.h"

class dtkComposerNodeIntegerPrivate
{
public:
};

dtkComposerNodeInteger::dtkComposerNodeInteger(dtkComposerNode *parent) : dtkComposerNode(parent), d(new dtkComposerNodeIntegerPrivate)
{
    this->setTitle("Integer");
    this->setType(dtkComposerNode::Atomic);

    this->addOutputProperty(new dtkComposerNodeProperty("value", dtkComposerNodeProperty::Output, dtkComposerNodeProperty::Multiple, this));
}

dtkComposerNodeInteger::~dtkComposerNodeInteger(void)
{
    delete d;

    d = NULL;
}
