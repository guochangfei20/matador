#include "matador/orm/relation_resolver.hpp"
#include "relation_resolver.hpp"

namespace matador {

namespace detail {

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && !matador::is_builtin<typename T::right_value_type>::value>::type>::serialize(const char *, V &x)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
  if (left_table_ptr_->is_loaded()) {
    has_many_item_holder<V> value(x, nullptr);
    left_endpoint_->insert_value_into_foreign(value, left_proxy_);
  } else {
    auto lptr = std::static_pointer_cast<table<left_value_type>>(left_table_ptr_);

    lptr->append_relation_data(table_.name(), left_proxy_->pk(), x);
  }
}

template < class T >
void relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && !matador::is_builtin<typename T::right_value_type>::value>::type>::serialize(const char *, char *, size_t)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
}

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && !matador::is_builtin<typename T::right_value_type>::value>::type>::serialize(const char *, belongs_to<V> &x, cascade_type cascade)
{
  // increase reference count of has_many_to_xxx item proxy
  // because there will be an object for this kind of field
  ++(*proxy_);
  // check wether is left or right side value
  // left side will be determined first
  std::shared_ptr<basic_identifier> pk = x.primary_key();
  if (!pk) {
    return;
  }

  if (left_proxy_ == nullptr) {
    // if left is not loaded
    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);

  } else {

    object_proxy* right_proxy = acquire_proxy(x, pk, cascade, right_table_ptr_);
    if (left_table_ptr_->is_loaded()) {
      left_endpoint_->insert_value_into_foreign(right_proxy, left_proxy_);
    } else {
      auto lptr = std::static_pointer_cast<table<left_value_type>>(left_table_ptr_);

//      ++(*right_proxy);
      lptr->append_relation_data(table_.name(), left_proxy_->pk(), object_pointer<right_value_type, object_holder_type::OBJECT_PTR>(right_proxy), proxy_);
    }

    if (right_table_ptr_->is_loaded()) {
      has_many_item_holder<right_value_type > holder(right_proxy, proxy_);
      right_endpoint_->insert_value_into_foreign(holder, left_proxy_);
    } else {
//      ++(*left_proxy_);

      auto rptr = std::static_pointer_cast<table<right_value_type>>(right_table_ptr_);
      rptr->append_relation_data(table_.name(), right_proxy->pk(), object_pointer<left_value_type, object_holder_type::OBJECT_PTR>(left_proxy_), proxy_);
    }

  }
}

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && matador::is_builtin<typename T::right_value_type>::value>::type>::serialize(const char *, V &x)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
  if (left_table_ptr_->is_loaded()) {
    has_many_item_holder<V> value(x, nullptr);
    left_endpoint_->insert_value_into_foreign(value, left_proxy_);
  } else {
    auto lptr = std::static_pointer_cast<table<left_value_type>>(left_table_ptr_);

    lptr->append_relation_data(table_.name(), left_proxy_->pk(), x, proxy_);
  }
}

template < class T >
void relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && matador::is_builtin<typename T::right_value_type>::value>::type>::serialize(const char *, char *, size_t)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
}

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, T>::value && matador::is_builtin<typename T::right_value_type>::value>::type>::serialize(const char *, belongs_to<V> &x, cascade_type cascade)
{
  // check wether is left or right side value
  // left side will be determined first
  std::shared_ptr<basic_identifier> pk = x.primary_key();
  if (!pk) {
    return;
  }

  if (left_proxy_ == nullptr) {
    // if left is not loaded
    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);

  } else {
    throw_object_exception("right value type must not be object pointer");
  }
}

}
}