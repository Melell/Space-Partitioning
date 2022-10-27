/**
* @file octree.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/11/07
* @brief Contains the implementation of the octree class, as well as some functions
*        to work with locational codes.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "octree.hpp"

namespace cs350 {

    /**
    * @brief Helper function that prints the given number in its binary form.
    * @param number   The number to print
    */
    void print_binary(uint32_t number)
    {
      // Make maximum of 8 bits since we know it won't use more than that for now, to make it more readable
      for (int i = 8/*sizeof(uint32_t) * 8 - 1*/; i >= 0; --i)
      {
        uint32_t isBitSet = number & (1 << i);

        if (isBitSet)
          std::cout<<"1 ";
        else
          std::cout<<"0 ";
      }

      std::cout<<std::endl;
    }


    /**
    * @brief Computes the locational code of the node that is common to lc1 and lc2.
    * @param lc1          The first locational code.
    * @param lc2          The second locational code.
    * @return uint32_t    The common locational code.
    */
    uint32_t common_locational_code(uint32_t lc1, uint32_t lc2)
    {
      // Optimization: return one of them right away if they are identical
      if (lc1 == lc2)
        return lc1;

      uint32_t totalBits = sizeof(uint32_t) * 8;    // The total number of bits that the codes can have
      uint32_t commonCode = 0;                      // The common locational code that we will return
      int sentinel1Index = -1;                      // The index of the sentinel bit of lc1
      int sentinel2Index = -1;                      // The index of the sentinel bit of lc2
      const int dimension = 3;                      // Assume the dimension is 3 (only valid for octrees)
      
      // Find the index of the sentinel bit in each of the codes
      for (int i = totalBits - 1; i >= 0; --i)
      {
        // These will parse each bit string from the end till the start
        uint32_t currentBit1 = lc1 & (1u << i);
        uint32_t currentBit2 = lc2 & (1u << i);

        // The first time a 1 is found on each bit string, store its index
        if (currentBit1 && sentinel1Index == -1)
          sentinel1Index = i;

        if (currentBit2 && sentinel2Index == -1)
          sentinel2Index = i;

        // If the first 1 has already been found on each bit string, end the loop
        if (sentinel1Index != -1 && sentinel2Index != -1)
          break;
      }

      // Get the maximum index for a sentinel
      int maxSentinel = glm::max(sentinel1Index, sentinel2Index);
      int minSentinel = glm::min(sentinel1Index, sentinel2Index);
      int maxMinOffset = maxSentinel - minSentinel;

      // Identify each code based on their sentinel index
      uint32_t maxCode = glm::max(lc1, lc2);
      uint32_t minCode = glm::min(lc1, lc2);

      // For each bit starting from the sentinel
      int i = 0;
      for (; i < maxSentinel; i += dimension)
      {
        // For each axis
        for (int axis = 0; axis < dimension; ++axis)
        {
          // Compute whether the current bit in the string with more levels is 1 or 0
          int maxIdxToCompare = maxSentinel - 1 - i - axis;
          uint32_t bitMax = maxCode & (1u << maxIdxToCompare);

          // If there are no more bits to compare in the smaller (in terms of levels) bit string, then return
          int minIdxToCompare = minSentinel - 1 - i - axis;
          if (minIdxToCompare < 0)
          {
            // Shift to the right the bits that are not in commone, add the sentinel, and return
            commonCode >>= (maxSentinel - i);
            commonCode |= (1u << i);
            return commonCode;
          }

          // Compute whether the current bit in the string with less levels is 1 or 0
          uint32_t bitMin = (minCode & (1u << minIdxToCompare)) << maxMinOffset;

          // The current level is not a common code
          if (bitMax != bitMin)
          {
            // Shift to the right the bits that are not in commone, add the sentinel, and return
            commonCode >>= (maxSentinel - i);
            commonCode |= (1u << i);
            return commonCode;
          }

          // Add the current bit as it is common
          commonCode |= bitMax;
        }
      }

      // Add the sentinel bit and return
      commonCode |= (1u << i);
      return commonCode;
    }


    /**
    * @brief Computes the bounding volume of the node corresponding to locational_code.
    * @param locational_node    The locational code of node whose bounding volume we have to compute.
    * @param root_size          The size of the side of the root's bounding volume.
    * @return aabb              The bounding volume of the of node with code locational_code.
    */
    aabb compute_bv(uint32_t locational_code, uint32_t root_size)
    {
      // Start with the bv as the bv of the root
      int halfSize = static_cast<int>(root_size) / 2;
      aabb result(glm::vec3(static_cast<float>(-halfSize)), glm::vec3(static_cast<float>(halfSize)));

      // Compute the necessary variables for the loop (namely the index of the sentinel bit)
      const int dimension = 3;
      uint32_t depth = locational_code_depth(locational_code);
      uint32_t sentinelIndex = depth * dimension;

      // From the sentinel bit, until the end of the bit string
      for (int i = sentinelIndex - 1; i >= 0; i -= dimension)
      {
        for (int axis = 0; axis < dimension; ++axis)
        {
          // Check if the current bit is 1
          uint32_t isBitOne = locational_code & (1u << (i - (dimension - 1 - axis)));

          // Handle case in which it is 1
          if (isBitOne)
            result.mMinPos[axis] += root_size >> ((sentinelIndex - 1 - i) / 3 + 1);
          // Handle case in which it is 0
          else
            result.mMaxPos[axis] -= root_size >> ((sentinelIndex - 1 - i) / 3 + 1);
        }
      }

      return result;
    }


    /**
    * @brief Computes the number of levels that are being used according to the given code lc.
    * @param lc               The locational code from which we will read the number of levels.
    * @return uint32_t        The depth indicated by lc.
    */
    uint32_t locational_code_depth(uint32_t lc)
    {
      uint32_t totalBits = sizeof(uint32_t) * 8;    // The total number of bits that the code can have
      const int dimension = 3;                      // The dimension, in case we decide to use a quadtree or something else

      // Find the index of the sentinel bit in the code
      for (int i = totalBits - 1; i >= 0; --i)
      {
        // Parse the bit string from the end until the sentinel is found
        uint32_t currentBit = lc & (1u << i);

        // When the sentinel is found, the depth is sentinelIndex / dimension
        if (currentBit)
          return static_cast<uint32_t>(i / dimension);
      }

      // Just in case no sentinel is found
      return 0;
    }


    /**
    * @brief Computes the locational code for the given bounding volume bv.
    * @param bv               The bounding volume whose locational code we are to compute.
    * @param root_size        The size of one side of the root bv.
    * @param levels           The number of levels being used in the tree.
    * @return uint32_t        The code for bv.
    */
    uint32_t compute_locational_code(aabb const& bv, uint32_t root_size, uint32_t levels)
    {
      // Get the minimum point of the bv floored
      glm::vec<3,int> minFloored{ static_cast<int>(glm::floor(bv.mMinPos.x)),
                                  static_cast<int>(glm::floor(bv.mMinPos.y)),
                                  static_cast<int>(glm::floor(bv.mMinPos.z)) };

      // Get the maximum point of the bv ceiled
      glm::vec<3,int> maxCeiled{ static_cast<int>(glm::ceil(bv.mMaxPos.x)),
                                 static_cast<int>(glm::ceil(bv.mMaxPos.y)),
                                 static_cast<int>(glm::ceil(bv.mMaxPos.z)) };

      // Compute the locational code of min and max floored and ceiled respectively
      uint32_t minCode = compute_locational_code<3>(minFloored, root_size, levels);
      uint32_t maxCode = compute_locational_code<3>(maxCeiled, root_size, levels);

      // The code of bv is the code that minCode and maxCode have in common
      return common_locational_code(minCode, maxCode);
    }
}