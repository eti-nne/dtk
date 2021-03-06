/* dtkDistributedMapper.cpp --- 
 * 
 * Author: Thibaud Kloczko
 * Created: 2013 Thu Feb  7 10:55:57 (+0100)
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "dtkDistributedMapper.h"

#include <QtCore>

// /////////////////////////////////////////////////////////////////
// dtkDistributedMapperPrivate interface
// /////////////////////////////////////////////////////////////////

class dtkDistributedMapperPrivate
{
public:
     dtkDistributedMapperPrivate(void) : ref(0) {;}
    ~dtkDistributedMapperPrivate(void) {;}

public:
    void setMapping(const qlonglong& id_number, const qlonglong& pu_number);
    void initMap(const qlonglong& map_size, const qlonglong& pu_size);
    void setMap(const qlonglong& local_map_size, const qlonglong& pu_id);

public:
    qlonglong localToGlobal(const qlonglong&  local_id, const qlonglong& pu_id) const;

    qlonglong globalToLocal(const qlonglong& global_id) const;
    qlonglong globalToLocal(const qlonglong& global_id, const qlonglong& owner) const;

    qlonglong count(const qlonglong& pu_id) const;
    qlonglong countMax(void) const;

    qlonglong owner(const qlonglong& global_id, qlonglong pu_id = -1) const;

    QVector<qlonglong> readers(const qlonglong& global_id) const;

public:
    QAtomicInt ref;

    qlonglong id_count;
    qlonglong pu_count;

    QVector<qlonglong> map;
};

// /////////////////////////////////////////////////////////////////
// dtkDistributedMapperPrivate implementation
// /////////////////////////////////////////////////////////////////

void dtkDistributedMapperPrivate::setMapping(const qlonglong& id_number, const qlonglong& pu_number)
{
    this->id_count = id_number;
    this->pu_count = pu_number;
    this->map.reserve(this->pu_count + 1);
    this->map.clear();

    if (this->pu_count == 1) {
        this->map << 0;

    } else if (this->id_count < this->pu_count  ) {

        qDebug() << "Number of ids less than process count: NOT YET IMPLEMENTED";
        return;

    } else {
        qlonglong step = this->id_count / this->pu_count + 1;

        qlonglong rest = this->id_count % this->pu_count;
        for (qlonglong i = 0; i < rest+1; ++i) {
            this->map << i * step;
        }
        qlonglong last = rest * step;
        for (qlonglong i = 1; i < this->pu_count-rest; ++i) {
            this->map << last + i * (step -1);
        }
    }

    this->map << this->id_count;
}

void dtkDistributedMapperPrivate::initMap(const qlonglong& map_size, const qlonglong& pu_size)
{
    this->id_count = map_size;
    this->pu_count = pu_size;

    this->map.resize(this->pu_count + 1);
    this->map[this->pu_count] = map_size;
}

void dtkDistributedMapperPrivate::setMap(const qlonglong& offset, const qlonglong& pu_id)
{
    this->map[pu_id] = offset;
}

qlonglong dtkDistributedMapperPrivate::localToGlobal(const qlonglong& local_id, const qlonglong& pu_id) const
{
    return ( local_id + this->map.at(pu_id) );
}

qlonglong dtkDistributedMapperPrivate::globalToLocal(const qlonglong& global_id) const
{
    qlonglong pu_id = this->owner(global_id);

    return ( global_id - this->map.at(pu_id) );
}

qlonglong dtkDistributedMapperPrivate::globalToLocal(const qlonglong& global_id, const qlonglong& owner) const
{
    return ( global_id - this->map.at(owner) );
}

qlonglong dtkDistributedMapperPrivate::count(const qlonglong& pu_id) const
{
    return ( this->map.at(pu_id + 1) - this->map.at(pu_id) );
}

qlonglong dtkDistributedMapperPrivate::countMax(void) const
{
    qlonglong count_max = this->map.at(1) - this->map.at(0);
    for(int i = 1; i < this->map.count() - 1; ++i) {
        count_max = qMax(count_max, this->map.at(i + 1) - this->map.at(i));
    }
    return count_max;
}

qlonglong dtkDistributedMapperPrivate::owner(const qlonglong& global_id, qlonglong pu_id) const
{
    // qlonglong current_id = this->map[this->map.count() - 2];
    // qlonglong pid = this->pu_count-1;
    // while (global_id < current_id) {
    //     current_id = this->map.at(--pid);
    // }
    // return pid;

    if (pu_id < 0) {
        pu_id =  qFloor((global_id * this->pu_count) / this->map.last());
    }

    if (global_id >= this->map.at(pu_id + 1)) {
        ++pu_id;
        while(global_id >= this->map.at(pu_id + 1))
            ++pu_id;
    }
    else if (global_id  < this->map.at(pu_id)) {
        --pu_id;
        while(global_id < this->map.at(pu_id))
            --pu_id;

    }

    return pu_id;
}

// /////////////////////////////////////////////////////////////////
// dtkDistributedMapper implementation
// /////////////////////////////////////////////////////////////////

dtkDistributedMapper::dtkDistributedMapper(void) : QObject(), d(new dtkDistributedMapperPrivate)
{

}

dtkDistributedMapper::~dtkDistributedMapper(void)
{
    delete d;
}

bool dtkDistributedMapper::deref(void)
{
    return d->ref.deref();
}

bool dtkDistributedMapper::ref(void)
{
    return d->ref.ref();
}

void dtkDistributedMapper::setMapping(const qlonglong& id_count, const qlonglong& pu_count)
{
    d->setMapping(id_count, pu_count);
}

void dtkDistributedMapper::initMap(const qlonglong& map_size, const qlonglong& pu_size)
{
    d->initMap(map_size, pu_size);
}

void dtkDistributedMapper::setMap(const qlonglong& offset, const qlonglong& pu_id)
{
    d->setMap(offset, pu_id);
}

qlonglong dtkDistributedMapper::localToGlobal(const qlonglong& local_id, const qlonglong& pu_id) const
{
    return d->localToGlobal(local_id, pu_id);
}

qlonglong dtkDistributedMapper::globalToLocal(const qlonglong& global_id) const
{
    return d->globalToLocal(global_id);
}

qlonglong dtkDistributedMapper::globalToLocal(const qlonglong& global_id, const qlonglong& owner ) const
{
    return d->globalToLocal(global_id, owner);
}

qlonglong dtkDistributedMapper::count(const qlonglong& pu_id) const
{
    return d->count(pu_id);
}

qlonglong dtkDistributedMapper::countMax(void) const
{
    return d->countMax();
}

qlonglong dtkDistributedMapper::firstIndex(const qlonglong& pu_id) const
{
    return d->map[pu_id];
}

qlonglong dtkDistributedMapper::lastIndex(const qlonglong& pu_id) const
{
    return d->map[pu_id + 1] - 1;    
}

qlonglong dtkDistributedMapper::owner(const qlonglong& global_id, qlonglong pu_id) const
{
    return d->owner(global_id, pu_id);
}

