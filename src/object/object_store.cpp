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

#include "object/object.hpp"
#include "object/object_proxy.hpp"
#include "object/object_store.hpp"
#include "object/object_observer.hpp"
#include "object/object_list.hpp"
#include "object/object_creator.hpp"
#include "object/object_deleter.hpp"
#include "object/prototype_node.hpp"

#ifdef WIN32
#include <functional>
#include <memory>
#else
#include <tr1/functional>
#include <tr1/memory>
#endif

#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <stack>

using namespace std;
using namespace std::tr1::placeholders;

namespace oos {

class equal_type : public std::unary_function<const prototype_node*, bool> {
public:
  explicit equal_type(const std::string &type) : type_(type) {}

  bool operator() (const prototype_node *x) const {
    return x->type == type_;
  }
private:
  const std::string &type_;
};

object_store::object_store()
  : root_(new prototype_node(new object_producer<object>, "OBJECT"))
  , id_(0)
  , first_(new object_proxy(this))
  , last_(new object_proxy(this))
  , object_deleter_(new object_deleter)
{
  prototype_node_map_.insert(std::make_pair("OBJECT", root_));
  prototype_node_map_.insert(std::make_pair(root_->producer->classname(), root_));
  // set marker for root element
  root_->op_first = first_;
  root_->op_marker = last_;
  root_->op_last = last_;
  root_->op_first->next = root_->op_last;
  root_->op_last->prev = root_->op_first;
}

object_store::~object_store()
{
  clear();
  // delete all deleted object_proxys
  if (root_->op_first->next) {
//    delete root_->op_first->next;
  }
  if (root_->op_last->prev) {
//    delete root_->op_last->prev;
  }
  delete root_;
  delete first_;
  delete last_;
  delete object_deleter_;
}

bool
object_store::insert_prototype(object_base_producer *producer, const char *type, const char *parent)
{
  // set node to root node
  prototype_node *parent_node;
  if (parent) {
    // parent type name is set search parent node
    t_prototype_node_map::iterator i = prototype_node_map_.find(parent);
    if (i == prototype_node_map_.end()) {
      //throw new object_exception("couldn't find parent prototype");
      return false;
    }
    parent_node = i->second;
  }
  // find prototype node 'type' starting from node
  t_prototype_node_map::iterator i = prototype_node_map_.find(type);
  if (i != prototype_node_map_.end()) {
    //throw new object_exception("prototype already exists");
    return false;
  }
  prototype_node *node = new prototype_node(producer, type);
  // append as child to parent prototype node
  parent_node->insert(node);
  // store prototype in map
  prototype_node_map_.insert(std::make_pair(type, node));
  prototype_node_map_.insert(std::make_pair(producer->classname(), node));
  // return success
  return true;
}

bool object_store::remove_prototype(const char *type)
{
  t_prototype_node_map::iterator i = prototype_node_map_.find(type);
  if (i == prototype_node_map_.end()) {
    //throw new object_exception("couldn't find prototype");
    return false;
  }
  if (!i->second->parent) {
    // throw new object_exception("prototype has no parent");
    // no parent
    return false;
  }
  t_prototype_node_map::iterator j = prototype_node_map_.find(i->second->producer->classname());
  if (j == prototype_node_map_.end()) {
		// couldn't fnd prototype in type map
		// throw exception
		return false;
	}

  // remove (and delete) from tree (deletes subsequently all child nodes
  // for each child call remove_prototype(child);
  while (i->second->first->next != i->second->last) {
    prototype_node *node = i->second->first->next;
    remove_prototype(node->type.c_str());
  }
  // and objects they're containing 
  i->second->clear();
  // unlink node
  i->second->unlink();
  // delete node
  delete i->second;
  // erase node from maps
  prototype_node_map_.erase(i);
  prototype_node_map_.erase(j);
  return true;
}

const prototype_node* object_store::find_prototype(const char *type) const
{
  t_prototype_node_map::const_iterator i = prototype_node_map_.find(type);
  if (i == prototype_node_map_.end()) {
    //throw new object_exception("couldn't find prototype");
    return NULL;
  }
  return i->second;
}

void object_store::clear()
{
  while (root_->first->next != root_->last) {
    prototype_node *node = root_->first->next;
    remove_prototype(node->type.c_str());
    //delete node;
  }
//  cout << "size of object map: " << object_map_.size() << "\n";
//  cout << "clearing object map ... ";
  object_map_.clear();
//  cout << "done.\n";
//  cout << "size of object map: " << object_map_.size() << "\n";
}

int depth(prototype_node *node)
{
  int d = 0;
  while (node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

void object_store::dump_prototypes(std::ostream &out) const
{
  prototype_node *node = root_;
//  out << "dumping prototype tree:\n";
  out << "digraph G {\n";
  out << "\tgraph [fontsize=14]\n";
	out << "\tnode [color=\"#0c0c0c\", fillcolor=\"#dd5555\", shape=record, style=\"rounded,filled\", fontname=\"Verdana-Bold\"]\n";
	out << "\tedge [color=\"#0c0c0c\"]\n";
  do {
    int d = depth(node);
    for (int i = 0; i < d; ++i) out << " ";
    out << *node;
    node = node->next_node();
  } while (node);
  /*
  object_proxy *i = first_;
  while (i != last_) {
    out << "\t" << i << "[label=\"{" << i << "}\"]\n";
    i = i->next;
  }
  */
  out << "}" << std::endl;
}

void object_store::dump_objects(std::ostream &out) const
{
  out << "dumping all objects\n";

  object_proxy *op = first_;
  while (op) {
    out << "[" << op << "] (";
    if (op->obj) {
      out << *op->obj << " prev [" << op->prev->obj << "] next [" << op->next->obj << "])\n";
    } else {
      out << "object 0)\n";
    }
    op = op->next;
  }
}

object* object_store::create(const char *type) const
{
  t_prototype_node_map::const_iterator i = prototype_node_map_.find(type);
  if (i == prototype_node_map_.end()) {
    // try it with the type
    i = prototype_node_map_.find(type);
    if (i == prototype_node_map_.end()) {
      //throw new object_exception("couldn't find prototype");
      return NULL;
    }
  }
	return i->second->producer->create();
}

void object_store::mark_modified(object_proxy *oproxy)
{
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_update, _1, oproxy->obj));
}

void object_store::register_observer(object_observer *observer)
{
  if (std::find(observer_list_.begin(), observer_list_.end(), observer) == observer_list_.end()) {
    observer_list_.push_back(observer);
  }
}

void object_store::unregister_observer(object_observer *observer)
{
  t_observer_list::iterator i = std::find(observer_list_.begin(), observer_list_.end(), observer);
  if (i != observer_list_.end()) {
//    delete *i;
    observer_list_.erase(i);
  }
}

void
object_store::insert(object_list_base &olb)
{
  olb.install(this);
}

object*
object_store::insert_object(object *o, bool notify)
{
  // find type in tree
  if (!o) {
    // throw exception
    return NULL;
  }
  // find prototype node
  t_prototype_node_map::iterator i = prototype_node_map_.find(typeid(*o).name());
  if (i == prototype_node_map_.end()) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return NULL;
  }
  prototype_node *node = i->second;
  // retrieve and set new unique number into object
  object_proxy *oproxy = find_proxy(o->id());
  if (oproxy) {
    if (oproxy->linked()) {
      // an object exists in map.
      // replace it with new object
      // unlink it and
      // link it into new place in list
      remove_proxy(oproxy->node, oproxy);
    }
    oproxy->reset(o);
  } else {
    // object gets new unique id
    if (o->id() == 0) {
      o->id(++id_);
    }
    oproxy = create_proxy(o->id());
    oproxy->obj = o;
    if (!oproxy) {
      // throw exception
      return 0;
    }
  }
  // insert new element node
//  object_proxy *oproxy(new object_proxy(o, this));
  
