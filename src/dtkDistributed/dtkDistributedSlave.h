/* dtkDistributedSlave.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed May 25 14:13:03 2011 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Sep 14 13:59:22 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 26
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef DTKDISTRIBUTEDSLAVE_H
#define DTKDISTRIBUTEDSLAVE_H

#include "dtkDistributedExport.h"

#include <QtCore>
#include <QtNetwork>

class dtkDistributedSlavePrivate;

class DTKDISTRIBUTED_EXPORT dtkDistributedSlave : public QObject
{
    Q_OBJECT

public:
     dtkDistributedSlave(void);
    ~dtkDistributedSlave(void);

public:
    virtual int exec(void);

public:
    bool    isConnected(void);
    bool isDisconnected(void);

signals:
    void    connected(const QUrl& server);
    void disconnected(const QUrl& server);

    void started(void);
    void   ended(void);

public slots:
    void    connect(const QUrl& server);
    void disconnect(const QUrl& server);

protected slots:
    void onStarted(void);
    void   onEnded(void);

protected slots:
    void read(void);
    void error(QAbstractSocket::SocketError error);

private:
    dtkDistributedSlavePrivate *d;
};

#endif
