/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flmap.h
	\brief special map class for all 32-bit key/value-pairs   
*/

#ifndef __FLMAP_H
#define __FLMAP_H

#include "flprefix.h"

/*!	\defgroup FLEXT_SUPPORT Flext support classes
	@{
*/

#if 0

#include <map>

//! Key/Value type for AnyMap... must have size of pointer!
typedef size_t AnyMapType;

//! Base class for maps
class AnyMap:
    public std::map<AnyMapType,AnyMapType>
{
    typedef std::map<AnyMapType,AnyMapType> Parent;
public:
    AnyMap();
    ~AnyMap();
    iterator find(AnyMapType k);
    AnyMapType &operator [](AnyMapType k);

    typedef std::pair<AnyMapType,AnyMapType> pair;
};

//! Specialized map class for any 32-bit key/value types
template <class K,class T>
class DataMap:
    public AnyMap
{
public:
    class iterator:
        public AnyMap::iterator
    {
    public:
        iterator() {}
#if defined(_MSC_VER) && (_MSC_VER < 0x1300)
        // with the MSVC6 STL implementation iterators can't be initialized...
        iterator(AnyMap::iterator it) { static_cast<AnyMap::iterator &>(*this) = it; }
#else
        // note: &it doesn't work for gcc (i don't know why it doesn't...)
        iterator(AnyMap::iterator it): AnyMap::iterator(it) {}
#endif

        inline K &key() const { return *(K *)&((*this)->first); }
        inline T &data() const { return *(T *)&((*this)->second); }
    };

    class pair:
        public AnyMap::pair
    {
	public:
        inline K &key() const { return *(K *)&first; }
        inline T &data() const { return *(T *)&second; }
	};

    inline iterator find(K k) { return AnyMap::find(*(AnyMapType *)&k); }
    inline T &operator [](K k) { return *(T *)&(AnyMap::operator [](*(AnyMapType *)&k)); }
    inline void erase(K k) { AnyMap::erase(*(AnyMapType *)&k); }
};

#endif

class FLEXT_SHARE TableAnyMap
{
protected:
    virtual TableAnyMap *New(TableAnyMap *parent) = 0;
    virtual void Free(void *ptr) = 0;

    struct Data {
        void operator()(size_t k,void *v) { key = k,value = v; }
        void operator =(void *v) { value = v; }

        size_t key;
        void *value;
    };

    TableAnyMap(TableAnyMap *p,int mx,Data *dt,bool o)
        : owned(o),max(mx),data(dt)
        , n(0),parent(p),left(NULL),right(NULL) 
    {}

    virtual ~TableAnyMap() { clear(); }

    int size() const;

    void insert(size_t k,void *t)
    {
        FLEXT_ASSERT(t);
        if(n) _set(k,t);
        else data[n++](k,t);
    }

    void *find(size_t k) { return n?_find(k):NULL; }

    void clear();

    void _toleft(size_t k,void *t)
    {
        if(left)
            left->_set(k,t);
        else {
            left = New(this);
            left->data[0](k,t);
            left->n = 1;
        }
    }

    void _toright(size_t k,void *t)
    {
        if(right) 
            right->_set(k,t);
        else {
            right = New(this);
            right->data[0](k,t);
            right->n = 1;
        }
    }

    void _toleft(Data &v) { _toleft(v.key,v.value); }
    void _toright(Data &v) { _toright(v.key,v.value); }

    void _set(size_t k,void *t);
    void *_find(size_t k);

    Data *const data;
    const int max;
    const bool owned;
    int n;
    TableAnyMap *parent,*left,*right;


    class iterator
    {
    public:
        iterator(): map(NULL) {}
        iterator(TableAnyMap &m): map(&m),ix(0) { leftmost(); }
        iterator(iterator &it): map(it.map),ix(it.ix) {}
    
        iterator &operator =(const iterator &it) { map = it.map,ix = it.ix; return *this; }

        operator bool() const { return map && /*ix >= 0 &&*/ ix < map->n; }

        // no checking here!
        void *data() const { return map->data[ix].value; }
        size_t key() const { return map->data[ix].key; }

        iterator &operator ++() { forward(); return *this; }  

    protected:
        void leftmost()
        {
            // search smallest branch (go left as far as possible)
            TableAnyMap *nmap;
            while((nmap = map->left) != NULL) map = nmap;
        }

        void forward();

        TableAnyMap *map;
        int ix;
    };
};


template <typename K,typename T,int N = 8,bool O = false>
class FLEXT_SHARE TableMap
    : TableAnyMap
{
public:
    TableMap(): TableAnyMap(NULL,N,slots,O) {}
    virtual ~TableMap() { clear(); }

    inline void clear() { TableAnyMap::clear(); }

    inline int size() const { return TableAnyMap::size(); }

    inline void insert(K k,T *t) { TableAnyMap::insert(*(size_t *)&k,t); }

    inline T *find(K k) { return (T *)TableAnyMap::find(*(size_t *)&k); }

    class iterator
        : TableAnyMap::iterator
    {
    public:
        iterator() {}
        iterator(TableMap &m): TableAnyMap::iterator(m) {}
        iterator(iterator &it): TableAnyMap::iterator(it) {}

        inline iterator &operator =(const iterator &it) { TableAnyMap::operator =(it); return *this; }

        inline operator bool() const {return TableAnyMap::iterator::operator bool(); }
        inline T *data() const { return (T *)TableAnyMap::iterator::data(); }
        inline K key() const { return (K)TableAnyMap::iterator::key(); }

        inline iterator &operator ++() { TableAnyMap::iterator::operator ++(); return *this; }  

    };

protected:
    TableMap(TableAnyMap *p): TableAnyMap(p,N,slots,O) {}

    virtual TableAnyMap *New(TableAnyMap *parent) { return new TableMap(parent); }
    virtual void Free(void *ptr) { delete (T *)ptr; }

    Data slots[N];
};


//! @} // FLEXT_SUPPORT

#endif
