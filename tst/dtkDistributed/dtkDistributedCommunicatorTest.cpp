/* dtkDistributedCommunicatorTest.cpp --- 
 * 
 * Author: Julien Wintz
 * Created: Mon Feb  4 14:11:27 2013 (+0100)
 * Version: 
 * Last-Updated: Tue Feb 26 14:35:33 2013 (+0100)
 *           By: Julien Wintz
 *     Update #: 107
 */

/* Change Log:
 * 
 */

#include "dtkDistributedCommunicatorTest.h"

#include <dtkDistributed>

void dtkDistributedCommunicatorTestCase::initTestCase(void)
{
    dtkDistributed::communicator::pluginManager().initialize();
}

void dtkDistributedCommunicatorTestCase::init(void)
{

}

void dtkDistributedCommunicatorTestCase::testPluginManager(void)
{
    qDebug() << dtkDistributed::communicator::pluginManager().plugins();
}

void dtkDistributedCommunicatorTestCase::testPluginFactory(void)
{
    qDebug() << dtkDistributed::communicator::pluginFactory().keys();
}

void dtkDistributedCommunicatorTestCase::testPlugin(void)
{
    dtkDistributedCommunicator *communicator = dtkDistributed::communicator::pluginFactory().create("mpi");
    communicator->initialize();
    qDebug() << communicator->metaObject()->className();
    qDebug() << communicator->pid();
    qDebug() << communicator->size();
    communicator->uninitialize();
    delete communicator;
}

void dtkDistributedCommunicatorTestCase::cleanupTestCase(void)
{
    dtkDistributed::communicator::pluginManager().uninitialize();
}

void dtkDistributedCommunicatorTestCase::cleanup(void)
{

}

DTKTEST_MAIN_NOGUI(dtkDistributedCommunicatorTest, dtkDistributedCommunicatorTestCase)