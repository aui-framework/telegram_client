// AUI Telegram Client - example of using AUI Framework
// Copyright (C) 2025 Alex2772 and Contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <list>
#include <map>

template <class Key, class Value>
class LRUCache {
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef std::list<key_type> list_type;
    typedef std::map<key_type, std::pair<value_type, typename list_type::iterator> > map_type;

    LRUCache(size_t capacity) : mCapacity(capacity) {}

    ~LRUCache() {}

    size_t size() const { return mMap.size(); }

    size_t capacity() const { return mCapacity; }

    bool empty() const { return mMap.empty(); }

    bool contains(const key_type &key) { return mMap.find(key) != mMap.end(); }

    void insert(const key_type &key, const value_type &value) {
        typename map_type::iterator i = mMap.find(key);
        if (i == mMap.end()) {
            // insert item into the cache, but first check if it is full
            if (size() >= mCapacity) {
                // cache is full, evict the least recently used item
                evict();
            }

            // insert the new item
            mList.push_front(key);
            mMap[key] = std::make_pair(value, mList.begin());
        }
    }

    AOptional<value_type> get(const key_type &key) {
        // lookup value in the cache
        typename map_type::iterator i = mMap.find(key);
        if (i == mMap.end()) {
            // value not in cache
            return std::nullopt;
        }

        // return the value, but first update its place in the most
        // recently used list
        typename list_type::iterator j = i->second.second;
        if (j != mList.begin()) {
            // move item to the front of the most recently used list
            mList.erase(j);
            mList.push_front(key);

            // update iterator in map
            j = mList.begin();
            const value_type &value = i->second.first;
            mMap[key] = std::make_pair(value, j);

            // return the value
            return value;
        } else {
            // the item is already at the front of the most recently
            // used list so just return it
            return i->second.first;
        }
    }

    void clear() {
        mMap.clear();
        mList.clear();
    }

private:
    void evict() {
        // evict item from the end of most recently used list
        typename list_type::iterator i = --mList.end();
        mMap.erase(*i);
        mList.erase(i);
    }

private:
    map_type mMap;
    list_type mList;
    size_t mCapacity;
};
