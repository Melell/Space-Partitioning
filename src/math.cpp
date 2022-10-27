/**
* @file math.cpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/04/10
* @brief Contains the implementation of helpful mathematical functions.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/


#include "pch.hpp"
#include "math.hpp"

namespace cs350
{
    /**
    * @brief Returns true if the values passed as parameter are equal
    *        taking into a account a specific offset defined by cEpsilon.
    * @param x     The first value to compare.
    * @param y     The second value to compare.
    * @return bool  True if they are equal taking into account the epsilon, false otherwise.
    */
    bool are_equal(float x, float y)
    {
        if (x <= y + cEpsilon && x >= y - cEpsilon)
            return true;

        return false;
    }
}