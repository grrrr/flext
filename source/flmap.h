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

#include <map>

/*!	\defgroup FLEXT_SUPPORT Flext support classes
	@{
*/

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

//! @} // FLEXT_SUPPORT

#endif
