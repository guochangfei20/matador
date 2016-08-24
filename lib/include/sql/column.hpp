//
// Created by sascha on 3/2/16.
//

#ifndef OOS_COLUMN_HPP
#define OOS_COLUMN_HPP

#include "sql/token.hpp"
#include "sql/value.hpp"
#include "sql/basic_dialect.hpp"

namespace oos {

struct column : public detail::token
{
  column(const std::string &col);

  virtual void accept(token_visitor &visitor) override;

  std::string name;
};

struct columns : public detail::token
{
  enum t_brackets {
    WITH_BRACKETS,
    WITHOUT_BRACKETS
  };

  columns(std::initializer_list<column> cols, t_brackets with_brackets = WITH_BRACKETS);
  explicit columns(t_brackets with_brackets = WITH_BRACKETS);

  void push_back(const std::shared_ptr<column> &col);

  columns & with_brackets();
  columns & without_brackets();

  static columns all();

  virtual void accept(token_visitor &visitor);

  std::vector<std::shared_ptr<column>> columns_;
  t_brackets with_brackets_ = WITH_BRACKETS;

  static columns all_;
};

namespace detail {

struct typed_column : public oos::column
{
  typed_column(const std::string &col, data_type_t t, std::size_t idx, bool host);

  virtual void accept(token_visitor &visitor) override;

  data_type_t type;
  std::size_t index;
  bool is_host;
};

struct identifier_column : public typed_column
{
  identifier_column(const char *n, data_type_t t, size_t idx, bool host) : typed_column(n, t, idx, host) { }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }
};

struct typed_varchar_column : public typed_column
{
  typed_varchar_column(const char *n, size_t size, data_type_t t, size_t idx, bool host)
    : typed_column(n, t, idx, host)
    , size(size)
  { }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }

  size_t size;
};

struct identifier_varchar_column : public typed_varchar_column
{
  identifier_varchar_column(const char *n, size_t size, data_type_t t, size_t idx, bool host)
    : typed_varchar_column(n, size, t, idx, host)
  { }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }
};

struct basic_value_column : public column
{
  basic_value_column(const std::string &col, basic_value *val)
    : column(col)
    , value_(val)
  { }

  basic_value_column(const char *col, basic_value *val)
    : column(col)
    , value_(val)
  { }

  virtual void accept(token_visitor &visitor) override
  {
    visitor.visit(*this);
//    return visitor.visit(*this) + "=" + value_.compile(d);
  }

  std::unique_ptr<basic_value> value_;
};

template < class T >
struct value_column : public basic_value_column
{

  value_column(const std::string &col, T& val)
    : basic_value_column(col, new value<T>(val))
  { }

  value_column(const char *col, T& val)
  : basic_value_column(col, new value<T>(val))
  { }
};

template <>
struct value_column<char*> : public basic_value_column
{
  value_column(const std::string &col, char*& val, size_t s)
    : basic_value_column(col, new value<char*>(val, s))
  { }

  value_column(const char *col, char*& val, size_t s)
  : basic_value_column(col, new value<char*>(val, s))
  { }
};

}

}
#endif //OOS_COLUMN_HPP