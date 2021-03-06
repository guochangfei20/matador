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

#ifndef ITEM_HPP
#define ITEM_HPP

#include "matador/utils/base_class.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/has_many.hpp"

#include "matador/utils/time.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/identifier.hpp"

#include "matador/utils/varchar.hpp"
#include <ostream>
#include <utility>

class Item
{
public:
  Item() : Item("") { }

  explicit Item(const std::string &str) : Item(str, -65000) {}

  Item(std::string str, int i) : int_(i), string_(std::move(str))
  {
    memset(cstr_, 0, CSTR_LEN);
#ifdef _MSC_VER
    strcpy_s(cstr_, CSTR_LEN, "Hallo");
#else
    strcpy(cstr_, "Hallo");
#endif
    cstr_[5] = '\0';
  }

  ~Item() = default;

public:
  template < class SERIALIZER > void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("val_char", char_);
    serializer.serialize("val_float", float_);
    serializer.serialize("val_double", double_);
    serializer.serialize("val_short", short_);
    serializer.serialize("val_int", int_);
    serializer.serialize("val_long", long_);
    serializer.serialize("val_unsigned_short", unsigned_short_);
    serializer.serialize("val_unsigned_int", unsigned_int_);
    serializer.serialize("val_unsigned_long", unsigned_long_);
    serializer.serialize("val_bool", bool_);
    serializer.serialize("val_cstr", cstr_, (size_t)CSTR_LEN);
    serializer.serialize("val_string", string_);
    serializer.serialize("val_varchar", varchar_);
    serializer.serialize("val_date", date_);
    serializer.serialize("val_time", time_);
  }

  unsigned long id() const { return id_.value(); }
  void id(unsigned long i) { id_.value(i); }

  void set_char(char x) { char_ = x; }
  void set_float(float x) { float_ = x; }
  void set_double(double x) { double_ = x; }
  void set_short(short x) { short_ = x; }
  void set_int(int x) { int_ = x; }
  void set_long(long x) { long_ = x; }
  void set_unsigned_short(unsigned short x) { unsigned_short_ = x; }
  void set_unsigned_int(unsigned int x) { unsigned_int_ = x; }
  void set_unsigned_long(unsigned long x) { unsigned_long_ = x; }
  void set_bool(bool x) { bool_ = x; }
  void set_cstr(const char *x, size_t size)
  {
    if (CSTR_LEN < size) {
      throw std::logic_error("not enough character size");
    }
//    mark_modified();
#ifdef _MSC_VER
    strcpy_s(cstr_, CSTR_LEN, x);
#else
    strcpy(cstr_, x);
#endif
  }
  void set_string(const std::string &x) { string_ = x; }
  void set_varchar(const matador::varchar_base &x) { varchar_ = x.c_str(); }
  void set_date(const matador::date &d) { date_ = d; }
  void set_time(const matador::time &d) { time_ = d; }

  char get_char() const { return char_; }
  float get_float() const { return float_; }
  double get_double() const { return double_; }
  short get_short() const { return short_; }
  int get_int() const { return int_; }
  long get_long() const { return long_; }
  unsigned short get_unsigned_short() const { return unsigned_short_; }
  unsigned int get_unsigned_int() const { return unsigned_int_; }
  unsigned long get_unsigned_long() const { return unsigned_long_; }
  bool get_bool() const { return bool_; }
  const char* get_cstr() const { return cstr_; }
  std::string get_string() const { return string_; }
  matador::varchar_base get_varchar() const { return varchar_; }
  matador::date get_date() const { return date_; }
  matador::time get_time() const { return time_; }

  friend std::ostream& operator <<(std::ostream &os, const Item &i)
  {
    os << "Item [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  enum { CSTR_LEN=256 };

  matador::identifier<unsigned long> id_ = 0;

  char char_ = 'c';
  float float_ = 3.1415f;
  double double_ = 1.1414;
  short short_ = -127;
  int int_ = -65000;
  long long_ = -128000;
  unsigned short unsigned_short_ = 128;
  unsigned int unsigned_int_ = 65000;
  unsigned long unsigned_long_ = 128000;
  bool bool_ = true;
  char cstr_[CSTR_LEN];
  std::string string_ = "Welt";
  matador::varchar<64> varchar_ = "Erde";
  matador::date date_;
  matador::time time_;
};

