/* dtkComposerView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Sep  7 15:06:52 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Dec 10 22:31:25 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 26
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKCOMPOSERVIEW_H
#define DTKCOMPOSERVIEW_H

#include "dtkComposerExport.h"

#include <QtCore>
#include <QtGui>

class DTKCOMPOSER_EXPORT dtkComposerView : public QGraphicsView
{
    Q_OBJECT

public:
     dtkComposerView(QWidget *parent = 0);
    ~dtkComposerView(void);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif
