#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

#ifdef _MSC_VER
  #ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_SQL_API
#endif

#include "matador/utils/access.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/cascade_type.hpp"

#include <memory>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API result_impl : public matador::serializer
{
private:
  result_impl(const result_impl &) = delete;
  result_impl &operator=(const result_impl &) = delete;

public:
  typedef unsigned long size_type;

protected:
  result_impl();

  virtual bool needs_bind() { return false; };
  virtual bool finalize_bind() { return false; }
  virtual bool prepare_fetch() = 0;
  virtual bool finalize_fetch() = 0;

public:
  virtual ~result_impl();

  template < class T >
  void get(T *o)
  {
    matador::access::serialize(*this, *o);
  }

  virtual void serialize(const char*, char&) = 0;
  virtual void serialize(const char*, short&) = 0;
  virtual void serialize(const char*, int&) = 0;
  virtual void serialize(const char*, long&) = 0;
  virtual void serialize(const char*, unsigned char&) = 0;
  virtual void serialize(const char*, unsigned short&) = 0;
  virtual void serialize(const char*, unsigned int&) = 0;
  virtual void serialize(const char*, unsigned long&) = 0;
  virtual void serialize(const char*, bool&) = 0;
  virtual void serialize(const char*, float&) = 0;
  virtual void serialize(const char*, double&) = 0;
  virtual void serialize(const char*, char *, size_t) = 0;
  virtual void serialize(const char*, std::string&) = 0;
  virtual void serialize(const char*, matador::varchar_base&) = 0;
  virtual void serialize(const char*, matador::time&) = 0;
  virtual void serialize(const char*, matador::date&) = 0;
  virtual void serialize(const char*, matador::basic_identifier &x) = 0;
  virtual void serialize(const char*, matador::identifiable_holder &x, cascade_type) = 0;

  virtual const char *column(size_type c) const = 0;

  virtual bool fetch() = 0;

  /**
   * Fetch next line from sql and
   * deserialized the given serializable.
   *
   * @param o Object to be deserialized
   * @return True if serializable was successfully deserialized
   */
  template < class T >
  void bind(T *o)
  {
    if (needs_bind()) {
      serializer::serialize(*o);
      finalize_bind();
    }
  }

  template < class T >
  bool fetch(T *o)
  {
    if (!prepare_fetch()) {
      return false;
    }
    result_index_ = transform_index(0);
    serializer::serialize(*o);
    return finalize_fetch();
  }

  virtual size_type affected_rows() const = 0;

  virtual size_type result_rows() const = 0;

  virtual size_type fields() const = 0;

  virtual int transform_index(int index) const = 0;

protected:
  void read_foreign_object(const char *id, identifiable_holder &x);

protected:
  int result_index_ = 0;
};

/// @endcond

}

}

#endif /* RESULT_IMPL_HPP */