class ItemA : public Item {};
class ItemB : public Item {};
class ItemC : public Item {};

template < class T >
class ObjectItem : public Item
{
public:
  typedef matador::object_ptr<T> value_ptr;

  ObjectItem() = default;
  ObjectItem(const std::string &n, int i)
    : Item(n, i)
  {}

  template < class SERIALIZER > void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*matador::base_class<Item>(this));
    serializer.serialize("ref", ref_, matador::cascade_type::NONE);
    serializer.serialize("ptr", ptr_, matador::cascade_type::ALL);
  }

  void ref(const value_ptr &r)
  {
    ref_ = r;
  }
  void ptr(const value_ptr &p) { ptr_ = p; }

  value_ptr ref() const { return ref_; }
  value_ptr ptr() const { return ptr_; }

  template < class I >
  friend std::ostream& operator <<(std::ostream &os, const ObjectItem<I> &i)
  {
    os << "ObjectItem [" << i.get_string() << "] (" << i.get_int() << ")";
    return os;
  }

private:
  matador::has_one<T> ref_;
  matador::has_one<T> ptr_;
};

class ObjectItemList
{
public:
  typedef matador::has_many<ObjectItem<Item>> object_item_list_t;
  typedef typename object_item_list_t::iterator iterator;
  typedef typename object_item_list_t::const_iterator const_iterator;

  matador::identifier<unsigned long> id;
  std::string name;
  object_item_list_t items;

  ObjectItemList() = default;
  explicit ObjectItemList(std::string n) : name(std::move(n)) {}

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("name", name);
    s.serialize("object_item_list", items, "list_id", "object_item_id", matador::cascade_type::NONE);
  }

  iterator begin() { return items.begin(); }
  iterator end() { return items.end(); }

  const_iterator begin() const { return items.begin(); }
  const_iterator end() const { return items.end(); }

};

class book
{
private:
  matador::identifier<unsigned long> id_;
  std::string title_;
  std::string isbn_;
  std::string author_;

public:
  book() = default;
  book(std::string title, std::string isbn, std::string author)
    : title_(std::move(title))
    , isbn_(std::move(isbn))
    , author_(std::move(author))
  {}
  ~book() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("title", title_);
    serializer.serialize("isbn", isbn_);
    serializer.serialize("author", author_);
  }

  unsigned long id() const { return id_; }
  void id(unsigned long i)  { id_ = i; }
  std::string title() const { return title_; }
  std::string isbn() const { return isbn_; }
  std::string author() const { return author_; }
};

class book_list
{
public:
  typedef matador::has_many<book> book_list_t;
  typedef book_list_t::size_type size_type;
  typedef book_list_t::iterator iterator;
  typedef book_list_t::const_iterator const_iterator;
  
  book_list() = default;
  ~book_list() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("books", book_list_, "book_list_id", "book_id", matador::cascade_type::NONE);
  }

  void add(const matador::object_ptr<book> &b)
  {
    book_list_.push_back(b);
  }

  iterator begin() { return book_list_.begin(); }
  const_iterator begin() const { return book_list_.begin(); }

  iterator end() { return book_list_.end(); }
  const_iterator end() const { return book_list_.end(); }

  iterator erase(iterator i) { return book_list_.erase(i); }

  size_type size() const { return book_list_.size(); }
  bool empty() const { return book_list_.empty(); }
  
private:
  matador::identifier<unsigned long> id_;
  book_list_t book_list_;
};

class person
{
private:
  matador::identifier<unsigned long> id_;
  matador::varchar<255> name_;
  matador::date birthdate_;
  unsigned int height_ = 0;

public:
  person() = default;
  person(unsigned long id, const std::string &name, const matador::date &birthdate, unsigned int height)
    : id_(id)
    , name_(name)
    , birthdate_(birthdate)
    , height_(height)
  {}

