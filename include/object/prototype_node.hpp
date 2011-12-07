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

#ifndef PROTOTYPE_NODE_HPP
#define PROTOTYPE_NODE_HPP

#include "object/object_proxy.hpp"

#include <iostream>
#include <memory>

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
  #define EXPIMP_TEMPLATE
#endif

namespace oos {

class object_base_producer;
struct object_proxy;

/**
 * @struct prototype_node
 * @brief Holds the prototype of a concrete object.
 *
 * The prototype_node class holds the prototype of
 * a concrete object inside a producer object. Whenever
 * requested the class produces a new object.
 * 
 * It also holds a partial list containing all objects of
 * this and all children nodes.
 * This list is defined by three marker: the beginning
 * of the list, the end of the own objects and the end of
 * the last child objects.
 */ 
struct OOS_API prototype_node
{
  prototype_node();

  /**
   * @brief Creates a new prototype_node.
   * 
   * Creates a new prototype_node which creates an
   * object from given object_base_producer. The node
   * gets the given type name t.
   * 
   * @param p The object_base_producer.
   * @param t The type name of this node.
   */
  prototype_node(object_base_producer *p, const char *t);

  ~prototype_node();

  /**
   * clear removes all object proxies really belonging to
   * this node and deletes them and the holded object
   */
  void clear();

  /**
   * Returns true if object proxy list is empty. If self is true, only
   * list of own objects is checked. If self is false, complete list is
   * checked.
   * 
   * @param self If true only elements inside this node are considered.
   * @return True if the node is empty.
   */
  bool empty(bool self) const;
  
  /**
   * Returns the size of the object proxy list.
   * 
   * @return The number of objects.
   */
  unsigned long size() const;
  
  /**
   * Appends the given prototype node to the list of children.
   * 
   * @param child The child node to add.
   */
  void insert(prototype_node *child);
    
  /**
   * Removes node and cildren nodes from list, clears all object
   * and deletes all members.
   */
  void remove();

  /**
   * Unlinks node from list.
   */
  void unlink();

  /**
   * Returns nodes successor node or NULL if node is last.
   * 
   * @return The next node.
   */
  prototype_node* next_node() const;

  /**
   * Returns nodes successor node or NULL if node is last or
   * if node is the given root element.
   * 
   * @return The next node.
   */
  prototype_node* next_node(prototype_node *root) const;
  
  /**
   * Return nodes predeccessor node or NULL if node is first.
   * 
   * @return The previous node.
   */
  prototype_node* previous_node() const;

  /**
   * Returns true if node is child of given parent node.
   * 
   * @param parent The parent node.
   * @return True if node is child of given parent node.
   */
  bool is_child_of(const prototype_node *parent) const;

  /**
   * Adjusts self and last marker of all predeccessor nodes with given
   * object proxy.
   * 
   * @param old_proxy The old last marker proxy.
   * @param new_proxy The new last marker proxy.
   */
  void adjust_left_marker(object_proxy *old_proxy, object_proxy *new_proxy);
  
  /**
   * Adjust first marker of all successor nodes with given object proxy.
   * 
   * @param old_proxy The old first marker proxy.
   * @param new_proxy The new first marker proxy.
   */
  void adjust_right_marker(object_proxy *old_proxy, object_proxy *new_proxy);

  /**
   * Prints the node in graphviz layout to the stream.
   * 
   * @param os The ostream to write to.
   * @param pn The prototype_node to be written.
   * @return The modified ostream.
   */
  friend std::ostream& operator <<(std::ostream &os, const prototype_node &pn);

  // tree links
  prototype_node *parent;              /**< The parent node */
  prototype_node *prev;                /**< The previous node */
  prototype_node *next;                /**< The next node */
  std::auto_ptr<prototype_node> first; /**< The first children node */
  std::auto_ptr<prototype_node> last;  /**< The last children node */

  // data
  std::auto_ptr<object_base_producer> producer; /**< The object producer */

  object_proxy *op_first;  /**< The marker of the first list node. */
  object_proxy *op_marker; /**< The marker of the last list node of the own elements. */
  object_proxy *op_last;   /**< The marker of the last list node of all elements. */
  
  unsigned int depth;  /**< The depth of the node inside of the tree. */
  unsigned long count; /**< The total count of elements. */

  std::string type;	   /**< The type name of the object */
};

}

#endif /* PROTOTYPE_NODE_HPP */
