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

    TableAnyMap(TableAnyMap *p,int sz,Data *dt)
        : tsize(sz),data(dt)
        , n(0),count(0)
        , parent(p),left(NULL),right(NULL) 
    {}

    virtual ~TableAnyMap();

    int size() const { return count; }

    void insert(size_t k,void *t)
    {
//        FLEXT_ASSERT(t);
        if(n) 
            _set(k,t);
        else {
            data[n++](k,t);
            ++count;
        }
    }

    void *find(size_t k) const { return n?_find(k):NULL; }

    void erase(size_t k) { if(n) { void *s = _remove(k); if(s) Free(s); } }

    void *remove(size_t k) { return n?_remove(k):NULL; }

    void clear();

    class FLEXT_SHARE iterator
    {
    public:
        iterator(): map(NULL) {}
        iterator(const TableAnyMap &m): map(&m),ix(0) { leftmost(); }
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
            const TableAnyMap *nmap;
            while((nmap = map->left) != NULL) map = nmap;
        }

        void forward();

        const TableAnyMap *map;
        int ix;
    };

private:
    void _init(size_t k,void *t) { data[0](k,t); n = count = 1; }

    bool _toleft(size_t k,void *t)
    {
        if(left) {
            bool a = left->_set(k,t);
            if(a) ++count;
            return a;
        }
        else {
            (left = New(this))->_init(k,t);
            ++count;
            return true;
        }
    }

    bool _toright(size_t k,void *t)
    {
        if(right) {
            bool a = right->_set(k,t);
            if(a) ++count;
            return a;
        }
        else {
            (right = New(this))->_init(k,t);
            ++count;
            return true;
        }
    }

    bool _toleft(Data &v) { return _toleft(v.key,v.value); }
    bool _toright(Data &v) { return _toright(v.key,v.value); }

    bool _set(size_t k,void *t);
    void *_find(size_t k) const;
    void *_remove(size_t k);

    const int tsize;
    Data *const data;
    int count,n;
    TableAnyMap *parent,*left,*right;

    int _tryix(size_t k) const
    {
        //! return index of data item with key <= k

//        FLEXT_ASSERT(n);
        int ix = 0;
        {
            int b = n;
            while(ix != b) {
                const int c = (ix+b)/2;
                const size_t dk = data[c].key;
                if(k == dk)
                    return c;
                else if(k < dk)
                    b = c;
                else if(ix < c)
                    ix = c;
                else {
                    ix = b;
                    break;
                }
            }
        }
        return ix;
    }

    static void _eraseempty(TableAnyMap *&b)
    {
//        FLEXT_ASSERT(b);
        if(!b->n) { 
            // remove empty branch
            delete b; b = NULL; 
        }
    }

    void _getsmall(Data &dt);
    void _getbig(Data &dt);
};

template <typename K,typename T,int N = 8>
class TablePtrMap
    : TableAnyMap
{
public:
    TablePtrMap(): TableAnyMap(NULL,N,slots) {}
    virtual ~TablePtrMap() { clear(); }

    inline void clear() { TableAnyMap::clear(); }

    inline int size() const { return TableAnyMap::size(); }

    inline void insert(K k,T *t) { TableAnyMap::insert(*(size_t *)&k,t); }

    inline T *find(K k) const { return (T *)TableAnyMap::find(*(size_t *)&k); }

    inline void erase(K k) { TableAnyMap::erase(*(size_t *)&k); }
    inline T *remove(K k) { return (T *)TableAnyMap::remove(*(size_t *)&k); }

    class iterator
        : TableAnyMap::iterator
    {
    public:
        iterator() {}
        iterator(const TablePtrMap &m): TableAnyMap::iterator(m) {}
        iterator(iterator &it): TableAnyMap::iterator(it) {}

        inline iterator &operator =(const iterator &it) { TableAnyMap::operator =(it); return *this; }

        inline operator bool() const {return TableAnyMap::iterator::operator bool(); }
        inline T *data() const { return (T *)TableAnyMap::iterator::data(); }
        inline K key() const { return (K)TableAnyMap::iterator::key(); }

        inline iterator &operator ++() { TableAnyMap::iterator::operator ++(); return *this; }  

    };

protected:
    TablePtrMap(TableAnyMap *p): TableAnyMap(p,N,slots) {}

    virtual TableAnyMap *New(TableAnyMap *parent) { return new TablePtrMap(parent); }

    virtual void Free(void *ptr) {}

    Data slots[N];
};

template <typename K,typename T,int N = 8>
class TablePtrMapOwned
    : public TablePtrMap<K,T,N>
{
public:
    virtual ~TablePtrMapOwned() { TablePtrMapOwned<K,T,N>::clear(); }

protected:
    virtual void Free(void *ptr) 
    { 
//            FLEXT_ASSERT(ptr);
        delete (T *)ptr;
    }

};

//! @} // FLEXT_SUPPORT

#endif
