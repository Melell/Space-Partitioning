/**
* @file mesh_data.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/19/09
* @brief Contains the definitions for the mesh_data class, as
*        well as the obj loading functions.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "mesh_data.hpp"

namespace cs350 {

    /**
    * @brief This is called inside the main loop of the application, and
    *        it is the main update for the demo of assignment 1.
    * @param  filename      The name of the OBJ file containing the mesh information.
    * @return mesh_data     Returns a mesh_data struct with the data read from the file.
    */
    mesh_data load_obj(const char* filename)
    {
        // The final vertex data that we will return
        mesh_data finalVertexData;

        std::stringstream ss;               // The stringstream for extracting the first character
        std::ifstream inFile(filename);     // The obj input file (opened in the ctor)
        std::string currentLine = "";       // Each line of the file will be written into this var
        std::string firstChar = "";         // To extract the first character of each line
        
        // To store temporarily the values extracted
        glm::vec3 tempVec3;
        glm::vec2 tempVec2;
        int tempInt;

        // Make sure the file is open
        assert(inFile.is_open());

        // Read the file line by line
        while (std::getline(inFile, currentLine))
        {
            firstChar = "";

            // Set the stringstream to the contents of the
            // current line, and extract the first character
            ss.clear();
            ss.str("");
            ss.str(currentLine);
            ss >> firstChar;


            // In the case of positions, UVs, and normals, extract the
            // values and push them to their corresponding vectors

            // Case vertex position
            if (firstChar == "v")
            {
                ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
                finalVertexData.positions.push_back(tempVec3);
            }
            // Case texture coordinate
            else if (firstChar == "vt")
            {
                ss >> tempVec2.x >> tempVec2.y;
                finalVertexData.uvs.push_back(tempVec2);
            }
            // Case vertex normal
            else if (firstChar == "vn")
            {
                ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
                finalVertexData.normals.push_back(tempVec3);
            }
            // Case indices
            else if (firstChar == "f")
            {
                int attribute = 0;
                // Read next character as an int (will always be an index)
                while (ss >> tempInt)
                {
                    // Push to the indices vector of the corresponding attribute
                    if (attribute == 0)
                        finalVertexData.mPosIndices.push_back(tempInt - 1);
                    else if (attribute == 1)
                        finalVertexData.mUVsIndices.push_back(tempInt - 1);
                    else if (attribute == 2)
                        finalVertexData.mNormalsIndices.push_back(tempInt - 1);

                    // If next character is '/'
                    if (ss.peek() == '/')
                    {
                        // Go to next attribute and jump the '/'
                        ++attribute;
                        ss.ignore(1, '/');

                        // Do the same in case we are using pos and normals but not UVs
                        if (ss.peek() == '/')
                        {
                            ++attribute;
                            ss.ignore(1, '/');
                        }
                    }
                    // If there is a space, increase the attribute considerably
                    // to avoid problems when we are only using positions
                    else if (ss.peek() == ' ')
                    {
                        attribute += 3;
                        ss.ignore(1, ' ');
                    }

                    // Reset the attribute controller
                    if (attribute > 2)
                        attribute = 0;
                }
            }
            // Some other character
            //else
            //{
            //}
        }

        //std::cout<<filename<<" loaded successfully.\n";

        return finalVertexData;
    }



    //std::vector<mesh_data> load_objs(const char* filename)
    //{

    //}
}