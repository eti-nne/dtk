/* @(#)dtkComposerControlsListItemBoolean.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2013 - Nicolas Niclausse, Inria.
 * Created: 2013/06/03 12:33:16
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef DTKCOMPOSERCONTROLSLISTITEMBOOLEAN_H
#define DTKCOMPOSERCONTROLSLISTITEMBOOLEAN_H

#include "dtkComposerControlsListItem.h"

class dtkComposerSceneNode;
class dtkComposerControlsListItemBooleanPrivate;

class dtkComposerControlsListItemBoolean: public dtkComposerControlsListItem
{
    Q_OBJECT

public:
    dtkComposerControlsListItemBoolean(QListWidget *parent = 0, dtkComposerSceneNode *node = NULL);
    virtual ~dtkComposerControlsListItemBoolean(void);

public:
    QWidget *widget(void);

public slots:
    void onValueChanged(bool value);

private:
    dtkComposerControlsListItemBooleanPrivate *d;
};

#endif
