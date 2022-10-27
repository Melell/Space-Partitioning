/**
* @file octree.inl
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/11/07
* @brief Contains the implementation of the templated functions from the octree.cpp file.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

namespace cs350 {

    /**
    * @brief Computes the locational code for the given world_position's node.
    * @param world_position   The position in world coordinates as a vector of ints.
    * @param root_size        The size of one side of the root bv.
    * @param levels           The number of levels being used in the tree.
    * @return uint32_t        The locational code of the node to which world_position belongs.
    */
    template <int dimension>
    uint32_t compute_locational_code(glm::vec<dimension, int> world_position, uint32_t root_size, uint32_t levels)
    {
      // Tree too small
      if (root_size <= 1)
        return 1u;

      // Transform the points so that they are in the range [0, root_size)
      // Any points outside that range after transforming them, are added to the root
      uint32_t halfSize = root_size / 2;
      world_position += halfSize;

      // The final code that we will return
      uint32_t finalCode = 0;

      // For each axis
      for (int axis = 0; axis < dimension; ++axis)
        // If any coordinate is outside the root, return the code of the root
        if (world_position[axis] < 0 || world_position[axis] >= static_cast<int>(root_size))
          return 1u;

      // Find the exponent of the power of two that represents the half size of the root
      uint32_t bits = static_cast<uint32_t>(glm::log2(halfSize));

      // Make sure we don't introduce undefined behaviour by bit shifting more than the bits available
      assert(bits <= sizeof(uint32_t) * 8 + 1);

      int bitsToShift = bits - levels + 1;

      glm::vec<dimension, uint32_t> codes;
      for (int axis = 0; axis < dimension; ++axis)
        codes[axis] = world_position[axis];

      // Compute the locational codes on each axis
      if (bitsToShift >= 0)
        for (int axis = 0; axis < dimension; ++axis)
          codes[axis] >>= bitsToShift;
      else
        assert(false);

      // "Merge" the locational codes of each axis
      for (uint32_t i = 0; i < levels; ++i)
        for (int axis = 0; axis < dimension; ++axis)
          finalCode |= (codes[axis] & (1u << i)) << (i * (dimension - 1) + axis);

      uint32_t bitsUsed = levels * dimension;

      // Make sure we don't introduce undefined behaviour by bit shifting more than the bits available
      assert(bitsUsed + 1 <= sizeof(uint32_t) * 8);

      // Compute the position of the sentinel bit
      uint32_t sentinel = 1u << bitsUsed;

      // Add the sentinel bit
      finalCode |= sentinel;

      return finalCode;
    }


    /**
    * @brief Default constructs the root size and levels.
    */
    template<typename T>
    octree<T>::octree()
        :   m_root_size(128u)
        ,   m_levels(3u)
    {
    }

    /**
    * @brief Destroy all the existing nodes.
    */
    template<typename T>
    octree<T>::~octree()
    {
      destroy();
    }


    /**
    * @brief Deletes the memory of all the existing nodes and removes them from the container.
    */
    template<typename T>
    void octree<T>::destroy()
    {
      while (!m_nodes.empty())
      {
        auto beginIt = m_nodes.begin();
        delete beginIt->second;
        m_nodes.erase(beginIt);
      }
    }


    /**
    * @brief Finds and returns the node corresponding to bv. If it doesn't exist, it is created, and the new one is returned.
    * @param bv       The bounding volume whose node we are to find.
    * @return node *  The found node.
    */
    template<typename T>
    typename octree<T>::node* octree<T>::find_create_node(aabb const& bv)
    {
      return find_create_node<T>(compute_locational_code(bv, m_root_size, m_levels));
    }


    /**
    * @brief Finds and returns the node corresponding to bv. Returns nullptr if it is not found.
    * @param bv       The bounding volume whose node we are to find.
    * @return node *  The found node. Nullptr if it wasn't found.
    */
    template<typename T>
    typename octree<T>::node* octree<T>::find_node(aabb const& bv)
    {
      return find_node<T>(compute_locational_code(bv, m_root_size, m_levels));
    }


    /**
    * @brief Finds and returns the node corresponding to bv. Returns nullptr if it is not found. (Const overload)
    * @param bv       The bounding volume whose node we are to find.
    * @return node *  The found node. Nullptr if it wasn't found.
    */
    template<typename T>
    typename octree<T>::node const* octree<T>::find_node(aabb const& bv) const
    {
      return find_node<T>(bv);
    }


    /**
    * @brief Finds and returns the node corresponding to 'locational_code'. If it doesn't exist,
    *        it is created, and the new one is returned.
    * @param locational_code       The code of the node we want to find.
    * @return node *               The found node, or the newly created one.
    */
    template<typename T>
    typename octree<T>::node* octree<T>::find_create_node(uint32_t locational_code)
    {
      // Find the node
      auto foundIt = m_nodes.find(locational_code);

      // If it wasn't found, create it and add it to the map
      if (foundIt == m_nodes.end())
      {
        node * newNode = new node;
        newNode->locational_code = locational_code;
        newNode->first = nullptr;
        m_nodes[locational_code] = newNode;
        return newNode;
      }

      return foundIt->second;
    }


    /**
    * @brief Finds and returns the node corresponding to 'locational_code'. Returns nullptr if it doesn't exist.
    * @param locational_code       The code of the node we want to find.
    * @return node *               The found node, or nullptr if it wasn't found.
    */
    template<typename T>
    typename octree<T>::node* octree<T>::find_node(uint32_t locational_code)
    {
      auto foundIt = m_nodes.find(locational_code);
      if (foundIt == m_nodes.end())
        return nullptr;

      return foundIt->second;
    }


    /**
    * @brief Finds and returns the node corresponding to 'locational_code'. Returns nullptr if
    *        it doesn't exist. (Const overload)
    * @param locational_code       The code of the node we want to find.
    * @return node *               The found node, or nullptr if it wasn't found.
    */
    template<typename T>
    typename octree<T>::node const* octree<T>::find_node(uint32_t locational_code) const
    {
      return find_node<T>(locational_code);
    }


    /**
    * @brief Deletes the memory of the node corresponding to 'locational_code' and removes it from the container.
    *        Note that it only deletes the node corresponding to locational_code, and not its children or parents.
    * @param locational_code       The code of the node we want to delete.
    */
    template<typename T>
    void octree<T>::delete_node(uint32_t locational_code)
    {
      // Find it and check if it was found
      auto foundIt = m_nodes.find(locational_code);
      if (foundIt == m_nodes.end())
        return;
      
      // Delete the memory and remove it from the map
      delete foundIt->second;
      m_nodes.erase(foundIt);
    }


    /**
    * @brief Deletes the memory of the node corresponding to 'locational_code' and removes it from the container.
    *        Sets the bit of its parent to 0, and if the parent no longer has child nodes active, it is deleted.
    *        This is repeated recursively until the root node.
    * @param locational_code       The code of the node we want to delete.
    */
    template <typename T>
    void octree<T>::delete_node_rec(uint32_t locational_code)
    {
      const int dimension = 3;
      uint32_t maxValue = (1u << dimension) - 1;

      node * childNode = nullptr;
      node * parentNode = nullptr;

      // Find the child node and delete it
      childNode = find_node(locational_code);
      assert(childNode != nullptr);

      // If it doesn't have active children, delete it
      if (childNode->children_active == 0)
        delete_node(locational_code);

      // Until we reach the sentinel bit
      while (locational_code > 1)
      {
        // Find the parent node
        parentNode = find_node(locational_code >> dimension);

        // Set to 0 the active child bit of childNode
        if (parentNode != nullptr)
          parentNode->children_active &= ~(1u << (locational_code & maxValue));

        // Update the child node
        childNode = parentNode;

        // If the parent has no children active, delete it, otherwise, the rest of the parents won't be deleted either
        if (parentNode != nullptr)
        {
          if (parentNode->children_active == 0)
            delete_node(locational_code >> dimension);
          else
            return;
        }
        
        // Update locational code to the one of the parent
        locational_code >>= dimension;
      }
    }


    /**
    * @brief Finds the node corresponding to 'locational_code' and if there isn't any, creates
    *        a new one. If it is necessary creates all its parents until the root.
    * @param locational_code       The code of the node we want to create.
    * @return node *               The node we created.
    */
    template<typename T>
    typename octree<T>::node* octree<T>::create_node(uint32_t locational_code)
    {
        // Get the maximum value that the last dimension digits of the code can have
        const int dimension = 3;
        uint32_t maxValue = (1u << dimension) - 1;

        node * childNode = nullptr;
        node * parentNode = nullptr;
        node * toReturn = nullptr;

        // Create the node
        childNode = find_create_node(locational_code);
        toReturn = childNode;

        // Until we reach the sentinel bit, create the parent node (if it doesn't exist), and update its children_active variable
        while (locational_code > 1)
        {
          parentNode = find_create_node(locational_code >> dimension);
          parentNode->children_active |= (1u << (locational_code & maxValue));
          childNode = parentNode;
          locational_code >>= dimension;
        }

        return toReturn;
    }


    /**
    * @brief Debug draws the bvs of each node in the highlight_level specified. If -1 is specified, debug draw all.
    * @param highlight_level       The level of nodes we want to debug draw.
    */
    template<typename T>
    void octree<T>::debug_draw_levels(int highlight_level)
    {
      // Debug draw all the existing nodes if -1
      if (highlight_level == -1)
      {
        for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
        {
          debug_draw_aabb(compute_bv(it->first, m_root_size), {0.2f,0.6f,0.4f,0.5f}, debug_draw_type::wireframe);
        }
      }
      // Else, debug draw only the ones in the level highlight_level
      else
      {
        uint32_t exponent = 3 * highlight_level;

        uint32_t nodesInCurrentLevel = 1u << exponent;

        for (uint32_t i = 0; i < nodesInCurrentLevel; ++i)
        {
            uint32_t currentCode = nodesInCurrentLevel + i;
            node * currentNode = find_node(currentCode);
            if (currentNode != nullptr)
                debug_draw_aabb(compute_bv(currentCode, m_root_size), {0.2f,0.6f,0.4f,0.5f}, debug_draw_type::wireframe);
        }
      }
    }


    /**
    * @brief Adds an object of type T to the beginning of the linked list of this node.
    * @param object       A pointer to the object to add.
    */
    template<typename T>
    void octree<T>::node::push_front(T * object)
    {
      assert(object != nullptr);

      // Update object's connecting pointers
      object->octree_prev_object = nullptr;
      object->octree_next_object = first;

      // Update the previous first pointer's (if valid) prev pointer to object
      if (first != nullptr)
        first->octree_prev_object = object;

      // Set the first pointer to object
      first = object;
    }


    /**
    * @brief Removes object from the linked list of objects of type T of the node it belongs to.
    * @param object       A pointer to the object to remove.
    */
    template<typename T>
    void octree<T>::node::remove(T * object)
    {
      assert(object != nullptr);

      // If its the first, reset the first pointer
      if (object == first)
        first = object->octree_next_object;

      // If object's next pointer is valid, update next's prev pointer
      if (object->octree_next_object != nullptr)
        object->octree_next_object->octree_prev_object = object->octree_prev_object;

      // If object's prev pointer is valid, update prev's next pointer
      if (object->octree_prev_object != nullptr)
        object->octree_prev_object->octree_next_object = object->octree_next_object;

      // Reset all of its pointers to nullptr
      object->octree_next_object = nullptr;
      object->octree_prev_object = nullptr;
      object->octree_node = nullptr;
    }
}