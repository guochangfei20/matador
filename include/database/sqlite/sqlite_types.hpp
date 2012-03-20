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

#ifndef SQLITE_TYPES_HPP
#define SQLITE_TYPES_HPP

#include "database/sqlite/sqlite_statement.hpp"

namespace oos {

class varchar_base;
class object_base_ptr;

namespace sqlite {

template <class T> 
struct type_traits; 

template <> struct type_traits<char> { inline static const char* type_string() { return "INTEGER"; } }; 
template <> struct type_traits<int> { inline static const char* type_string() { return "INTEGER"; } };
template <> struct type_traits<long> { inline static const char* type_string() { return "INTEGER"; } };
template <> struct type_traits<unsigned> { inline static const char* type_string() { return "INTEGER"; } };
template <> struct type_traits<bool> { inline static const char* type_string() { return "INTEGER"; } };
template <> struct type_traits<float> { inline static const char* type_string() { return "REAL"; } };
template <> struct type_traits<double> { inline static const char* type_string() { return "REAL"; } };
template <> struct type_traits<varchar_base> { inline static const char* type_string() { return "VARCHAR"; } };
template <> struct type_traits<const char*> { inline static const char* type_string() { return "TEXT"; } };
template <> struct type_traits<std::string> { inline static const char* type_string() { return "TEXT"; } };
template <> struct type_traits<object_base_ptr> { inline static const char* type_string() { return "INTEGER"; } };

class sqlite_types
{
public:
  static const char *create_postfix() { return "CREATE TABLE"; }
  static const char *select_postfix() { return "SELECT"; }
  static const char *insert_postfix() { return "INSERT INTO"; }
  static const char *update_postfix() { return "UPDATE"; }
  static const char *delete_postfix() { return "DELETE FROM"; }
  static const char *drop_postfix() { return "DROP TABLE"; }

  static const char *primary_key_prefix() { return "PRIMARY KEY NOT NULL"; }

  template < class T >
  const char* type_string(const T&) const
  {
    return type_traits<T>::type_string();
  }
};

class sqlite_create_type
{
public:
  static const char *name() { return "CREATE TABLE"; }
};

class sqlite_select_type
{
public:
  static const char *name() { return "SELECT"; }
};

class sqlite_update_type
{
public:
  static const char *name() { return "UPDATE"; }
};

}

}

#endif /* SQLITE_TYPES_HPP */