  person(const std::string &name, const matador::date &birthdate, unsigned int height)
    : person(0, name, birthdate, height)
  {}

  virtual ~person() = default;

  template < class T >
  void serialize(T &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_);
    serializer.serialize("birthdate", birthdate_);
    serializer.serialize("height", height_);
  }

  void id(unsigned long i) { id_.value(i); }
  unsigned long id() const { return id_.value(); }

  std::string name() const { return name_.str(); }
  void name(const std::string &name) { name_ = name; }

  matador::date birthdate() const { return birthdate_; }
  void birthdate(const matador::date &birthdate) { birthdate_ = birthdate; }

  unsigned int height() const { return height_; }
  void height(unsigned int height) { height_ = height; }
};

struct department;

class employee : public person
{
public:
  matador::belongs_to<department> department_;
  
public:
  employee() = default;
  explicit employee(const std::string &name) : person(name, matador::date(17, 12, 1983), 183) {}
  employee(const std::string &name, const matador::object_ptr<department> &dep)
    : person(name, matador::date(17, 12, 1983), 183)
    , department_(dep)
  {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*matador::base_class<person>(this));
    serializer.serialize("department"    , department_, matador::cascade_type::NONE);
                         // name of table, object     , cascade
  }

  matador::object_ptr<department> dep() { return department_; }
  void dep(const matador::object_ptr<department> &d)
  {
    department_ = d;
  }
};

struct department
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> name;
  matador::has_many<employee> employees;

  department() = default;
  explicit department(std::string n)
    : name(std::move(n))
  {}
  
  ~department() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("employee"    , employees, "department", "id", matador::cascade_type::NONE);
    //                    name of table, container,  name of member
    //                                   to serialize
  }
};

class course;

class student : public person
{
public:
  student() = default;
  explicit student(const std::string &name, const matador::date &bdate = matador::date(), unsigned h = 170)
    : person(name, bdate, h) {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*matador::base_class<person>(this));
    serializer.serialize("student_course", courses, "student_id", "course_id", matador::cascade_type::NONE);
  }

  matador::has_many<course> courses;
};

class course
{
public:

  course() = default;
  explicit course(std::string t) : title(std::move(t)) {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("title", title);
    serializer.serialize("student_course", students, "student_id", "course_id", matador::cascade_type::ALL);
  }

  matador::identifier<unsigned long> id;
  std::string title;
  matador::has_many<student> students;
};

struct address;

struct citizen : public person
{
  citizen() = default;
  explicit citizen(const std::string &name, const matador::date &bdate = matador::date(), unsigned h = 170) : person(name, bdate, h) {}

  matador::has_one<address> address_;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(*matador::base_class<person>(this));
    serializer.serialize("address", address_, matador::cascade_type::ALL);
  }
};

struct address
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> street;
  matador::varchar<255> city;
  matador::belongs_to<citizen> citizen_;

  address() = default;
  address(const std::string &str, const std::string &c)
    : street(str), city(c)
  {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("street", street);
    serializer.serialize("city", city);
    serializer.serialize("citizen", citizen_, matador::cascade_type::NONE);
  }
};

class album;

class track
{
public:
  typedef matador::object_ptr<album> album_ptr;

private:
  matador::identifier<unsigned long> id_;
  std::string title_;
  matador::has_one<album> album_;
  int index_;

public:
  track() : index_(0) {}
  track(const std::string &title)
    : title_(title)
    , index_(0)
  {}
  
  ~track() {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("title", title_);
    serializer.serialize("album", album_);
    serializer.serialize("track_index", index_);
  }

  unsigned long id() { return id_.value(); }

  std::string title() const { return title_; }
  void title(const std::string &t) { title_ = t; }

  album_ptr alb() const { return album_; }
  void alb(const album_ptr &a) { album_ = a; }
  
  int index() const { return index_; }
  void index(int i) { index_ = i; }
};

class album
{
public:
  typedef matador::object_ptr<track> track_ptr;
  typedef matador::has_many<track> track_vector_t;
  typedef track_vector_t::size_type size_type;
  typedef track_vector_t::iterator iterator;
  typedef track_vector_t::const_iterator const_iterator;

private:
  matador::identifier<unsigned long> id_;
  std::string name_;
  track_vector_t tracks_;
  
public:
  album() = default;
  explicit album(std::string name) : name_(std::move(name)) {}
  
