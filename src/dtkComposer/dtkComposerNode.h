/* dtkComposerNode.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Sep  7 13:48:02 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jul 13 10:20:49 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 92
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERNODE_H
#define DTKCOMPOSERNODE_H

#include "dtkComposerExport.h"

#include <QtCore>
#include <QtGui>

class dtkAbstractObject;
class dtkComposerEdge;
class dtkComposerNodePrivate;
class dtkComposerNodeProperty;

class DTKCOMPOSER_EXPORT dtkComposerNode : public QObject, public QGraphicsItem
{
    Q_OBJECT

#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif

public:
    enum Type {
        Unknown,
        Atomic,
        Control,
        Data,
        Process,
        View
    };

     dtkComposerNode(dtkComposerNode *parent = 0);
    ~dtkComposerNode(void);

    void setTitle(const QString& title);
    void setType(Type type);
    void setObject(dtkAbstractObject *object);

    void addInputProperty(dtkComposerNodeProperty *property);
    void addOutputProperty(dtkComposerNodeProperty *property);

    void addInputEdge(dtkComposerEdge *edge, dtkComposerNodeProperty *property);
    void addOutputEdge(dtkComposerEdge *edge, dtkComposerNodeProperty *property);

    void removeInputEdge(dtkComposerEdge *edge);
    void removeOutputEdge(dtkComposerEdge *edge);

    void addAction(const QString& text, const QObject *receiver, const char *slot);

    int count(dtkComposerNodeProperty *property);

    Type type(void);

    dtkAbstractObject *object(void);

    QList<dtkComposerNodeProperty *> inputProperties(void);
    QList<dtkComposerNodeProperty *> outputProperties(void);

    QList<dtkComposerEdge *> inputEdges(void);
    QList<dtkComposerEdge *> outputEdges(void);

    QList<dtkComposerNode *> inputNodes(void);
    QList<dtkComposerNode *> outputNodes(void);

    dtkComposerEdge *edge(dtkComposerNodeProperty *property);

    dtkComposerNodeProperty *propertyAt(const QPointF& point) const;

    QRectF boundingRect(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    dtkComposerNodePrivate *d;
};

#endif
