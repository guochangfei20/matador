/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MYSQL_STATEMENT_HPP
#define MYSQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/utils/varchar.hpp"

#ifdef _MSC_VER
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <string>
#include <vector>
#include <type_traits>

namespace matador {

class varchar_base;
class time;
class date;

namespace mysql {

class mysql_connection;

class mysql_statement : public matador::detail::statement_impl
{
public:
  mysql_statement(mysql_connection &db, const matador::sql &stmt);
  virtual ~mysql_statement();

  virtual void clear();
  virtual detail::result_impl* execute();
  virtual void reset();
  
protected:
  virtual void serialize(const char *id, char &x);
  virtual void serialize(const char *id, short &x);
  virtual void serialize(const char *id, int &x);
  virtual void serialize(const char *id, long &x);
  virtual void serialize(const char *id, unsigned char &x);
  virtual void serialize(const char *id, unsigned short &x);
  virtual void serialize(const char *id, unsigned int &x);
  virtual void serialize(const char *id, unsigned long &x);
  virtual void serialize(const char *id, float &x);
  virtual void serialize(const char *id, double &x);
  virtual void serialize(const char *id, bool &x);
  virtual void serialize(const char *id, char *x, size_t s);
  virtual void serialize(const char *id, varchar_base &x);
  virtual void serialize(const char *id, std::string &x);
  virtual void serialize(const char *id, matador::date &x);
  virtual void serialize(const char *id, matador::time &x);
  virtual void serialize(const char *id, basic_identifier &x);
  virtual void serialize(const char *id, identifiable_holder&x, cascade_type);

private:
  template < class T >
  void bind_value(std::size_t index, enum_field_types type, T value)
  {
    MYSQL_BIND &bind = host_array[index];
    if (bind.buffer == nullptr) {
      // allocating memory
      bind.buffer = new char[sizeof(T)];
    }
    *static_cast<T*>(bind.buffer) = value;
    bind.buffer_type = type;
	  bind.buffer_length = sizeof(T);
    is_null_vector[index] = false;
    bind.is_null = &is_null_vector[index];
	  bind.is_unsigned = std::is_unsigned<T>::value;
  }
  void bind_value(std::size_t index, enum_field_types type, char x);
  void bind_value(std::size_t index, enum_field_types type, unsigned char x);
  void bind_value(std::size_t index, enum_field_types type, const matador::date &x);
  void bind_value(std::size_t index, enum_field_types type, const matador::time &x);
  void bind_value(std::size_t index, enum_field_types type, const char *value, size_t size);

  void bind_null(std::size_t index);

private:
  size_t result_size;
  size_t host_size;
  std::vector<my_bool> is_null_vector;
  MYSQL_STMT *stmt_ = nullptr;
  MYSQL_BIND *host_array = nullptr;
};

}

}

#endif /* MYSQL_STATEMENT_HPP */
