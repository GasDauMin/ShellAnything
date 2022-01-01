/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "shellanything/Node.h"
#include <algorithm>
#include <stdexcept>

namespace shellanything
{

  Node::Node() :
    mParent(NULL)
  {
  }

  Node::Node(const std::string & type) :
    mParent(NULL)
  {
    mNodeType = type;
  }

  Node::~Node()
  {
    //delete children
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * node = mChildren[i];
      if (node)
        delete node;
    }
    mChildren.clear();
  }

  const std::string & Node::GetNodeType() const
  {
    return mNodeType;
  }

  Node * Node::GetParent() const
  {
    return mParent;
  }

  Node * Node::AddChild(Node * child)
  {
    if (child->mParent != NULL)
    {
      throw std::runtime_error("Node already in another tree.");
    }

    child->mParent = this;
    mChildren.push_back(child);

    return child;
  }

  bool Node::RemoveChild(Node * child)
  {
    Node::NodePtrList::iterator it = std::find(mChildren.begin(), mChildren.end(), child);
    if (it != mChildren.end())
    {
      //found!
      Node * node = (*it);
      mChildren.erase(it);
      delete node;
      return true;
    }
    return false;
  }

  Node::NodePtrList Node::GetChildren() const
  {
    return mChildren;
  }

  Node::NodePtrList Node::FindChildren(const std::string & type) const
  {
    Node::NodePtrList nodes;
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * n = mChildren[i];
      if (n->mNodeType == type)
        nodes.push_back(n);
    }
    return nodes;
  }

  Node * Node::FindFirst(const std::string & type) const
  {
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * n = mChildren[i];
      if (n->mNodeType == type)
        return n;
    }
    return NULL;
  }

  size_t Node::GetNumChildren() const
  {
    return mChildren.size();
  }

  Node * Node::GetChild(size_t index) const
  {
    if (index < mChildren.size())
    {
      Node * node = mChildren[index];
      return node;
    }
    return NULL;
  }

  bool Node::RemoveChild(size_t index)
  {
    if (index < mChildren.size())
    {
      Node * node = mChildren[index];
      mChildren.erase(mChildren.begin() + index);
      delete node;
      return true;
    }
    return false;
  }

  bool Node::RemoveChildren()
  {
    bool success = true;
    while(mChildren.size() > 0)
    {
      success = success && RemoveChild((size_t)0);
    }
    return success;
  }
 
  bool Node::RemoveChildren(const std::string & type)
  {
    bool success = true;
    Node * node = FindFirst(type);
    while(node != NULL)
    {
      success = success && RemoveChild(node);
      node = FindFirst(type);
    }
    return success;
  }

  size_t Node::Depth() const
  {
    size_t depth = 0;
    Node * ancestor = mParent;
    for (depth = 0; ancestor != NULL; depth++)
    {
      ancestor = ancestor->mParent;
    }
    return depth;
  }

  bool Node::IsLeaf() const
  {
    return (mChildren.size() == 0);
  }

  bool Node::IsRoot() const
  {
    return (mParent == NULL);
  }

  size_t Node::Size() const
  {
    size_t total = 1;
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * n = mChildren[i];
      size_t node_size = n->Size();
      total += node_size;
    }
    return total;
  }

} //namespace shellanything