  insert_proxy(node, oproxy);
  // create object
  object_creator oc(*this, notify);
  o->read_from(&oc);
  // notify observer
  if (notify) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_insert, _1, o));
  }
  // insert element into hash map for fast lookup
  object_map_[o->id()] = oproxy;
  // set this into persistent object
  o->proxy_ = oproxy;
  return o;
}

bool
object_store::remove(object *o)
{
  // check if object tree is deletable
  if (!object_deleter_->is_deletable(o)) {
    return false;
  }
  
  object_deleter::iterator first = object_deleter_->begin();
  object_deleter::iterator last = object_deleter_->end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.obj, true);
    } else {
      ++first;
    }
  }
	return true;
}
bool
object_store::remove_object(object *o, bool notify)
{
  // find prototype node
  t_prototype_node_map::iterator i = prototype_node_map_.find(typeid(*o).name());
  if (i == prototype_node_map_.end()) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return false;
  }

  if (object_map_.erase(o->id()) != 1) {
    // couldn't remove object
    // throw exception
    return false;
  }

  remove_proxy(i->second, o->proxy_);

  if (notify) {
    // notify observer
    std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_delete, _1, o));
  }
  // set object in object_proxy to null
  object_proxy *op = o->proxy_;
  // delete node
  delete op;
  // set node of proxy to NULL
