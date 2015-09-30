//
// Created by sascha on 08.04.15.
//

#include "database/result_impl.hpp"

#include "object/object_exception.hpp"
#include "object/object_ptr.hpp"
#include "object/prototype_tree.hpp"
#include "object/serializable.hpp"

namespace oos {
namespace detail {

result_impl::result_impl(std::shared_ptr<object_base_producer> producer)
  : producer_(producer)
{}

result_impl::~result_impl()
{}

void result_impl::get(serializable *o)
{
  result_index = transform_index(0);
  o->deserialize(*this);
}

serializable* result_impl::fetch(const oos::prototype_node *node)
{
  node_ = node;

  std::unique_ptr<serializable> obj(node_->producer->create());

  if (!fetch(obj.get())) {
    obj.reset(nullptr);
  }

  node_ = nullptr;
  return obj.release();
}

void result_impl::read_foreign_object(const char *id, object_base_ptr &x)
{
  /*
   * deterime and create primary key of object ptr
   *
   */
  std::shared_ptr<basic_identifier> pk(x.create_identifier());

  pk->deserialize(id, *this);
  if (!pk->is_valid()) {
    // no pk is set => null
    return;
  }

  // set found primary key into object_base_ptr
  x.reset(pk);
}

//void result_impl::read(const char *id, object_base_ptr &x)
//{
  /*
   * deterime and create primary key of object ptr
   *
   */

//  std::shared_ptr<basic_identifier> pk = create_primary_key(x);
//
//  pk->deserialize(id, this);
//  if (!pk->is_valid()) {
//    // no pk is set => null
//    return;
//  }

  // set found primary key into object_base_ptr
//  x.identifier = pk;

  /*
   * read key
   * if valid key
   * create serializable with key
   * insert into object store
   * reset object ptr
   */
  // find prototype node of foreign key
//  prototype_node::t_foreign_key_map::const_iterator i = node_->foreign_keys.find(id);
//  if (i == node_->foreign_keys.end()) {
//    throw_object_exception("couldn't find foreign key for serializable of type'" << x.type() << "'");
//  }
//
//  /*
//   * clone new primary key and deserialize it
//   * if valid value is set create new proxy
//   * with primary key
//   */
//  std::shared_ptr<basic_identifier> pk(i->second->clone());
//  pk->deserialize(id, *this);
//  if (!pk->is_valid()) {
//    return;
//  }
//
//  // get node of object type
//  prototype_iterator xnode = node_->tree->find(x.type());
//
//  std::unique_ptr<object_proxy> proxy(xnode->find_proxy(pk));
//
//  if (!proxy) {
//    proxy.reset(new object_proxy(pk, const_cast<prototype_node*>(xnode.get())));
//  }
//
//  x.reset(proxy.release());
//}

const prototype_node* result_impl::node() const
{
  return node_;
}

std::shared_ptr<object_base_producer> result_impl::producer() const
{
  return producer_;
}

}
}