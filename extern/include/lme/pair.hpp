#ifndef LIBME_PAIR_HPP
  #define LIBME_PAIR_HPP

namespace me {

  template<class T1, class T2>
  struct pair {

    typedef T1 Type1;
    typedef T2 Type2;

    Type1 first;
    Type2 second;

    pair(const Type1 &first, const Type2 &second)
      : first(first), second(second)
    {
    }
    
    pair(Type1 &&first, Type2 &&second)
    {
      this->first = static_cast<Type1&&>(first);
      this->second = static_cast<Type2&&>(second);
    }

    bool operator==(const pair &p) const
    {
      return first = p.first && second == p.second;
    }

    bool operator!=(const pair &p) const
    {
      return !operator==(p);
    }

  };

}

#endif