//  op->obj = NULL;
  return true;
}

bool
object_store::remove(object_list_base &olb)
{
  /**************
   * 
   * remove all object from list
   * and first and last sentinel
   * 
   **************/
  // check if object tree is deletable
  if (!object_deleter_->is_deletable(olb)) {
    return false;
  }
  object_deleter::iterator first = object_deleter_->begin();
  object_deleter::iterator last = object_deleter_->end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.obj, true);
    } else {
      ++first;
    }
  }
  olb.uninstall();
  return true;
}

void
object_store::link_proxy(object_proxy *base, object_proxy *prev_proxy)
{
  // link oproxy before this node
  prev_proxy->prev = base->prev;
  prev_proxy->next = base;
  if (base->prev) {
    base->prev->next = prev_proxy;
  }
  base->prev = prev_proxy;
}

void
object_store::unlink_proxy(object_proxy *proxy)
{
  if (proxy->prev) {
    proxy->prev->next = proxy->next;
  }
  if (proxy->next) {
    proxy->next->prev = proxy->prev;
  }
  proxy->prev = NULL;
  proxy->next = NULL;
}

object_proxy* object_store::find_proxy(long id) const
{
  t_object_proxy_map::const_iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return NULL;
  } else {
    return i->second;
  }
}

object_proxy* object_store::create_proxy(long id)
{
  if (id == 0) {
    return NULL;
  }
  
  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return object_map_.insert(std::make_pair(id, new object_proxy(id, this))).first->second;
  } else {
    return 0;
  }
}

bool object_store::delete_proxy(long id)
{
  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return false;
  } else if (i->second->linked()) {
    return false;
  } else {
    object_map_.erase(i);
    return true;
  }
}

void object_store::insert_proxy(prototype_node *node, object_proxy *oproxy)
{
  // check count of object in subtree
  if (node->count >= 2) {
    // there are more than two objects (normal case)
    // insert before last last
    //cout << "more than two elements: inserting " << *o << " before second last (" << *node->op_marker->prev->obj << ")\n";
//    link_proxy(node->op_marker->prev, oproxy);
    oproxy->link(node->op_marker->prev);
//    node->op_marker->prev->insert(oproxy);
  } else if (node->count == 1) {
    // there is one object in subtree
    // insert as first; adjust "left" marker
    /*if (node->op_marker->prev->obj) {
      cout << "one element in list: inserting " << *o << " as first (before: " << *node->op_marker->prev->obj << ")\n";
    } else {
      cout << "one element in list: inserting " << *o << " as first (before: [0])\n";
    }*/
//    link_proxy(node->op_marker->prev, oproxy);
    oproxy->link(node->op_marker->prev);
//    node->op_marker->prev->insert(oproxy);
    node->adjust_left_marker(oproxy->next, oproxy);
  } else /* if (node->count == 0) */ {
    // there is no object in subtree
    // insert as last; adjust "right" marker
    /*if (node->op_marker->obj) {
      cout << "list is empty: inserting " << *o << " as last before " << *node->op_marker->obj << "\n";
    } else {
      cout << "list is empty: inserting " << *o << " as last before [0]\n";
    }*/
//    link_proxy(node->op_marker, oproxy);
    oproxy->link(node->op_marker);
//    node->op_marker->insert(oproxy);
    node->adjust_left_marker(oproxy->next, oproxy);
    node->adjust_right_marker(oproxy->prev, oproxy);
  }
  // set prototype node
  oproxy->node = node;
  // adjust size
  ++node->count;
}

void object_store::remove_proxy(prototype_node *node, object_proxy *oproxy)
{
  if (oproxy == node->op_first->next) {
    // adjust left marker
    //cout << "remove: object proxy is left marker " << *o << " before second last (" << *node->op_marker->prev->obj << ")\n";
    node->adjust_left_marker(node->op_first->next, node->op_first->next->next);
    //adjust_left_marker(node, node->op_first->next->next);
  }
  if (oproxy == node->op_marker->prev) {
    // adjust right marker
    node->adjust_right_marker(oproxy, node->op_marker->prev->prev);
    //adjust_right_marker(node, o->proxy_, node->op_marker->prev->prev);
  }
  // unlink object_proxy
  unlink_proxy(oproxy);
  // adjust object count for node
  --node->count;
}

}
