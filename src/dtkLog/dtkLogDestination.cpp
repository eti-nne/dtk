// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#include "dtkLogDestination.h"

// /////////////////////////////////////////////////////////////////
// dtkLogDestination
// /////////////////////////////////////////////////////////////////

dtkLogDestination::~dtkLogDestination(void)
{

}

// /////////////////////////////////////////////////////////////////
// dtkLogDestinationConsole
// /////////////////////////////////////////////////////////////////

void dtkLogDestinationConsole::write(const QString& message)
{
    fprintf(stderr, "%s\n", qPrintable(message));
    fflush(stderr);
}

// /////////////////////////////////////////////////////////////////
// dtkLogDestinationFile
// /////////////////////////////////////////////////////////////////

class dtkLogDestinationFilePrivate
{
public:
    QFile file;

public:
    QTextStream stream;
};

dtkLogDestinationFile::dtkLogDestinationFile(const QString& path) : d(new dtkLogDestinationFilePrivate)
{
    d->file.setFileName(path);

    QFileInfo info(path);

    QDir dir(info.absoluteDir());

    if(!dir.exists()) {
        QString name = dir.path();
        dir.mkpath(name);
    }

    if(!d->file.open(QFile::WriteOnly | QFile::Text | QIODevice::Append))
        qDebug() << "Unable to open" << path << "for writing";

    d->stream.setDevice(&(d->file));
}

dtkLogDestinationFile::~dtkLogDestinationFile(void)
{
    delete d;

    d = NULL;
}

void dtkLogDestinationFile::write(const QString& message)
{
    d->stream << message << endl;
    d->stream.flush();
}

//
// dtkLogDestination.cpp ends here