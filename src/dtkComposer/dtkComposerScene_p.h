/* dtkComposerScene_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sat Feb 26 18:33:27 2011 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar 24 10:38:57 2011 (+0100)
 *           By: Thibaud Kloczko
 *     Update #: 31
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERSCENE_P_H
#define DTKCOMPOSERSCENE_P_H

class dtkComposerEdge;
class dtkComposerNode;
class dtkComposerNodeFactory;
class dtkComposerNote;

// /////////////////////////////////////////////////////////////////
// dtkComposerSceneClipboard
// /////////////////////////////////////////////////////////////////

class dtkComposerSceneClipboard
{
public:
    QList<dtkComposerNode *> nodes;
    QList<dtkComposerEdge *> edges;
};

// /////////////////////////////////////////////////////////////////
// dtkComposerScenePrivate
// /////////////////////////////////////////////////////////////////

class dtkComposerScenePrivate
{
public:
    dtkComposerEdge *edge(dtkComposerEdge *edge);

public:
    bool isChildOf(QGraphicsItem *item, QGraphicsItem *parent);

public:
    QPointF grabber_node_origin;

public:
    dtkComposerNode *grabber_node;
    dtkComposerNode *current_node; // current_node is NULL for the root, !NULL when inside a composite node.
    dtkComposerEdge *current_edge;

public:
    dtkComposerNodeFactory *factory;

public:
    bool modified;

public:
    QList<dtkComposerNode *> nodes;
    QList<dtkComposerEdge *> edges;
    QList<dtkComposerNote *> notes;

public:
    dtkComposerSceneClipboard clipboard;
};

#endif
