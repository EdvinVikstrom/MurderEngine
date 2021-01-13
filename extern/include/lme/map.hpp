#ifndef LIBME_MAP_HPP
  #define LIBME_MAP_HPP

#include "pair.hpp"
#include "vector.hpp"

namespace me {

  template<typename K, typename V>
  class map { /* TODO: fix key */

  protected:

    typedef const K Key;
    typedef V Value;

    typedef K PK;
    typedef V PV;

    vector<pair<PK, PV>> entries;

  public:

    map()
    {
    }

    [[nodiscard]] pair<PK, PV>* data() const
    {
      return entries.data();
    }

    [[nodiscard]] pair<PK, PV>* begin() const
    {
      return entries.begin();
    }

    [[nodiscard]] pair<PK, PV>* end() const
    {
      return entries.end();
    }


    pair<PK, PV>& put(Key &key, Value &&value)
    {
      return entries.emplace_back(key, value);
    }

    pair<PK, PV>& put(Key &key, const Value &value)
    {
      return entries.emplace_back(key, value);
    }


    void erase(Key &key)
    {
      for (size_t i = 0; i < entries.size(); i++)
      {
	if (entries.at(i).first == key)
	{
	  entries.erase(i);
	  break;
	}
      }
    }


    [[nodiscard]] Value& at(Key &key)
    {
      for (const auto &entry : entries)
      {
	if (entry->first == key)
	  return entry->second;
      }
      return nullptr;
    }


    size_t capacity() const
    {
      return entries.capacity();
    }

    size_t size() const
    {
      return entries.size();
    }

    [[nodiscard]] bool is_empty() const
    {
      return entries.is_empty();
    }


    Value& operator[](Key &key)
    {
      for (auto &entry : entries)
      {
	if (entry.first == key)
	  return entry.second;
      }
      return put(key, Value()).second;
    }

  };

}

#endif
