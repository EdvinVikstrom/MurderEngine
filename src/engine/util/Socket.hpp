/* temporaryry name */
/* must find a cooler name */

#ifndef ME_SOCKET_HPP
  #define ME_SOCKET_HPP

namespace me {

  template<typename T>
  struct Socket {

    virtual void* request(const T what) const = 0;

  };

}

#endif
