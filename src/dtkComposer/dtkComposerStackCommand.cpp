/* dtkComposerStackCommand.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Tue Jan 31 18:17:43 2012 (+0100)
 * Version: $Id$
 * Last-Updated: mar. avril  3 18:13:35 2012 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 3367
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerFactory.h"
#include "dtkComposerGraph.h"
#include "dtkComposerGraphNode.h"
#include "dtkComposerNode.h"
#include "dtkComposerNodeComposite.h"
#include "dtkComposerNodeLeaf.h"
#include "dtkComposerScene.h"
#include "dtkComposerScene_p.h"
#include "dtkComposerSceneEdge.h"
#include "dtkComposerSceneNode.h"
#include "dtkComposerSceneNodeControl.h"
#include "dtkComposerSceneNodeComposite.h"
#include "dtkComposerSceneNodeLeaf.h"
#include "dtkComposerSceneNote.h"
#include "dtkComposerScenePort.h"
#include "dtkComposerStackCommand.h"
#include "dtkComposerStackUtils.h"
#include "dtkComposerTransmitter.h"
#include "dtkComposerTransmitterProxy.h"
#include "dtkComposerTransmitterVariant.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// Base Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandPrivate
{
public:
    dtkComposerFactory *factory;
    dtkComposerScene *scene;
    dtkComposerGraph *graph;
};

dtkComposerStackCommand::dtkComposerStackCommand(dtkComposerStackCommand *parent) : QUndoCommand(parent), d(new dtkComposerStackCommandPrivate)
{
    d->factory = NULL;
    d->scene = NULL;
    d->graph = NULL;
}

dtkComposerStackCommand::~dtkComposerStackCommand(void)
{
    delete d;

    d = NULL;
}

void dtkComposerStackCommand::setFactory(dtkComposerFactory *factory)
{
    d->factory = factory;
}

void dtkComposerStackCommand::setScene(dtkComposerScene *scene)
{
    d->scene = scene;
}

void dtkComposerStackCommand::setGraph(dtkComposerGraph *graph)
{
    d->graph = graph;
}

// /////////////////////////////////////////////////////////////////
// Create Node Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandCreateNodePrivate
{
public:
    QString name;
    QString type;
    QPointF position;

public:
    dtkComposerSceneNode *node;

public:
    dtkComposerSceneNodeComposite *parent;
};

dtkComposerStackCommandCreateNode::dtkComposerStackCommandCreateNode(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandCreateNodePrivate)
{
    e->node = NULL;
    e->parent = NULL;
}

dtkComposerStackCommandCreateNode::~dtkComposerStackCommandCreateNode(void)
{
    delete e->node;
    delete e;

    e = NULL;
}

void dtkComposerStackCommandCreateNode::setParent(dtkComposerSceneNodeComposite *parent)
{
    e->parent = parent;
}

void dtkComposerStackCommandCreateNode::setPosition(const QPointF& position)
{
    e->position = position;
}

void dtkComposerStackCommandCreateNode::setName(const QString& name)
{
    e->name = name;

    this->setText(QString("Create node %1").arg(e->name));
}

void dtkComposerStackCommandCreateNode::setType(const QString& type)
{
    e->type = type;
}

void dtkComposerStackCommandCreateNode::redo(void)
{
    if(!d->factory)
        return;

    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(e->type.isEmpty())
        return;

    if(!e->node) {

        dtkComposerNode *node = d->factory->create(e->type);

        if(dynamic_cast<dtkComposerNodeLeaf *>(node))
            e->node = new dtkComposerSceneNodeLeaf;
        else if(dynamic_cast<dtkComposerNodeComposite *>(node) )
            e->node = new dtkComposerSceneNodeComposite;
        else
            e->node = new dtkComposerSceneNodeControl;

        e->node->wrap(node);
        e->node->setParent(e->parent);
    }

    e->node->setPos(e->position);

    e->parent->addNode(e->node);
    e->parent->layout();

    d->graph->addNode(e->node);
    d->graph->layout();


// -- ??
    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->addItem(e->node);
    d->scene->modify(true);
// --
}

void dtkComposerStackCommandCreateNode::undo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->node)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    e->position = e->node->scenePos();


    d->graph->removeNode(e->node);
    d->graph->layout();

    e->parent->removeNode(e->node);
    e->parent->layout();
// -- ??
    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->removeItem(e->node);

    d->scene->modify(true);
// --
}

// /////////////////////////////////////////////////////////////////
// Destroy Node Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandDestroyNodePrivate
{
public:
    dtkComposerSceneNode *node;

public:
    dtkComposerSceneNodeComposite *parent;

public:
    QList<dtkComposerSceneEdge *>       input_edges;
    QList<dtkComposerSceneEdge *>      output_edges;
    QList<dtkComposerStackCommand *>  destroy_nodes;

};

dtkComposerStackCommandDestroyNode::dtkComposerStackCommandDestroyNode(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandDestroyNodePrivate)
{
    e->node = NULL;
    e->parent = NULL;
}

dtkComposerStackCommandDestroyNode::~dtkComposerStackCommandDestroyNode(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandDestroyNode::setNode(dtkComposerSceneNode *node)
{
    e->node = node;

    foreach(dtkComposerSceneEdge *edge, e->node->inputEdges())
        e->input_edges << edge;

    foreach(dtkComposerSceneEdge *edge, e->node->outputEdges())
        e->output_edges << edge;

    e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->node->parent());

    if (dtkComposerSceneNodeComposite *composite = dynamic_cast<dtkComposerSceneNodeComposite *>(node)) {

        foreach(dtkComposerSceneNode *subnode, composite->nodes()) {
            dtkComposerStackCommandDestroyNode *cmd = new dtkComposerStackCommandDestroyNode();
            cmd->setScene(d->scene);
            cmd->setGraph(d->graph);
            cmd->setNode(subnode);
            e->destroy_nodes << cmd;
        }
        foreach(dtkComposerSceneNote *subnote, composite->notes()) {
            dtkComposerStackCommandDestroyNote *cmd = new dtkComposerStackCommandDestroyNote();
            cmd->setScene(d->scene);
            cmd->setNote(subnote);
            e->destroy_nodes << cmd;
        }

    } else if (dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(node)) {

        foreach(dtkComposerSceneNodeComposite *block, control->blocks()) {
            foreach(dtkComposerSceneEdge *edge, block->inputEdges())
                e->input_edges << edge;
            foreach(dtkComposerSceneEdge *edge, block->outputEdges())
                e->output_edges << edge;
            foreach(dtkComposerSceneNode *subnode, block->nodes()) {
                dtkComposerStackCommandDestroyNode *cmd = new dtkComposerStackCommandDestroyNode();
                cmd->setScene(d->scene);
                cmd->setGraph(d->graph);
                cmd->setNode(subnode);
                e->destroy_nodes << cmd;
            }
            foreach(dtkComposerSceneNote *subnote, block->notes()) {
                dtkComposerStackCommandDestroyNote *cmd = new dtkComposerStackCommandDestroyNote();
                cmd->setScene(d->scene);
                cmd->setNote(subnote);
                e->destroy_nodes << cmd;
            }
        }
        if (control->header())
            e->input_edges << control->header()->inputEdges();
        if (control->footer())
            e->output_edges << control->footer()->outputEdges();

    }

    this->setText(QString("Destroy node %1").arg(e->node->title()));
}

void dtkComposerStackCommandDestroyNode::redo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    // destroy internal nodes of composite first
    foreach(dtkComposerStackCommand *cmd, e->destroy_nodes)
        cmd->redo();

    foreach(dtkComposerSceneEdge *edge, e->input_edges) {
        if (d->scene->items().contains(edge))
            d->scene->removeItem(edge);
        d->graph->removeEdge(edge);
        e->parent->removeEdge(edge);
    }

    foreach(dtkComposerSceneEdge *edge, e->output_edges) {
        if (d->scene->items().contains(edge))
            d->scene->removeItem(edge);
        d->graph->removeEdge(edge);
        e->parent->removeEdge(edge);
    }

    d->graph->removeNode(e->node);
    d->graph->layout();

    e->parent->removeNode(e->node);
    e->parent->layout();

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->removeItem(e->node);

    d->scene->modify(true);
}

void dtkComposerStackCommandDestroyNode::undo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    e->parent->addNode(e->node);
    d->graph->addNode(e->node);

    foreach(dtkComposerStackCommand *destroy_node, e->destroy_nodes)
        destroy_node->undo();

    e->parent->layout();

    foreach(dtkComposerSceneEdge *edge, e->input_edges) {
        if(!d->scene->items().contains(edge))
            d->scene->addItem(edge);
        e->parent->addEdge(edge);
        d->graph->addEdge(edge);
    }

    foreach(dtkComposerSceneEdge *edge, e->output_edges) {
        if(!d->scene->items().contains(edge))
            d->scene->addItem(edge);
        e->parent->addEdge(edge);
        d->graph->addEdge(edge);
    }

    d->graph->layout();

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->addItem(e->node);

    d->scene->modify(true);
}

// /////////////////////////////////////////////////////////////////
// Create Edge Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandCreateEdgePrivate
{
public:
    dtkComposerScenePort *source;
    dtkComposerScenePort *destination;

public:
    dtkComposerSceneEdge *edge;

public:
    dtkComposerSceneNodeComposite *parent;
};

dtkComposerStackCommandCreateEdge::dtkComposerStackCommandCreateEdge(void) : dtkComposerStackCommand(), e(new dtkComposerStackCommandCreateEdgePrivate)
{
    e->edge = NULL;
    e->source = NULL;
    e->destination = NULL;
    e->parent = NULL;

    this->setText("Create edge");
}

dtkComposerStackCommandCreateEdge::~dtkComposerStackCommandCreateEdge(void)
{
    delete e->edge;
    delete e;

    e = NULL;
}

void dtkComposerStackCommandCreateEdge::setSource(dtkComposerScenePort *source)
{
    e->source = source;

    this->setParent();
}

void dtkComposerStackCommandCreateEdge::setDestination(dtkComposerScenePort *destination)
{
    e->destination = destination;

    this->setParent();
}

void dtkComposerStackCommandCreateEdge::redo(void)
{
    if(!d->graph)
        return;

    if(!d->scene)
        return;

    if(!e->parent)
        return;

    if(!e->source)
        return;

    if(!e->destination)
        return;

    if(!e->edge) {
        e->edge = new dtkComposerSceneEdge;
        e->edge->setParent(e->parent);
        e->edge->setSource(e->source);
        e->edge->setDestination(e->destination);
    }

    e->edge->link();

    e->parent->addEdge(e->edge);

    if(e->parent->entered() || e->parent->flattened() || e->parent->root()) {

        d->scene->addItem(e->edge);

        if (d->scene->items().contains(e->source->node())) {
            if (e->source->node() != e->parent) {
                if (e->source->owner()->parent() == e->parent)
                    e->edge->stackBefore(e->source->owner());
                else
                    e->edge->stackBefore(e->source->node());
            }
        }

        if (d->scene->items().contains(e->destination->node())) {
            if (e->destination->node() != e->parent) {
                if (e->destination->owner()->parent() == e->parent)
                    e->edge->stackBefore(e->destination->owner());
                else
                    e->edge->stackBefore(e->destination->node());
            }
        }
    }

    d->scene->modify(true);

    // Setting up control flow

    d->graph->addEdge(e->edge);

    // Setting up data flow

    dtkComposerTransmitterConnection(d->scene->root(), e->parent, e->edge);
}

void dtkComposerStackCommandCreateEdge::undo(void)
{
    if(!d->scene)
        return;

    if(!e->parent)
        return;

    if(!e->edge)
        return;

    // Setting up data flow

    dtkComposerTransmitterDisconnection(d->scene->root(), e->parent, e->edge);

    // Setting up scene

    e->edge->unlink();

    e->parent->removeEdge(e->edge);

    // Setting up control flow

    d->graph->removeEdge(e->edge);

    if(e->parent->entered() || e->parent->flattened() || e->parent->root())
        d->scene->removeItem(e->edge);

    d->scene->modify(true);
}

void dtkComposerStackCommandCreateEdge::setParent(void)
{
    if(!e->source)
        return;

    if(!e->destination)
        return;

    if(e->source->node()->parent() == e->destination->node()->parent())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->source->node()->parent());
    else if(e->source->node() == e->destination->node()->parent())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->source->node());
    else if(e->destination->node() == e->source->node()->parent())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->destination->node());
    else if(e->source->node()->parent()->parent() == e->destination->node()->parent())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->source->node()->parent()->parent());
    else if(e->destination->node()->parent()->parent() == e->source->node()->parent())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->destination->node()->parent()->parent());
    else if(e->source->node()->parent()->parent() == e->destination->node())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->destination->node());
    else if(e->destination->node()->parent()->parent() == e->source->node())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->source->node());
    else if(e->source->node()->parent()->parent() == e->destination->node()->parent()->parent())
        e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->destination->node()->parent()->parent());
    else
        qDebug() << "dtkComposerStackCommandCreateEdge::setParent" << "Unhandled case" ;
}

// /////////////////////////////////////////////////////////////////
// Destroy Edge Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandDestroyEdgePrivate
{
public:
    dtkComposerSceneEdge *edge;

public:
    dtkComposerSceneNodeComposite *parent;
};

dtkComposerStackCommandDestroyEdge::dtkComposerStackCommandDestroyEdge(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandDestroyEdgePrivate)
{
    e->edge = NULL;
    e->parent = NULL;

    this->setText("Destroy edge");
}

dtkComposerStackCommandDestroyEdge::~dtkComposerStackCommandDestroyEdge(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandDestroyEdge::setEdge(dtkComposerSceneEdge *edge)
{
    e->edge = edge;

    e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(edge->parent());
}

void dtkComposerStackCommandDestroyEdge::redo(void)
{
    if(!e->edge) {
        dtkError() << "no edge in destroy edge!" ;
        return;
    }

    if(!e->parent) {
        dtkError() << "no parent in destroy edge!" ;
        return;
    }

    if(!d->graph) {
        dtkError() << "no graph in destroy edge!" ;
        return;
    }

    // Setting up data flow

    dtkComposerTransmitterDisconnection(d->scene->root(), e->parent, e->edge);

    // Setting up control flow

    d->graph->removeEdge(e->edge);

    // Setting up scene

    e->edge->unlink();

    e->parent->removeEdge(e->edge);

    if(e->parent->entered() || e->parent->flattened() || e->parent->root())
        d->scene->removeItem(e->edge);

    d->scene->modify(true);
}

void dtkComposerStackCommandDestroyEdge::undo(void)
{
    if(!e->edge) {
        dtkError() << "no edge in destroy edge undo!" ;
        return;
    }

    if(!e->parent) {
        dtkError() << "no parent in destroy edge undo!" ;
        return;
    }

    if(!d->graph) {
        dtkError() << "no graph in destroy edge undo!" ;
        return;
    }

    e->edge->link();

    e->parent->addEdge(e->edge);

    if(e->parent->entered() || e->parent->flattened() || e->parent->root())
        d->scene->addItem(e->edge);

    d->scene->modify(true);

    // Setting up control flow

    d->graph->addEdge(e->edge);

    // Setting up data flow

    dtkComposerTransmitterConnection(d->scene->root(), e->parent, e->edge);
}

// /////////////////////////////////////////////////////////////////
// Create Note Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandCreateNotePrivate
{
public:
    QPointF position;

public:
    dtkComposerSceneNote *note;

public:
    dtkComposerSceneNodeComposite *parent;
};

dtkComposerStackCommandCreateNote::dtkComposerStackCommandCreateNote(dtkComposerStackCommand *parent) : dtkComposerStackCommand(), e(new dtkComposerStackCommandCreateNotePrivate)
{
    e->note = NULL;
    e->parent = NULL;

    this->setText("Create note");
}

dtkComposerStackCommandCreateNote::~dtkComposerStackCommandCreateNote(void)
{
    delete e->note;
    delete e;

    e = NULL;
}

void dtkComposerStackCommandCreateNote::setParent(dtkComposerSceneNodeComposite *parent)
{
    e->parent = parent;
}

void dtkComposerStackCommandCreateNote::setPosition(const QPointF& position)
{
    e->position = position;
}

void dtkComposerStackCommandCreateNote::redo(void)
{
    if(!d->scene)
        return;

    if(!e->parent)
        return;

    if(!e->note) {
        e->note = new dtkComposerSceneNote;
        e->note->setParent(e->parent);
    }

    e->note->setPos(e->position);

    e->parent->addNote(e->note);

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->addItem(e->note);
    d->scene->modify(true);
}

void dtkComposerStackCommandCreateNote::undo(void)
{
    if(!e->parent)
        return;

    if(!e->note)
        return;

    e->position = e->note->scenePos();

    e->parent->removeNote(e->note);

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->removeItem(e->note);
    d->scene->modify(true);
}

// /////////////////////////////////////////////////////////////////
// Destroy Note Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandDestroyNotePrivate
{
public:
    QPointF position;

public:
    dtkComposerSceneNote *note;

public:
    dtkComposerSceneNodeComposite *parent;
};

dtkComposerStackCommandDestroyNote::dtkComposerStackCommandDestroyNote(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandDestroyNotePrivate)
{
    e->parent = NULL;
    e->note = NULL;

    this->setText("Destroy note");
}

dtkComposerStackCommandDestroyNote::~dtkComposerStackCommandDestroyNote(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandDestroyNote::setNote(dtkComposerSceneNote *note)
{
    e->note = note;

    e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->note->parent());
}

void dtkComposerStackCommandDestroyNote::redo(void)
{
    if(!d->scene)
        return;

    if(!e->parent)
        return;

    if(!e->note)
        return;

    e->position = e->note->pos();

    e->parent->removeNote(e->note);

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->removeItem(e->note);

    d->scene->modify(true);
}

void dtkComposerStackCommandDestroyNote::undo(void)
{
    if(!d->scene)
        return;

    if(!e->parent)
        return;

    if(!e->note)
        return;

    e->parent->addNote(e->note);

    e->note->setPos(e->position);

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->addItem(e->note);

    d->scene->modify(true);
}

// /////////////////////////////////////////////////////////////////
// Create Group Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandCreateGroupPrivate
{
public:
    dtkComposerSceneNodeComposite *node;

public:
    dtkComposerSceneNodeComposite *parent;

public:
    QList<dtkComposerSceneNote *> notes;

public:
    QList<dtkComposerStackCommandReparentNode *> reparent;

public:
    QPointF pos;
};

dtkComposerStackCommandCreateGroup::dtkComposerStackCommandCreateGroup(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandCreateGroupPrivate)
{
    e->parent = NULL;
    e->node   = NULL;

    this->setText("Create group");
}

dtkComposerStackCommandCreateGroup::~dtkComposerStackCommandCreateGroup(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandCreateGroup::setNodes(dtkComposerSceneNodeList nodes)
{
    QRectF rect;

    e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(nodes.first()->parent());

    if(!e->node && e->parent) {
        e->node = new dtkComposerSceneNodeComposite;
        e->node->wrap(new dtkComposerNodeComposite);
        e->node->setParent(e->parent);
    }

    foreach(dtkComposerSceneNode *node, nodes) {
        if(node->parent() == e->parent) {
            rect |= node->sceneBoundingRect();
        }
    }

    foreach(dtkComposerSceneNode *node, nodes) {
        if(node->parent() == e->parent) {
            e->reparent << new dtkComposerStackCommandReparentNode;
            e->reparent.last()->setFactory(d->factory);
            e->reparent.last()->setScene(d->scene);
            e->reparent.last()->setGraph(d->graph);
            e->reparent.last()->setOriginNode(node);
            e->reparent.last()->setOriginPosition(node->sceneBoundingRect().topLeft());
            e->reparent.last()->setTargetPosition(node->sceneBoundingRect().topLeft());
            e->reparent.last()->setTargetNode(e->node);
        }
    }

    e->pos = rect.center();
}

void dtkComposerStackCommandCreateGroup::setNotes(dtkComposerSceneNoteList notes)
{
    if(!e->parent)
        return;

    foreach(dtkComposerSceneNote *note, notes)
        if(note->parent() == e->parent)
            e->notes << note;
}

void dtkComposerStackCommandCreateGroup::redo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    e->parent->addNode(e->node);

    d->graph->addNode(e->node);
    d->graph->layout();

    e->node->setPos(e->pos - e->node->boundingRect().center());
    
    foreach(dtkComposerStackCommandReparentNode *cmd, e->reparent)
        cmd->redo();

    foreach(dtkComposerSceneNote *note, e->notes) {
        e->parent->removeNote(note);
        d->scene->removeItem(note);
        e->node->addNote(note);
        note->setParent(e->node);
    }

    d->scene->addItem(e->node);
    d->scene->modify(true);
    d->scene->update();

}

void dtkComposerStackCommandCreateGroup::undo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    foreach(dtkComposerSceneNote *note, e->notes) {
        e->parent->addNote(note);
        d->scene->addItem(note);
        e->node->removeNote(note);
        note->setParent(e->node->parent());
    }

    QListIterator<dtkComposerStackCommandReparentNode *> i(e->reparent);
    i.toBack();
    while (i.hasPrevious())
        i.previous()->undo();

   d->graph->removeNode(e->node);
   d->graph->layout();

    e->parent->removeNode(e->node);

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->removeItem(e->node);

    d->scene->update();
    d->scene->modify(true);
}

// /////////////////////////////////////////////////////////////////
// Destroy Group Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandDestroyGroupPrivate
{
public:
    dtkComposerSceneNodeComposite *node;

public:
    dtkComposerSceneNodeComposite *parent;

public:
    QList<dtkComposerSceneNode *> nodes;

public:
    QList<dtkComposerSceneNote *> notes;

public:
    QList<dtkComposerStackCommandReparentNode *> reparent;
};

dtkComposerStackCommandDestroyGroup::dtkComposerStackCommandDestroyGroup(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandDestroyGroupPrivate)
{
    e->parent = NULL;
    e->node = NULL;

    this->setText("Destroy group");
}

dtkComposerStackCommandDestroyGroup::~dtkComposerStackCommandDestroyGroup(void)
{
    delete e;
    
    e = NULL;
}

void dtkComposerStackCommandDestroyGroup::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node  = node;
    e->nodes = node->nodes();
    e->notes = node->notes();
    e->parent = dynamic_cast<dtkComposerSceneNodeComposite *>(node->parent());

    foreach(dtkComposerSceneNode *node, e->nodes) {
        e->reparent << new dtkComposerStackCommandReparentNode;
        e->reparent.last()->setFactory(d->factory);
        e->reparent.last()->setScene(d->scene);
        e->reparent.last()->setGraph(d->graph);
        e->reparent.last()->setOriginNode(node);
        e->reparent.last()->setOriginPosition(node->sceneBoundingRect().topLeft());
        e->reparent.last()->setTargetPosition(node->sceneBoundingRect().topLeft());
        e->reparent.last()->setTargetNode(e->parent);
    }

}

void dtkComposerStackCommandDestroyGroup::redo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    foreach(dtkComposerSceneNote *note, e->notes) {
        e->parent->addNote(note);
        d->scene->addItem(note);
        e->node->removeNote(note);
        note->setParent(e->node->parent());
    }
    
    foreach(dtkComposerStackCommandReparentNode *cmd, e->reparent)
        cmd->redo();

    d->graph->removeNode(e->node);
    d->graph->layout();

    e->parent->removeNode(e->node);

    d->scene->removeItem(e->node);
    d->scene->modify(true);
    d->scene->update();
}

void dtkComposerStackCommandDestroyGroup::undo(void)
{
    if(!d->scene)
        return;

    if(!d->graph)
        return;

    if(!e->parent)
        return;

    if(!e->node)
        return;

    e->parent->addNode(e->node);

    d->graph->addNode(e->node);
    d->graph->layout();

    QRectF rect;
    foreach(dtkComposerSceneNode *node, e->nodes) {
        rect |= node->sceneBoundingRect();
    }

    e->node->setPos(rect.center() - e->node->boundingRect().center());
    
    foreach(dtkComposerStackCommandReparentNode *cmd, e->reparent)
        cmd->undo();

    foreach(dtkComposerSceneNote *note, e->notes) {
        e->parent->removeNote(note);
        d->scene->removeItem(note);
        e->node->addNote(note);
        note->setParent(e->node);
    }

    if (e->parent->root() || e->parent->flattened() || e->parent->entered())
        d->scene->addItem(e->node);

    d->scene->update();
    d->scene->modify(true);
}

// /////////////////////////////////////////////////////////////////
// Enter Group Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandEnterGroupPrivate
{
public:
    dtkComposerSceneNodeComposite *node;

public:
    dtkComposerSceneNodeComposite *former;
};

dtkComposerStackCommandEnterGroup::dtkComposerStackCommandEnterGroup(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandEnterGroupPrivate)
{
    e->node = NULL;
    e->former = NULL;

    this->setText("Enter group");
}

dtkComposerStackCommandEnterGroup::~dtkComposerStackCommandEnterGroup(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandEnterGroup::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node = node;
}

void dtkComposerStackCommandEnterGroup::setFormer(dtkComposerSceneNodeComposite *former)
{
    e->former = former;
}

void dtkComposerStackCommandEnterGroup::redo(void)
{
    if(!e->node)
        return;

    e->node->setFormer(e->former);

    e->node->setUnrevealRect(e->node->sceneBoundingRect());
    e->node->setUnrevealPos(e->node->scenePos());

    d->scene->removeItem(e->former);

    e->node->enter();

    d->scene->addItem(e->node);
    d->scene->setCurrent(e->node);
    d->scene->update();
}

void dtkComposerStackCommandEnterGroup::undo(void)
{
    if(!e->node)
        return;

    d->scene->removeItem(e->node);

    e->node->leave();
    e->node->setPos(e->node->unrevealPos());
    e->node->layout();

    d->scene->addItem(e->former);
    d->scene->setCurrent(e->former);
    d->scene->update();
}

// /////////////////////////////////////////////////////////////////
// Leave Group Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandLeaveGroupPrivate
{
public:
    dtkComposerSceneNodeComposite *node;
};

dtkComposerStackCommandLeaveGroup::dtkComposerStackCommandLeaveGroup(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandLeaveGroupPrivate)
{
    e->node = NULL;

    this->setText("Leave group");
}

dtkComposerStackCommandLeaveGroup::~dtkComposerStackCommandLeaveGroup(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandLeaveGroup::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node = node;
}

void dtkComposerStackCommandLeaveGroup::redo(void)
{
    if(!e->node)
        return;

    d->scene->removeItem(e->node);

    e->node->leave();
    e->node->setPos(e->node->unrevealPos());
    e->node->layout();

    d->scene->addItem(e->node->former());
    d->scene->setCurrent(e->node->former());

    d->scene->update();
}

void dtkComposerStackCommandLeaveGroup::undo(void)
{
    if(!e->node)
        return;

    e->node->setUnrevealRect(e->node->sceneBoundingRect());
    e->node->setUnrevealPos(e->node->scenePos());

    d->scene->removeItem(e->node->former());

    e->node->enter();
    
    d->scene->addItem(e->node);
    d->scene->setCurrent(e->node);

    d->scene->update();
}

// /////////////////////////////////////////////////////////////////
// Flatten Group Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandFlattenGroupPrivate
{
public:
    dtkComposerSceneNodeComposite *node;
};

dtkComposerStackCommandFlattenGroup::dtkComposerStackCommandFlattenGroup(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandFlattenGroupPrivate)
{
    e->node = NULL;

    this->setText("Flatten group");
}

dtkComposerStackCommandFlattenGroup::~dtkComposerStackCommandFlattenGroup(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandFlattenGroup::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node = node;
}

void dtkComposerStackCommandFlattenGroup::redo(void)
{
    if(!e->node)
        return;

    e->node->setUnrevealPos(e->node->scenePos());

    d->scene->removeItem(e->node);

    e->node->flatten();
    e->node->layout();

    d->scene->addItem(e->node);


    d->scene->update();
}

void dtkComposerStackCommandFlattenGroup::undo(void)
{
    if(!e->node)
        return;

    d->scene->removeItem(e->node);

    e->node->unflatten();
    e->node->setPos(e->node->unrevealPos());
    e->node->layout();

    d->scene->addItem(e->node);
    d->scene->update();
}

// /////////////////////////////////////////////////////////////////
// Unflatten Group Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandUnflattenGroupPrivate
{
public:
    dtkComposerSceneNodeComposite *node;
};

dtkComposerStackCommandUnflattenGroup::dtkComposerStackCommandUnflattenGroup(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandUnflattenGroupPrivate)
{
    e->node = NULL;

    this->setText("Unflatten group");
}

dtkComposerStackCommandUnflattenGroup::~dtkComposerStackCommandUnflattenGroup(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandUnflattenGroup::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node = node;
}

void dtkComposerStackCommandUnflattenGroup::redo(void)
{
    if(!e->node)
        return;

    d->scene->removeItem(e->node);

    e->node->unflatten();
    e->node->setPos(e->node->unrevealPos());
    e->node->layout();

    d->scene->addItem(e->node);
    d->scene->update();
}

void dtkComposerStackCommandUnflattenGroup::undo(void)
{
    if(!e->node)
        return;

    e->node->setUnrevealPos(e->node->scenePos());

    d->scene->removeItem(e->node);

    e->node->flatten();
    e->node->layout();

    d->scene->addItem(e->node);
    d->scene->update();
}

// /////////////////////////////////////////////////////////////////
// Create Port Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandCreatePortPrivate
{
public:
    dtkComposerSceneNodeComposite *node;
    dtkComposerScenePort *port;

public:
    dtkComposerTransmitter *transmitter;

public:
    int type;

public:
    dtkComposerTransmitter::Kind kind;
};

dtkComposerStackCommandCreatePort::dtkComposerStackCommandCreatePort(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandCreatePortPrivate)
{
    e->node = NULL;
    e->port = NULL;
    e->transmitter = NULL;
    e->type = -1;
    e->kind = dtkComposerTransmitter::Proxy;

    this->setText("Create port");
}

dtkComposerStackCommandCreatePort::~dtkComposerStackCommandCreatePort(void)
{
    if (e->port)
        delete e->port;
    if (e->transmitter)
        delete e->transmitter;

    delete e;

    e = NULL;
}

dtkComposerScenePort *dtkComposerStackCommandCreatePort::port(void)
{
    return e->port;
}

void dtkComposerStackCommandCreatePort::setKind(dtkComposerTransmitter::Kind kind)
{
    e->kind = kind;
}

void dtkComposerStackCommandCreatePort::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node = node;
}

void dtkComposerStackCommandCreatePort::setType(int type)
{
    e->type = type;
}

void dtkComposerStackCommandCreatePort::redo(void)
{
    if(!d->scene)
        return;

    if(!e->node)
        return;

    if(e->type < 0)
        return;

    switch(e->type) {
    case dtkComposerScenePort::Input:
        if(!e->port)
            e->port = new dtkComposerScenePort(dtkComposerScenePort::Input, e->node);
        if(!e->transmitter) {
            switch(e->kind) {
            case dtkComposerTransmitter::Variant:
                e->transmitter = new dtkComposerTransmitterVariant(e->node->wrapee());
                break;
            default:
                e->transmitter = new dtkComposerTransmitterProxy(e->node->wrapee());
                break;
            }
        }
        e->node->addInputPort(e->port);
        e->node->layout();
        e->transmitter->setRequired(false);
        e->node->wrapee()->appendReceiver(e->transmitter);
        break;
    case dtkComposerScenePort::Output:
        if(!e->port)
            e->port = new dtkComposerScenePort(dtkComposerScenePort::Output, e->node);
        if(!e->transmitter) {
            switch(e->kind) {
            case dtkComposerTransmitter::Variant:
                e->transmitter = new dtkComposerTransmitterVariant(e->node->wrapee());
                break;
            default:
                e->transmitter = new dtkComposerTransmitterProxy(e->node->wrapee());
                break;
            }
        }
        e->node->addOutputPort(e->port);
        e->node->layout();
        e->transmitter->setRequired(false);
        e->node->wrapee()->appendEmitter(e->transmitter);
        break;
    default:
        break;
    }

    e->port->setVisible(true);

    d->scene->update();
}

void dtkComposerStackCommandCreatePort::undo(void)
{
    if(!e->node)
        return;

    if(!e->port)
        return;

    if(!e->transmitter)
        return;

    if(!d->scene)
        return;

    if(e->type < 0)
        return;

    switch(e->type) {
    case dtkComposerScenePort::Input:
        e->node->wrapee()->removeReceiver(e->transmitter);
        e->node->removeInputPort(e->port);
        e->node->layout();
        break;
    case dtkComposerScenePort::Output:
        e->node->wrapee()->removeEmitter(e->transmitter);
        e->node->removeOutputPort(e->port);
        e->node->layout();
        break;
    default:
        break;
    }

    e->port->setVisible(false);

    d->scene->update();
}

// /////////////////////////////////////////////////////////////////
// Destroy Port Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandDestroyPortPrivate
{
public:
    dtkComposerSceneNodeComposite *node;
    dtkComposerScenePort *port;

public:
    dtkComposerTransmitter *transmitter;

public:
    QList<dtkComposerStackCommandDestroyEdge *> destroy_left_edges;
    QList<dtkComposerStackCommandDestroyEdge *> destroy_right_edges;
};

dtkComposerStackCommandDestroyPort::dtkComposerStackCommandDestroyPort(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandDestroyPortPrivate)
{
    e->node = NULL;
    e->port = NULL;
    e->transmitter = NULL;

    this->setText("Destroy port");
}

dtkComposerStackCommandDestroyPort::~dtkComposerStackCommandDestroyPort(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandDestroyPort::setNode(dtkComposerSceneNodeComposite *node)
{
    e->node = node;
}

void dtkComposerStackCommandDestroyPort::setPort(dtkComposerScenePort *port)
{
    if (!e->node)
        return;

    e->port = port;

    switch(e->port->type()) {
    case dtkComposerScenePort::Input:
        e->transmitter = e->node->wrapee()->receivers().at(e->node->inputPorts().indexOf(e->port));
        foreach(dtkComposerSceneEdge *l_edge, e->node->inputEdges()) {
            if (l_edge->destination() == e->port) {
                e->destroy_left_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_left_edges.last()->setFactory(d->factory);
                e->destroy_left_edges.last()->setScene(d->scene);
                e->destroy_left_edges.last()->setGraph(d->graph);
                e->destroy_left_edges.last()->setEdge(l_edge);
            }
        }
        if (dtkComposerSceneNodeComposite *composite = dynamic_cast<dtkComposerSceneNodeComposite *>(e->node)) {
            foreach(dtkComposerSceneEdge *r_edge, composite->edges()) {
                if (r_edge->source() == e->port) {
                    e->destroy_right_edges << new dtkComposerStackCommandDestroyEdge;
                    e->destroy_right_edges.last()->setFactory(d->factory);
                    e->destroy_right_edges.last()->setScene(d->scene);
                    e->destroy_right_edges.last()->setGraph(d->graph);
                    e->destroy_right_edges.last()->setEdge(r_edge);
                }
            }
        }
        break;
    case dtkComposerScenePort::Output:
        e->transmitter = e->node->wrapee()->emitters().at(e->node->outputPorts().indexOf(e->port));
        foreach(dtkComposerSceneEdge *r_edge, e->node->outputEdges()) {
            if (r_edge->source() == e->port) {
                e->destroy_right_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_right_edges.last()->setFactory(d->factory);
                e->destroy_right_edges.last()->setScene(d->scene);
                e->destroy_right_edges.last()->setGraph(d->graph);
                e->destroy_right_edges.last()->setEdge(r_edge);
            }
        }
        if (dtkComposerSceneNodeComposite *composite = dynamic_cast<dtkComposerSceneNodeComposite *>(e->node)) {
            foreach(dtkComposerSceneEdge *l_edge, composite->edges()) {
                if (l_edge->destination() == e->port) {
                    e->destroy_left_edges << new dtkComposerStackCommandDestroyEdge;
                    e->destroy_left_edges.last()->setFactory(d->factory);
                    e->destroy_left_edges.last()->setScene(d->scene);
                    e->destroy_left_edges.last()->setGraph(d->graph);
                    e->destroy_left_edges.last()->setEdge(l_edge);
                }
            }
        }
        break;
    }
}

void dtkComposerStackCommandDestroyPort::redo(void)
{
    if(!e->node)
        return;

    if(!e->port)
        return;

    if(!e->transmitter)
        return;

    if(!d->scene)
        return;

    foreach(dtkComposerStackCommandDestroyEdge *destroy_left_edge, e->destroy_left_edges)
        destroy_left_edge->redo();
        
    foreach(dtkComposerStackCommandDestroyEdge *destroy_right_edge, e->destroy_right_edges)
        destroy_right_edge->redo();

    switch(e->port->type()) {
    case dtkComposerScenePort::Input:
        e->node->wrapee()->removeReceiver(e->transmitter);
        e->node->removeInputPort(e->port);
        e->node->layout();
        break;
    case dtkComposerScenePort::Output:
        e->node->removeOutputPort(e->port);
        e->node->wrapee()->removeEmitter(e->transmitter);
        e->node->layout();
        break;
    default:
        break;
    };

    e->port->setVisible(false);

    d->scene->update();
}

void dtkComposerStackCommandDestroyPort::undo(void)
{
    if(!e->node)
        return;

    if(!e->port)
        return;

    if(!e->transmitter)
        return;

    switch(e->port->type()) {
    case dtkComposerScenePort::Input:
        e->node->addInputPort(e->port);
        e->node->wrapee()->appendReceiver(e->transmitter);
        e->node->layout();
        break;
    case dtkComposerScenePort::Output:
        e->node->addOutputPort(e->port);
        e->node->wrapee()->appendEmitter(e->transmitter);
        e->node->layout();
        break;
    default:
        break;
    }

    e->port->setVisible(true);

    foreach(dtkComposerStackCommandDestroyEdge *destroy_left_edge, e->destroy_left_edges)
        destroy_left_edge->undo();

    foreach(dtkComposerStackCommandDestroyEdge *destroy_right_edge, e->destroy_right_edges)
        destroy_right_edge->undo();

    d->scene->update();
}

// /////////////////////////////////////////////////////////////////
// Reparent Node Command
// /////////////////////////////////////////////////////////////////

class dtkComposerStackCommandReparentNodePrivate
{
public:
    QPointF origin_pos;
    QPointF target_pos;

public:
    dtkComposerSceneNode *origin;
    dtkComposerSceneNode *target;

public:
    dtkComposerSceneNodeComposite *origin_parent;

public:
    QList<dtkComposerSceneEdge *>  input_edges;
    QList<dtkComposerSceneEdge *> output_edges;
    QList<dtkComposerScenePort *>        ports;

public:
    QList<dtkComposerStackCommandDestroyEdge *> destroy_input_edges;
    QList<dtkComposerStackCommandCreatePort *>  create_input_ports;
    QList<dtkComposerStackCommandCreateEdge *>  create_input_lhs_edges;
    QList<dtkComposerStackCommandCreateEdge *>  create_input_rhs_edges;

public:
    QList<dtkComposerStackCommandDestroyEdge *> destroy_output_edges;
    QList<dtkComposerStackCommandCreatePort *>  create_output_ports;
    QList<dtkComposerStackCommandCreateEdge *>  create_output_lhs_edges;
    QList<dtkComposerStackCommandCreateEdge *>  create_output_rhs_edges;

public:
    QList<dtkComposerStackCommandDestroyEdge *> destroy_intern_edges;
    QList<dtkComposerStackCommandCreateEdge *>  create_intern_edges;
    QList<dtkComposerStackCommandDestroyPort *> destroy_ports;
};

dtkComposerStackCommandReparentNode::dtkComposerStackCommandReparentNode(dtkComposerStackCommand *parent) : dtkComposerStackCommand(parent), e(new dtkComposerStackCommandReparentNodePrivate)
{
    e->origin_parent = NULL;
    e->origin = NULL;
    e->target = NULL;

    this->setText("Reparent node");
}

dtkComposerStackCommandReparentNode::~dtkComposerStackCommandReparentNode(void)
{
    delete e;

    e = NULL;
}

void dtkComposerStackCommandReparentNode::setOriginNode(dtkComposerSceneNode *node)
{
    e->origin = node;
    e->origin_parent = dynamic_cast<dtkComposerSceneNodeComposite *>(e->origin->parent());

}

void dtkComposerStackCommandReparentNode::setTargetNode(dtkComposerSceneNode *node)
{
    e->target = node;

    QList<dtkComposerSceneEdge *> target_input;
    QList<dtkComposerSceneEdge *> target_output;
    QList<dtkComposerSceneEdge *> target_intern;

    QHash<dtkComposerScenePort *, int> port_used;

    foreach(dtkComposerSceneEdge *edge, e->origin_parent->edges()) {

        if (edge->destination()->node() == e->target)
            target_input << edge;
        else if (edge->source()->node() == e->target)
            target_output << edge;

        if ((e->origin != edge->source()->owner()) && e->origin == edge->destination()->owner()) {
            if (!(edge->source()->node() == e->target)) {

                e->input_edges << edge;

                e->destroy_input_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_input_edges.last()->setFactory(d->factory);
                e->destroy_input_edges.last()->setScene(d->scene);
                e->destroy_input_edges.last()->setGraph(d->graph);

                e->create_input_ports << new dtkComposerStackCommandCreatePort;
                e->create_input_ports.last()->setFactory(d->factory);
                e->create_input_ports.last()->setScene(d->scene);
                e->create_input_ports.last()->setGraph(d->graph);

                e->create_input_lhs_edges << new dtkComposerStackCommandCreateEdge;
                e->create_input_lhs_edges.last()->setFactory(d->factory);
                e->create_input_lhs_edges.last()->setScene(d->scene);
                e->create_input_lhs_edges.last()->setGraph(d->graph);

                e->create_input_rhs_edges << new dtkComposerStackCommandCreateEdge;
                e->create_input_rhs_edges.last()->setFactory(d->factory);
                e->create_input_rhs_edges.last()->setScene(d->scene);
                e->create_input_rhs_edges.last()->setGraph(d->graph);
            } else {// source is target composite
                if (port_used.contains(edge->source()))
                    port_used[edge->source()] ++;
                else
                    port_used.insert(edge->source(), 1);
            }
        } else if ((e->origin != edge->destination()->owner()) && e->origin == edge->source()->owner()) {
            if (!(edge->destination()->node() == e->target)) {

                e->output_edges << edge;

                e->destroy_output_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_output_edges.last()->setFactory(d->factory);
                e->destroy_output_edges.last()->setScene(d->scene);
                e->destroy_output_edges.last()->setGraph(d->graph);

                e->create_output_ports << new dtkComposerStackCommandCreatePort;
                e->create_output_ports.last()->setFactory(d->factory);
                e->create_output_ports.last()->setScene(d->scene);
                e->create_output_ports.last()->setGraph(d->graph);

                e->create_output_lhs_edges << new dtkComposerStackCommandCreateEdge;
                e->create_output_lhs_edges.last()->setFactory(d->factory);
                e->create_output_lhs_edges.last()->setScene(d->scene);
                e->create_output_lhs_edges.last()->setGraph(d->graph);

                e->create_output_rhs_edges << new dtkComposerStackCommandCreateEdge;
                e->create_output_rhs_edges.last()->setFactory(d->factory);
                e->create_output_rhs_edges.last()->setScene(d->scene);
                e->create_output_rhs_edges.last()->setGraph(d->graph);
            } else { // destination is target composite
                if (port_used.contains(edge->destination()))
                    port_used[edge->destination()] ++;
                else
                    port_used.insert(edge->destination(), 1);
            }
        } else if (edge->destination()->node() == e->target) {
            if (port_used.contains(edge->destination()))
                port_used[edge->destination()] ++;
            else
                port_used.insert(edge->destination(), 1);

        } else if ( edge->source()->node() == e->target) {
            if (port_used.contains(edge->source()))
                port_used[edge->source()] ++;
            else
                port_used.insert(edge->source(), 1);
        }
    }

    // search intern edges (connected to a port) of target composite
    foreach(dtkComposerSceneEdge *edge, dynamic_cast<dtkComposerSceneNodeComposite *>(e->target)->edges() )
        if ((edge->source()->node() == e->target) || (edge->destination()->node() == e->target))
            target_intern << edge;

    foreach(dtkComposerSceneEdge *edge, target_input) {
        if (edge->source()->owner() == e->origin ) {
            // edge from node to composite; if the composite port is
            // not connected inside, remove the edge, else connect
            // directly the nodes inside the composite and destroy the
            // port and old edge; if the port is also used inside the
            // composite, don't remove the port
            int count = 0;
            foreach(dtkComposerSceneEdge *i_edge, target_intern) {
                if (i_edge->source() == edge->destination()) { // same port
                    e->create_intern_edges << new dtkComposerStackCommandCreateEdge;
                    e->create_intern_edges.last()->setFactory(d->factory);
                    e->create_intern_edges.last()->setScene(d->scene);
                    e->create_intern_edges.last()->setGraph(d->graph);
                    e->create_intern_edges.last()->setSource(edge->source());
                    e->create_intern_edges.last()->setDestination(i_edge->destination());

                    e->destroy_intern_edges << new dtkComposerStackCommandDestroyEdge;
                    e->destroy_intern_edges.last()->setFactory(d->factory);
                    e->destroy_intern_edges.last()->setScene(d->scene);
                    e->destroy_intern_edges.last()->setGraph(d->graph);
                    e->destroy_intern_edges.last()->setEdge(edge);
                    port_used[edge->destination()] --;
                    count ++;
                }
            }
            if (count == 0) { // no intern edge connected to the port, we can remove this edge
                e->destroy_intern_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_intern_edges.last()->setFactory(d->factory);
                e->destroy_intern_edges.last()->setScene(d->scene);
                e->destroy_intern_edges.last()->setGraph(d->graph);
                e->destroy_intern_edges.last()->setEdge(edge);
            }
        }
    }

    foreach(dtkComposerSceneEdge *edge, target_output) {
        if (edge->destination()->owner() == e->origin ) {
            // edge from node to composite; if the composite port is
            // not connected inside, remove the edge, else connect
            // directly the nodes inside the composite and destroy the
            // port and old edge; if the port is also used inside the
            // composite, don't remove the port
            int count = 0;
            foreach(dtkComposerSceneEdge *i_edge, target_intern) {
                if (i_edge->destination() == edge->source()) { // same port
                    e->create_intern_edges << new dtkComposerStackCommandCreateEdge;
                    e->create_intern_edges.last()->setFactory(d->factory);
                    e->create_intern_edges.last()->setScene(d->scene);
                    e->create_intern_edges.last()->setGraph(d->graph);
                    e->create_intern_edges.last()->setSource(i_edge->source());
                    e->create_intern_edges.last()->setDestination(edge->destination());

                    e->destroy_intern_edges << new dtkComposerStackCommandDestroyEdge;
                    e->destroy_intern_edges.last()->setFactory(d->factory);
                    e->destroy_intern_edges.last()->setScene(d->scene);
                    e->destroy_intern_edges.last()->setGraph(d->graph);
                    e->destroy_intern_edges.last()->setEdge(edge);
                    port_used[edge->source()] --;
                    count ++;
                }
            }
            if (count == 0) { // no intern edge connected to the port, we can remove this edge
                e->destroy_intern_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_intern_edges.last()->setFactory(d->factory);
                e->destroy_intern_edges.last()->setScene(d->scene);
                e->destroy_intern_edges.last()->setGraph(d->graph);
                e->destroy_intern_edges.last()->setEdge(edge);
            }
        }
    }

    foreach(dtkComposerSceneEdge *edge, target_intern) {
        if (edge->source()->node() == e->target ) {
            if (port_used.contains(edge->source()) && port_used[edge->source()] == 0 ) { // port is not used outside, remove edge
                e->destroy_intern_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_intern_edges.last()->setFactory(d->factory);
                e->destroy_intern_edges.last()->setScene(d->scene);
                e->destroy_intern_edges.last()->setGraph(d->graph);
                e->destroy_intern_edges.last()->setEdge(edge);

                // we can't SetPort now, otherwise, it will add
                // destroy edges commands for edges that will be
                // deleted by us, so store the port, and to the Set Port in redo.

                e->ports << edge->source();
                e->destroy_ports << new dtkComposerStackCommandDestroyPort;
                e->destroy_ports.last()->setFactory(d->factory);
                e->destroy_ports.last()->setScene(d->scene);
                e->destroy_ports.last()->setGraph(d->graph);
                e->destroy_ports.last()->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(e->target));

            }
        } else if (edge->destination()->node() == e->target ) {
            if (port_used.contains(edge->destination()) && port_used[edge->destination()] == 0) { // port is not used outside, remove edge
                e->destroy_intern_edges << new dtkComposerStackCommandDestroyEdge;
                e->destroy_intern_edges.last()->setFactory(d->factory);
                e->destroy_intern_edges.last()->setScene(d->scene);
                e->destroy_intern_edges.last()->setGraph(d->graph);
                e->destroy_intern_edges.last()->setEdge(edge);

                e->ports << edge->destination();
                e->destroy_ports << new dtkComposerStackCommandDestroyPort;
                e->destroy_ports.last()->setFactory(d->factory);
                e->destroy_ports.last()->setScene(d->scene);
                e->destroy_ports.last()->setGraph(d->graph);
                e->destroy_ports.last()->setNode(dynamic_cast<dtkComposerSceneNodeComposite *>(e->target));
            }
        }
    }

}

void dtkComposerStackCommandReparentNode::setOriginPosition(QPointF position)
{
    e->origin_pos = position;
}

void dtkComposerStackCommandReparentNode::setTargetPosition(QPointF position)
{
    e->target_pos = position;
}

void dtkComposerStackCommandReparentNode::redo(void)
{
    if(e->origin_pos.isNull())
        return;

    if(e->target_pos.isNull())
        return;

    if(!e->origin)
        return;

    if(!e->target)
        return;

    if(!d->graph)
        return;

    if (dtkComposerSceneNodeComposite *target = dynamic_cast<dtkComposerSceneNodeComposite *>(e->target)) {

        for(int i = 0; i < e->input_edges.count(); i++) {
            e->destroy_input_edges.at(i)->setEdge(e->input_edges.at(i));
            e->destroy_input_edges.at(i)->redo();
        }

        for(int i = 0; i < e->output_edges.count(); i++) {
            e->destroy_output_edges.at(i)->setEdge(e->output_edges.at(i));
            e->destroy_output_edges.at(i)->redo();
        }

        foreach(dtkComposerStackCommandDestroyEdge *destroy_edge, e->destroy_intern_edges)
            destroy_edge->redo();

        for(int i = 0; i < e->ports.count(); i++) {
            e->destroy_ports.at(i)->setPort(e->ports.at(i));
            e->destroy_ports.at(i)->redo();
        }

        d->graph->reparentNode(e->origin, e->target);

        e->origin_parent->removeNode(e->origin);

        if (e->origin_parent->flattened() || e->origin_parent == d->scene->current()) {
            d->scene->removeItem(e->origin);
        }

        target->addNode(e->origin);
        e->origin->setParent(target);            

        if (target == d->scene->current()) {
            d->scene->addItem(e->origin);

        } else if (target->flattened()) {
            d->scene->addItem(e->origin);
            if (target->embedded() && !target->entered())
                target->parent()->stackBefore(e->origin);
            else
                target->stackBefore(e->origin);
            
        }
        e->origin->setPos(e->target_pos);

        for(int i = 0; i < e->input_edges.count(); i++) {
            e->create_input_ports.at(i)->setNode(target);
            e->create_input_ports.at(i)->setType(dtkComposerScenePort::Input);
            e->create_input_ports.at(i)->redo();

            e->create_input_lhs_edges.at(i)->setSource(e->input_edges.at(i)->source());
            e->create_input_lhs_edges.at(i)->setDestination(target->inputPorts().last());
            e->create_input_lhs_edges.at(i)->redo();

            e->create_input_rhs_edges.at(i)->setSource(target->inputPorts().last());
            e->create_input_rhs_edges.at(i)->setDestination(e->input_edges.at(i)->destination());
            e->create_input_rhs_edges.at(i)->redo();
        }

        for(int i = 0; i < e->output_edges.count(); i++) {
            e->create_output_ports.at(i)->setNode(target);
            e->create_output_ports.at(i)->setType(dtkComposerScenePort::Output);
            e->create_output_ports.at(i)->redo();

            e->create_output_lhs_edges.at(i)->setSource(e->output_edges.at(i)->source());
            e->create_output_lhs_edges.at(i)->setDestination(target->outputPorts().last());
            e->create_output_lhs_edges.at(i)->redo();

            e->create_output_rhs_edges.at(i)->setSource(target->outputPorts().last());
            e->create_output_rhs_edges.at(i)->setDestination(e->output_edges.at(i)->destination());
            e->create_output_rhs_edges.at(i)->redo();
        }

        foreach(dtkComposerStackCommandCreateEdge *create_edge, e->create_intern_edges){
            create_edge->setParent();
            create_edge->redo();
        }

        target->layout();
        d->graph->layout();
    }

    d->scene->modify(true);
    d->scene->update();
}

void dtkComposerStackCommandReparentNode::undo(void)
{
    if(e->origin_pos.isNull())
        return;

    if(e->target_pos.isNull())
        return;

    if(!e->origin)
        return;

    if(!e->target)
        return;

    if(!d->graph)
        return;

    if(dtkComposerSceneNodeComposite *target = dynamic_cast<dtkComposerSceneNodeComposite *>(e->target)) {

        foreach(dtkComposerStackCommandCreateEdge *create_edge, e->create_intern_edges)
            create_edge->undo();

        for(int i = 0; i < e->output_edges.count(); i++) {
            e->create_output_rhs_edges.at(i)->undo();
            e->create_output_lhs_edges.at(i)->undo();
            e->create_output_ports.at(i)->undo();
        }

        for(int i = 0; i < e->input_edges.count(); i++) {
            e->create_input_rhs_edges.at(i)->undo();
            e->create_input_lhs_edges.at(i)->undo();
            e->create_input_ports.at(i)->undo();
        }

        d->graph->reparentNode(e->origin, e->origin_parent);

        target->removeNode(e->origin);

        if (target->flattened()) {
            target->layout();
            d->scene->removeItem(e->origin);
        } else if (target == d->scene->current()) {
            d->scene->removeItem(e->origin);
        }

        e->origin_parent->addNode(e->origin);
        e->origin->setParent(e->origin_parent);

        e->origin->setPos(e->origin_pos);

       for(int i = 0; i < e->ports.count(); i++)
            e->destroy_ports.at(i)->undo();

       foreach(dtkComposerStackCommandDestroyEdge *destroy_edge, e->destroy_intern_edges)
            destroy_edge->undo();

       for(int i = 0; i < e->output_edges.count(); i++)
           e->destroy_output_edges.at(i)->undo();

       for(int i = 0; i < e->input_edges.count(); i++)
           e->destroy_input_edges.at(i)->undo();

        if (e->origin_parent->flattened() || e->origin_parent->entered() || e->origin_parent->root())
            d->scene->addItem(e->origin);

        e->origin_parent->layout();
    }

    d->scene->modify(true);
    d->scene->update();
}
