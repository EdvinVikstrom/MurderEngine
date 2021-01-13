#ifndef LIBME_REFERENCE_HPP
  #define LIBME_REFERENCE_HPP

namespace me {

  class reference {

  protected:

    mutable size_t* reference_count;
    const bool is_copy;

    reference(bool is_copy)
      : is_copy(is_copy)
    {
    }

  public:

    static size_t* make_counter();
    static void free_counter(size_t* counter);

  };

}

#endif
