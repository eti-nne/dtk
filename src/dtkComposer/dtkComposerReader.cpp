/* dtkComposerReader.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Mon Jan 30 23:41:08 2012 (+0100)
 * Version: $Id$
 * Last-Updated: mer. mai 16 13:49:10 2012 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 644
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkComposerFactory.h"
#include "dtkComposerGraph.h"
#include "dtkComposerNodeBoolean.h"
#include "dtkComposerNodeComposite.h"
#include "dtkComposerNodeControl.h"
#include "dtkComposerNodeControlCase.h"
#include "dtkComposerNodeInteger.h"
#include "dtkComposerNodeReal.h"
#include "dtkComposerNodeString.h"
#include "dtkComposerReader.h"
#include "dtkComposerScene.h"
#include "dtkComposerScene_p.h"
#include "dtkComposerSceneEdge.h"
#include "dtkComposerSceneNode.h"
#include "dtkComposerSceneNodeComposite.h"
#include "dtkComposerSceneNodeControl.h"
#include "dtkComposerSceneNodeLeaf.h"
#include "dtkComposerSceneNote.h"
#include "dtkComposerScenePort.h"
#include "dtkComposerStackUtils.h"
#include "dtkComposerTransmitter.h"
#include "dtkComposerTransmitterProxy.h"
#include "dtkComposerTransmitterVariant.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>
#include <QtXml>

// /////////////////////////////////////////////////////////////////
// dtkComposerReaderPrivate
// /////////////////////////////////////////////////////////////////

class dtkComposerReaderPrivate
{
public:
    bool check(const QDomDocument& document);

public:
    dtkComposerFactory *factory;
    dtkComposerScene *scene;
    dtkComposerGraph *graph;

public:
    QHash<int, dtkComposerSceneNode *> node_map;

public:
    dtkComposerSceneNodeComposite *root;
    dtkComposerSceneNodeComposite *node;

public:
    dtkComposerSceneNodeControl *control;
};

bool dtkComposerReaderPrivate::check(const QDomDocument& document)
{
    Q_UNUSED(document);

    return true;
}

// /////////////////////////////////////////////////////////////////
// dtkComposerReader
// /////////////////////////////////////////////////////////////////

dtkComposerReader::dtkComposerReader(void) : d(new dtkComposerReaderPrivate)
{
    d->factory = NULL;
    d->scene = NULL;
    d->root = NULL;
    d->graph = NULL;

    d->control = NULL;
}

dtkComposerReader::~dtkComposerReader(void)
{
    delete d;

    d = NULL;
}

void dtkComposerReader::setFactory(dtkComposerFactory *factory)
{
    d->factory = factory;
}

void dtkComposerReader::setScene(dtkComposerScene *scene)
{
    d->scene = scene;
}

void dtkComposerReader::setGraph(dtkComposerGraph *graph)
{
    d->graph = graph;
}

bool dtkComposerReader::read(const QString& fileName, bool append)
{

    QString content;

    QFile file(fileName);

    QByteArray c_bytes;

    if (!file.open(QIODevice::ReadOnly))
        return false;


    if (!dtkIsBinary(fileName)) {
        c_bytes = file.readAll();
        content = QString::fromUtf8(c_bytes.data());

    } else {
        QDataStream stream(&file); stream >> c_bytes;
        QByteArray u_bytes = QByteArray::fromHex(qUncompress(c_bytes));
        content = QString::fromUtf8(u_bytes.data(), u_bytes.size());
    }

    file.close();
    return this->readString(content);
}

bool dtkComposerReader::readString(const QString& data, bool append)
{
    QDomDocument document("dtk");

    if (!document.setContent(data)) {
        qDebug()<< "reader: no content"<< data.size();
        return false;
    }

    if(!d->check(document))
        return false;

    // Clear scene if applicable

    if(!append) {
        d->scene->clear();
        d->graph->clear();
    }

    d->node_map.clear();

    // --

    d->root = d->scene->root();

    if(!append) {
        d->node = d->root;
        d->graph->addNode(d->root);
    } else {
        d->node = new dtkComposerSceneNodeComposite;
        d->node->wrap(new dtkComposerNodeComposite);
        d->node->setParent(d->scene->current());
        d->scene->current()->addNode(d->node);
        d->graph->addNode(d->node);
    }

    // Feeding scene with notes
    
    QDomNodeList notes = document.firstChild().childNodes();

    for(int i = 0; i < notes.count(); i++)    
        if(notes.at(i).toElement().tagName() == "note")
            this->readNote(notes.at(i));

    // Feeding scene with nodes

    QDomNodeList nodes = document.firstChild().childNodes();

    for(int i = 0; i < nodes.count(); i++)
        if(nodes.at(i).toElement().tagName() == "node")
            this->readNode(nodes.at(i));

    // Feeding scene with edges

    QDomNodeList edges = document.firstChild().childNodes();

    for(int i = 0; i < edges.count(); i++)
        if(edges.at(i).toElement().tagName() == "edge")
            this->readEdge(edges.at(i));

    // --
    
    if(!append) {
        d->scene->setRoot(d->root);
    } else {
        d->scene->addItem(d->node);
    }

    d->graph->layout();

    // --
    
    return true;
}

dtkComposerSceneNote *dtkComposerReader::readNote(QDomNode node)
{
    qreal x = node.toElement().attribute("x").toFloat();
    qreal y = node.toElement().attribute("y").toFloat();
    qreal w = node.toElement().attribute("w").toFloat();
    qreal h = node.toElement().attribute("h").toFloat();
    
    dtkComposerSceneNote *note = new dtkComposerSceneNote;
    note->setPos(QPointF(x, y));
    note->setSize(QSizeF(w, h));
    note->setText(node.childNodes().at(0).toText().data());
    
    d->node->addNote(note);

    note->setParent(d->node);

    return note;
}

dtkComposerSceneNode *dtkComposerReader::readNode(QDomNode node)
{
    QDomNodeList childNodes = node.childNodes();

    QList<QDomNode> ports;
    QList<QDomNode> notes;
    QList<QDomNode> nodes;
    QList<QDomNode> edges;
    QList<QDomNode> blocks;

    QDomNode header;
    QDomNode footer;

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "port")
            ports << childNodes.at(i);

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "note")
            notes << childNodes.at(i);

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "node")
            nodes << childNodes.at(i);

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "edge")
            edges << childNodes.at(i);

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "block")
            blocks << childNodes.at(i);

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "header")
            header = childNodes.at(i);

    for(int i = 0; i < childNodes.count(); i++)
        if(childNodes.at(i).toElement().tagName() == "footer")
            footer = childNodes.at(i);

    // --

    dtkComposerSceneNode *n = NULL;
    QString type_n = node.toElement().attribute("type");
    if(blocks.count()) {

        qreal w = node.toElement().attribute("w").toFloat();
        qreal h = node.toElement().attribute("h").toFloat();

        n = new dtkComposerSceneNodeControl;
        n->wrap(d->factory->create(node.toElement().attribute("type")));
        n->setParent(d->node);
        d->node->addNode(n);
        d->graph->addNode(n);
        n->resize(w, h);

    } else if(node.toElement().tagName() == "block") {

        if (dtkComposerNodeControlCase *control = dynamic_cast<dtkComposerNodeControlCase *>(d->control->wrapee())) {
                dtkComposerSceneNodeComposite *b = new dtkComposerSceneNodeComposite;
                control->addBlock();
                b->wrap(control->block( control->blockCount()-1));
                d->control->addBlock(b);
                d->graph->addBlock(d->control);
                n = d->control->blocks().last();

        } else {
            n = d->control->blocks().at(node.toElement().attribute("blockid").toInt());
        }

    } else if(node.toElement().tagName() == "header") {

        n = d->control->header();

    } else if(node.toElement().tagName() == "footer") {

        n = d->control->footer();

    } else if( type_n == "composite" || type_n == "world" || type_n == "remote") {

        n = new dtkComposerSceneNodeComposite;
        n->wrap(d->factory->create(type_n));
        n->setParent(d->node);
        d->node->addNode(n);
        d->graph->addNode(n);

    } else {

        n = new dtkComposerSceneNodeLeaf;
        n->wrap(d->factory->create(node.toElement().attribute("type")));
        n->setParent(d->node);
        d->node->addNode(n);
        d->graph->addNode(n);

    }

    QPointF position;
    
    if(node.toElement().hasAttribute("x"))
        position.setX(node.toElement().attribute("x").toFloat());
    
    if(node.toElement().hasAttribute("y"))
        position.setY(node.toElement().attribute("y").toFloat());

    n->setPos(position);

    if(node.toElement().hasAttribute("title"))
        n->setTitle(node.toElement().attribute("title"));

    int id = node.toElement().attribute("id").toInt();

    d->node_map.insert(id, n);

    // --

    dtkComposerSceneNodeComposite *t = d->node;

    if(dtkComposerSceneNodeControl *control = dynamic_cast<dtkComposerSceneNodeControl *>(n)) {

        control->layout();

        d->control = control;
        this->readNode(header);

        for(int i = 0; i < blocks.count(); i++) {
            d->control = control;
            this->readNode(blocks.at(i));
        }

        d->control = control;
        this->readNode(footer);

// --- next/previous for loops

        if(dtkComposerSceneNodeComposite *body = control->block("Body")) {

            foreach(dtkComposerScenePort *port, body->inputPorts()) {

                if(int loop = port->loop()) {
                    
                    dtkComposerTransmitter *variant = port->node()->wrapee()->receivers().at(port->node()->inputPorts().indexOf(port));
                    
                    if(dtkComposerSceneNodeComposite *conditional = control->block("Conditional")) {

                        foreach(dtkComposerScenePort *i, conditional->inputPorts()) {

                            if(i->loop() == loop) {

                                dtkComposerTransmitter *proxy = i->node()->wrapee()->receivers().at(i->node()->inputPorts().indexOf(i));

                                dtkComposerTransmitterLinkList   valid_edges;
                                dtkComposerTransmitterLinkList invalid_edges;
                                dtkComposerTransmitter::onTransmittersConnected(variant, proxy, valid_edges, invalid_edges);
                                dtkComposerPropagateEdgeValidity(d->root, valid_edges, invalid_edges);

                            }
                        }
                    }
                }
            }


            foreach(dtkComposerScenePort *port, body->outputPorts()) {

                if(int loop = port->loop()) {
                    
                    dtkComposerTransmitter *variant = port->node()->wrapee()->emitters().at(port->node()->outputPorts().indexOf(port));
                    
                    if(dtkComposerSceneNodeComposite *increment = control->block("Increment")) {

                        foreach(dtkComposerScenePort *i, increment->inputPorts()) {

                            if(i->loop() == loop) {

                                dtkComposerTransmitter *proxy = i->node()->wrapee()->receivers().at(i->node()->inputPorts().indexOf(i));

                                dtkComposerTransmitterLinkList   valid_edges;
                                dtkComposerTransmitterLinkList invalid_edges;
                                dtkComposerTransmitter::onTransmittersConnected(variant, proxy, valid_edges, invalid_edges);
                                dtkComposerPropagateEdgeValidity(d->root, valid_edges, invalid_edges);

                            }
                        }
                    }
                }
            }
        }
// ---
    }

    if(dtkComposerSceneNodeComposite *composite = dynamic_cast<dtkComposerSceneNodeComposite *>(n)) {

        d->node = composite;

        for(int i = 0; i < ports.count(); i++) {
            if(ports.at(i).toElement().attribute("type") == "input") {
                dtkComposerScenePort *port = new dtkComposerScenePort(dtkComposerScenePort::Input, composite);
                if (ports.at(i).toElement().hasAttribute("label"))
                    port->setLabel(ports.at(i).toElement().attribute("label"));
                composite->addInputPort(port);
                if (ports.at(i).toElement().attribute("kind") == "proxy") {
                    dtkComposerTransmitter *proxy = new dtkComposerTransmitterProxy(composite->wrapee());
                    proxy->setRequired(false);
                    composite->wrapee()->appendReceiver(proxy);
                }
                if (ports.at(i).toElement().attribute("kind") == "variant") {
                    dtkComposerTransmitter *variant = new dtkComposerTransmitterVariant(composite->wrapee());
                    variant->setRequired(false);
                    composite->wrapee()->appendReceiver(variant);
                }
                if (ports.at(i).toElement().hasAttribute("loop"))
                    port->setLoop(ports.at(i).toElement().attribute("loop").toInt());
            } else {
                dtkComposerScenePort *port = new dtkComposerScenePort(dtkComposerScenePort::Output, composite);
                if (ports.at(i).toElement().hasAttribute("label"))
                    port->setLabel(ports.at(i).toElement().attribute("label"));
                composite->addOutputPort(port);
                if (ports.at(i).toElement().attribute("kind") == "proxy") {
                    dtkComposerTransmitter *proxy = new dtkComposerTransmitterProxy(composite->wrapee());
                    proxy->setRequired(false);
                    composite->wrapee()->appendEmitter(proxy);
                }
                if (ports.at(i).toElement().attribute("kind") == "variant") {
                    dtkComposerTransmitter *variant = new dtkComposerTransmitterVariant(composite->wrapee());
                    variant->setRequired(false);
                    composite->wrapee()->appendEmitter(variant);
                }
                if (ports.at(i).toElement().hasAttribute("loop"))
                    port->setLoop(ports.at(i).toElement().attribute("loop").toInt());

// ---- twins for loops

                if (ports.at(i).toElement().hasAttribute("twin")) {
                    
                    int twin = ports.at(i).toElement().attribute("twin").toInt();
                    
                    QString block;
                    
                    if (ports.at(i).toElement().hasAttribute("block"))
                        block = ports.at(i).toElement().attribute("block");
                    
                    dtkComposerNode *wrapee = NULL;
                    
                    if(block.isEmpty())
                        wrapee = composite->wrapee();
                    else if (d->control) {
                        wrapee = d->control->block(block)->wrapee();
                    }
                    
                    if(wrapee) {
                        
                        dtkComposerTransmitterVariant *variant = dynamic_cast<dtkComposerTransmitterVariant *>(composite->wrapee()->emitters().last());
                        
                        if(variant) {

                            variant->setTwin(dynamic_cast<dtkComposerTransmitterVariant *>(wrapee->receivers().at(twin)));

                            dynamic_cast<dtkComposerNodeControl *>(d->control->wrapee())->appendInputTwin(variant->twin());
                            dynamic_cast<dtkComposerNodeControl *>(d->control->wrapee())->appendOutputTwin(variant);

                        }
                    }
                }
// ----------

            }
        }

        composite->layout();

        for(int i = 0; i < notes.count(); i++)
            this->readNote(notes.at(i));
        
        for(int i = 0; i < nodes.count(); i++)
            this->readNode(nodes.at(i));
        
        for(int i = 0; i < edges.count(); i++)
            this->readEdge(edges.at(i));
    }

    if(dtkComposerSceneNodeLeaf *leaf = dynamic_cast<dtkComposerSceneNodeLeaf *>(n)) {

        for(int i = 0; i < ports.count(); i++) {
            if (ports.at(i).toElement().hasAttribute("label")) {
                if(ports.at(i).toElement().attribute("type") == "input")
                    leaf->inputPorts().at(ports.at(i).toElement().attribute("id").toUInt())->setLabel(ports.at(i).toElement().attribute("label"));
                else
                    leaf->outputPorts().at(ports.at(i).toElement().attribute("id").toUInt())->setLabel(ports.at(i).toElement().attribute("label"));
            }
        }

        if(dtkComposerNodeBoolean *boolean = dynamic_cast<dtkComposerNodeBoolean *>(leaf->wrapee())) {

            for(int i = 0; i < childNodes.count(); i++) {
                if(childNodes.at(i).toElement().tagName() == "value") {
                    boolean->setValue(childNodes.at(i).childNodes().at(0).toText().data() == "true");
                }
            }
        }

        if(dtkComposerNodeInteger *integer = dynamic_cast<dtkComposerNodeInteger *>(leaf->wrapee())) {

            for(int i = 0; i < childNodes.count(); i++) {
                if(childNodes.at(i).toElement().tagName() == "value") {
                    integer->setValue(childNodes.at(i).childNodes().at(0).toText().data().toLongLong());
                }
            }
        }

        if(dtkComposerNodeReal *real = dynamic_cast<dtkComposerNodeReal *>(leaf->wrapee())) {

            for(int i = 0; i < childNodes.count(); i++) {
                if(childNodes.at(i).toElement().tagName() == "value") {
                    real->setValue(childNodes.at(i).childNodes().at(0).toText().data().toDouble());
                }
            }
        }

        if(dtkComposerNodeString *s = dynamic_cast<dtkComposerNodeString *>(leaf->wrapee())) {

            for(int i = 0; i < childNodes.count(); i++) {
                if(childNodes.at(i).toElement().tagName() == "value") {
                    s->setValue(childNodes.at(i).childNodes().at(0).toText().data());
                }
            }
        }
    }

    d->node = t;

    // --

    return n;
}

dtkComposerSceneEdge *dtkComposerReader::readEdge(QDomNode node)
{
    QDomElement source = node.firstChildElement("source");
    QDomElement destin = node.firstChildElement("destination");
    
    int source_node = source.attribute("node").toInt();
    int destin_node = destin.attribute("node").toInt();
    
    int source_id = source.attribute("id").toInt();
    int destin_id = destin.attribute("id").toInt();

    QString source_type = source.attribute("type");
    QString destin_type = destin.attribute("type");
    
    dtkComposerSceneEdge *edge = new dtkComposerSceneEdge;
    if(source_type == "input")
        edge->setSource(d->node_map.value(source_node)->inputPorts().at(source_id));
    else
        edge->setSource(d->node_map.value(source_node)->outputPorts().at(source_id));
    if(destin_type == "input")
        edge->setDestination(d->node_map.value(destin_node)->inputPorts().at(destin_id));
    else
        edge->setDestination(d->node_map.value(destin_node)->outputPorts().at(destin_id));
    edge->link();
    edge->adjust();
    
    d->node->addEdge(edge);

    edge->setParent(d->node);

    dtkComposerTransmitterConnection(d->scene->root(), d->node, edge);

    d->graph->addEdge(edge);

    return edge;
}
