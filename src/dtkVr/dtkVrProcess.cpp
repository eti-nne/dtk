/* dtkVrProcess.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 10 21:11:31 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu May  5 09:16:22 2011 (+0200)
 *           By: Julien Wintz
 *     Update #: 174
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkVrProcess.h"
#include "dtkVrSlave.h"

#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkVec3.h>
#include <dtkCore/dtkQuat.h>

#include <dtkDistributed/dtkDistributedCommunicator.h>

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// dtkVrProcess
// /////////////////////////////////////////////////////////////////

class dtkVrProcessPrivate
{
public:
    char running;

    dtkDistributedCommunicator *communicator;
};

dtkVrProcess::dtkVrProcess(dtkDistributedCommunicator *communicator) : d(new dtkVrProcessPrivate)
{
    d->communicator = communicator;
}

dtkVrProcess::~dtkVrProcess(void)
{
    delete d;

    d = NULL;
}

void dtkVrProcess::show(bool fullscreen)
{
    if (!this->rank())
        return;

    if (!fullscreen) {
        dynamic_cast<dtkVrSlave *>(this)->show();
        dynamic_cast<dtkVrSlave *>(this)->resize(500, 320);
        dynamic_cast<dtkVrSlave *>(this)->move(
            this->rank()-1 == 3 ? 500 : (this->rank()-1)*500,
            this->rank()-1 == 3 ? 350 : 0);
    } else {
        dynamic_cast<dtkVrSlave *>(this)->showFullScreen();
    }
}

void dtkVrProcess::start(void)
{
    d->running = 0;

    this->initialize();
    this->synchronize();
    this->broadcast();

    while(this->running()) {
        this->broadcast();
        this->process();
        qApp->processEvents(QEventLoop::AllEvents, 10);
        this->synchronize();
    }

    this->uninitialize();
}

void dtkVrProcess::stop(void)
{
    d->running++;
}

int dtkVrProcess::rank(void) const
{
    return d->communicator->rank();
}

int dtkVrProcess::size(void) const
{
    return d->communicator->size();
}

bool dtkVrProcess::running(void)
{
    return d->running ? (++(d->running) < 3) : true;
}

void dtkVrProcess::broadcast(void)
{
    char status[1]; status[0] = d->running;

    // d->communicator->broadcast(&status, 1, dtkDistributedCommunicator::dtkDistributedCommunicatorChar, 0);

    // d->communicator->broadcast(d->user->position().values(), 3, dtkDistributedCommunicator::dtkDistributedCommunicatorDouble, 0);
    // d->communicator->broadcast(d->user->orientation().values(), 4, dtkDistributedCommunicator::dtkDistributedCommunicatorDouble, 0);

    // d->communicator->broadcast(&(d->wand->action()), 1, dtkDistributedCommunicator::dtkDistributedCommunicatorInt, 0);

    // d->communicator->broadcast(&(d->wand->mode()), 1, dtkDistributedCommunicator::dtkDistributedCommunicatorInt, 0);

    // d->communicator->broadcast(d->wand->referencePosition().values(), 3, dtkDistributedCommunicator::dtkDistributedCommunicatorDouble, 0);
    // d->communicator->broadcast(d->wand->referenceOrientation().values(), 4, dtkDistributedCommunicator::dtkDistributedCommunicatorDouble, 0);

    // d->communicator->broadcast(d->wand->currentPosition().values(), 3, dtkDistributedCommunicator::dtkDistributedCommunicatorDouble, 0);
    // d->communicator->broadcast(d->wand->currentOrientation().values(), 4, dtkDistributedCommunicator::dtkDistributedCommunicatorDouble, 0);

    d->running = status[0];
}

void dtkVrProcess::synchronize(void)
{
    d->communicator->barrier();
}

// /////////////////////////////////////////////////////////////////
// dtkVrProcess Documentation
// /////////////////////////////////////////////////////////////////

/*! \class dtkVrProcess
 *
 *  \brief The dtkAbstractObject is the base class virtual reality
 *  entities in a multiprocess envrionment.
 *
 *  The process is not meant to be instantiated by its own, the
 *  possibility remains though. Instead, it is meant to be created
 *  through the dtkVrManager that also equips it with a specialization
 *  of a dtkAbstractView.
 *
 *  \code
 *  dtkPluginManager::instance()->initialize();
 *
 *  dtkVrManager::instance()->initialize();
 *
 *  dtkVrProcess *process = dtkVrManager::instance()->create("vtkViewVr");
 *  process->show();
 *  process->start();
 *
 *  dtkVrManager::instance()->uninitialize();
 *
 *  dtkPluginManager::instance()->uninitialize();
 *  \encode
 */