  ~album() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_);
    serializer.serialize("tracks", tracks_);
  }

  unsigned long id() { return id_.value(); }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const track_ptr &t)
  {
    tracks_.push_back(t);
  }

  iterator insert(iterator pos, const track_ptr &b)
  {
    return tracks_.insert(pos, b);
  }

  iterator begin() { return tracks_.begin(); }
  const_iterator begin() const { return tracks_.begin(); }

  iterator end() { return tracks_.end(); }
  const_iterator end() const { return tracks_.end(); }

  iterator erase(iterator i) { return tracks_.erase(i); }
  iterator erase(iterator a, iterator b) { return tracks_.erase(a, b); }

  size_type size() const { return tracks_.size(); }
  bool empty() const { return tracks_.empty(); }
};

class playlist
{
public:
  typedef matador::object_ptr<track> track_ref;
  typedef matador::has_many<track> track_list_t;
  typedef track_list_t::size_type size_type;
  typedef track_list_t::iterator iterator;
  typedef track_list_t::const_iterator const_iterator;

private:
  matador::identifier<unsigned long> id_;
  std::string name_;
  track_list_t tracks_;
  track_list_t backup_tracks_;
  
public:
  playlist() = default;
  explicit playlist(std::string name) : name_(std::move(name)) {}
  
  ~playlist() = default;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
    serializer.serialize("name", name_);
    serializer.serialize("playlist_tracks", tracks_);
    serializer.serialize("backup_tracks", backup_tracks_);
  }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  void add(const track_ref &b)
  {
    tracks_.push_back(b);
  }

  iterator begin() { return tracks_.begin(); }
  const_iterator begin() const { return tracks_.begin(); }

  iterator end() { return tracks_.end(); }
  const_iterator end() const { return tracks_.end(); }

  iterator erase(const iterator &i) { return tracks_.erase(i); }

  size_type size() const { return tracks_.size(); }
  bool empty() const { return tracks_.empty(); }
};

class child
{
public:
  child() = default;
  explicit child(std::string n) : name(std::move(n)) {}
  ~child() = default;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
  }

  matador::identifier<unsigned long> id;
  std::string name;
};

class master
{
public:
  matador::identifier<unsigned long> id;
  std::string name;
  matador::has_one<child> children;

public:
  master() = default;
  explicit master(std::string n) : name(std::move(n)) {}
  master(std::string n, const matador::object_ptr<child> &c) : name(std::move(n)), children(c) {}
  ~master() = default;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("child", children, matador::cascade_type::ALL);
  }
};

class children_vector
{
public:
  typedef matador::has_many<child> children_vector_t;

  children_vector() = default;
  explicit children_vector(std::string n) : name(std::move(n)) {}
  ~children_vector() = default;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("children", children, "vector_id", "child_id", matador::cascade_type::ALL);
  }

  matador::identifier<unsigned long> id;
  std::string name;
  children_vector_t children;
};

class children_list
{
public:
  typedef matador::has_many<child, std::list> children_list_t;

  children_list() = default;
  explicit children_list(std::string n) : name(std::move(n)) {}
  ~children_list() = default;

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("children", children, "list_id", "child_id", matador::cascade_type::ALL);
  }

  matador::identifier<unsigned long> id;
  std::string name;
  children_list_t children;
};

template < class T, template <class ...> class C >
class many_builtins
{
public:
  typedef matador::has_many<T, C> element_list_t;

public:
  matador::identifier<unsigned long> id;
  element_list_t elements;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("id", id);
    s.serialize("elements", elements, "list_id", "value", matador::cascade_type::ALL);
  }
};

using many_ints = many_builtins<int, std::list>;
using many_strings = many_builtins<std::string, std::list>;

using many_vector_ints = many_builtins<int, std::vector>;
using many_vector_strings = many_builtins<std::string, std::vector>;

#endif /* ITEM_HPP */
