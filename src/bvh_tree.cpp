/**
* @file binary_tree.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/21/10
* @brief Contains the definitions for the binary_tree structure.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "bvh_tree.hpp"
#include "mesh_data.hpp"

namespace cs350
{
    /**
    * @brief Default constructs all the elements of the node.
    */
    bvh_node::bvh_node()
        :   mId(0),
            mNodeType(tree_node_type::internal),
            mLeft(nullptr),
            mRight(nullptr)
    {
    }


    /**
    * @brief Sets root to nullptr.
    */
    bvh_tree::bvh_tree()
        :   mRoot(nullptr),
            mBuildMethod(bvh_construction_method::not_constructed)
    {
    }


    /**
    * @brief Frees the memory used by the tree.
    */
    bvh_tree::~bvh_tree()
    {
        clear();
    }


    /**
    * @brief Acts as a dispatcher for the different construction methods of this mesh's
    *        bounding volume hierarchy. Also, the current tree is cleared before constructing.
    * @param building_method     The method used to construct the bvh.
    */
    void bvh_tree::construct_bvh(const mesh_data & vert_data, bvh_construction_method building_method)
    {
        // Avoid reconstructing the same tree
        if (building_method == mBuildMethod)
            return;

        mBuildMethod = building_method;

        clear();

        switch (building_method)
        {
        case bvh_construction_method::top_down:
            top_down_bvh(&mRoot, vert_data.positions, vert_data.mPosIndices);
            break;

        case bvh_construction_method::bottom_up:
            bottom_up_bvh(vert_data.positions, vert_data.mPosIndices);
            break;

        case bvh_construction_method::insertion:

            break;
        
        default:
            std::cout<<"DEBUG : BVH : UNKNOWN BVH CONSTRUCTION METHOD\n";
            break;
        }
    }


    /**
    * @brief Computes the bounding volume for the positions and position indices passed as parameter.
    * @param positions          The positions whose bv we are to compute.
    * @param posIndices         The indices to the positions vector, to form each triangle.
    * @return aabb              The bounding volume generated.
    */
    aabb bvh_tree::compute_bv(const std::vector<glm::vec3> & positions, const std::vector<unsigned> & posIndices) const
    {
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
        
        // For each triangle vertex
        for (auto currIndex : posIndices)
        {
            // For each axis
            for (int axis = 0; axis < 3; ++axis)
            {
                // Get the minimum of the current axis
                if (positions[currIndex][axis] < min[axis])
                    min[axis] = positions[currIndex][axis];

                // Get the maximum of the current axis
                if (positions[currIndex][axis] > max[axis])
                    max[axis] = positions[currIndex][axis];
            }
        }

        // Set the bounding volume
        aabb bv;
        bv.mMinPos = min;
        bv.mMaxPos = max;
        return bv;
    }


    bool bvh_tree::top_down_partition(bvh_node * nodeToDiv,
                                      const std::vector<glm::vec3> & inPositions,
                                      const std::vector<unsigned> & inIndices,
                                      std::vector<unsigned> & outLeftIndices,
                                      std::vector<unsigned> & outRightIndices)
    {
        assert(nodeToDiv != nullptr);

        // Get the partition axis as the longest cardinal axis
        const glm::vec3 & sizeVec = nodeToDiv->mBV.mMaxPos - nodeToDiv->mBV.mMinPos;
        float maxDist = sizeVec.x;
        int maxAxis = 0;
        glm::vec3 maxAxisDir = glm::vec3(1.0f, 0.0f, 0.0f);
        for (int axis = 1; axis < 3; ++axis)
        {
            if (sizeVec[axis] > maxDist)
            {
                maxDist = sizeVec[axis];
                maxAxis = axis;
                maxAxisDir = glm::vec3(0.0f, 0.0f, 0.0f);
                maxAxisDir[maxAxis] = 1.0f;
            }
        }

        // Get the partition point as the average of all the centroids
        glm::vec3 partPoint = glm::vec3(0.0f, 0.0f, 0.0f);
        for (size_t i = 0; i < inIndices.size(); i += 3)
        {
            glm::vec3 centroid = (inPositions[inIndices[i]] + inPositions[inIndices[i + 1]] + inPositions[inIndices[i + 2]]) / 3.0f;
            partPoint += centroid;
        }
        size_t numberOfTriangles = inIndices.size() / 3;
        partPoint /= numberOfTriangles;

        // Finally, compute the partition plane. With it, we can now start partitioning.
        plane partPlane(partPoint, maxAxisDir);
        outLeftIndices.clear();     // These are supposed to be size 0, but just in case, clear them
        outRightIndices.clear();

        for (size_t i = 0; i < inIndices.size(); i += 3)
        {
            glm::vec3 centroid = (inPositions[inIndices[i]] + inPositions[inIndices[i + 1]] + inPositions[inIndices[i + 2]]) / 3.0f;
            classification_t pointvsPlane = classify_plane_point(partPlane, centroid, cEpsilon);

            // If current centroid is inside the partition plane, put the
            // current triangle's vertices in the left node
            if (pointvsPlane == classification_t::inside)
            {
                outLeftIndices.push_back(inIndices[i]);
                outLeftIndices.push_back(inIndices[i + 1]);
                outLeftIndices.push_back(inIndices[i + 2]);
            }
            // Else, put them in the right node
            else
            {
                outRightIndices.push_back(inIndices[i]);
                outRightIndices.push_back(inIndices[i + 1]);
                outRightIndices.push_back(inIndices[i + 2]);
            }
        }

        // If the partition function failed (all triangles went to one side)
        if (outLeftIndices.size() == 0 || outRightIndices.size() == 0)
        {
            nodeToDiv->mNodeType = tree_node_type::leaf;
            //nodeToDiv->mIndices = outLeftIndices.size() == 0 ? outRightIndices : outLeftIndices;
            return false;
        }
        
        return true;
    }


    /**
    * @brief Constructs the bounding volume hierarchy tree using the top-down method.
    * @param tree                The tree we want to fill.
    * @param positions           The positions whose bv we are to compute.
    * @param posIndices          The indices to the positions vector, to form each triangle.
    */
    void bvh_tree::top_down_bvh(bvh_node ** tree, const std::vector<glm::vec3> & positions, const std::vector<unsigned> & posIndices)
    {
        // Nothing to do if there are no triangles to be added to this node
        if (posIndices.size() / 3 == 0)
            return;

        bvh_node * newNode = create_node(compute_bv(positions, posIndices), tree_node_type::internal);
        *tree = newNode;

        // If it's a leaf, end recursion
        if (posIndices.size() / 3 == 1)
        {
            newNode->mNodeType = tree_node_type::leaf;
            newNode->mIndices = posIndices;
        }
        // If it's not a leaf
        else
        {
            // Partition the current bv
            std::vector<unsigned> leftIndices;
            std::vector<unsigned> rightIndices;
            bool failed = !top_down_partition(newNode, positions, posIndices, leftIndices, rightIndices);

            if (failed)
                return;

            // Recursively make more nodes for each partition
            top_down_bvh(&(newNode->mLeft), positions, leftIndices);
            top_down_bvh(&(newNode->mRight), positions, rightIndices);
        }
    }


    /**
    * @brief Finds two candidate nodes to merge from nodes. Returns their indexes in
    *        dest and src. Uses the minimum surface area heuristic. Also, returns
    *        the bounding volume that we would get after merging them.
    * @param positions      The positions of the triangles vertices to index from.
    * @param nodes          The nodes from which we have to find two candidates.
    * @param numberOfNodes  The number of nodes in nodes.
    * @param dest           Out-parameter to return the destination for the merge.
    * @param src            Out-parameter to return the source of the merge.
    * @return aabb          Returns the bv that we would get after merging the
    *                       candidate nodes.
    */
    aabb bvh_tree::find_merge_candidates(const std::vector<glm::vec3> & positions, 
                                         const std::vector<bvh_node*> & nodes,
                                         int numberOfNodes,
                                         int & dest,
                                         int & src)
    {
        float minSurfArea = std::numeric_limits<float>::max();
        aabb resultingBV;

        // For each node
        for (int i = 0; i < numberOfNodes - 1; ++i)
        {
            // For every other node, starting from the current one
            for (int j = i + 1; j < numberOfNodes; ++j)
            {
                const aabb & bv1 = nodes[i]->mBV;
                const aabb & bv2 = nodes[j]->mBV;

                // Find the minimum and the maximum points between both bvs
                const glm::vec3 & min = glm::min(bv1.mMinPos, bv2.mMinPos);
                const glm::vec3 & max = glm::max(bv1.mMaxPos, bv2.mMaxPos);
                const glm::vec3 & sizeVec = max - min;

                // Compute the surface area
                float area1 = sizeVec.x * sizeVec.y * 2.0f;
                float area2 = sizeVec.x * sizeVec.z * 2.0f;
                float area3 = sizeVec.y * sizeVec.z * 2.0f;
                float surfaceArea = area1 + area2 + area3;

                if (surfaceArea < minSurfArea)
                {
                    minSurfArea = surfaceArea;
                    resultingBV.mMinPos = min;
                    resultingBV.mMaxPos = max;
                    dest = i;
                    src = j;
                }
            }
        }

        return resultingBV;
    }


    /**
    * @brief Constructs the bounding volume hierarchy tree using the bottom-up method.
    * @param positions           The positions of the triangles that we are going to merge.
    * @param posIndices          The indices to the positions vector, to form each triangle.
    */
    void bvh_tree::bottom_up_bvh(const std::vector<glm::vec3> & positions, const std::vector<unsigned> & posIndices)
    {
        // Create the leafs of the tree containing the original triangles
        size_t numberOfTriangles = posIndices.size() / 3;
        std::vector<bvh_node*> leafs(numberOfTriangles);

        int dest = 0;
        int src = 0;

        // Set each of the new nodes with the corresponding information
        for (size_t i = 0; i < posIndices.size(); i += 3)
        {
            std::vector<unsigned> indices(3);
            indices[0] = posIndices[i];
            indices[1] = posIndices[i + 1];
            indices[2] = posIndices[i + 2];
            bvh_node * leafNode = create_node(compute_bv(positions, indices), tree_node_type::leaf);
            leafNode->mIndices = indices;
            leafs[i / 3] = leafNode;
        }

        size_t currNumberOfNodes = numberOfTriangles;
        while (currNumberOfNodes > 1)
        {
            const aabb & newBV = find_merge_candidates(positions, leafs, currNumberOfNodes, dest, src);

            bvh_node * newNode = create_node(newBV);
            newNode->mLeft = leafs[dest];
            newNode->mRight = leafs[src];

            if (dest > src)
                std::swap(dest, src);
            
            leafs[dest] = newNode;
            --currNumberOfNodes;
            leafs[src] = leafs[currNumberOfNodes];
        }

        // Set the root
        mRoot = leafs[0];
    }


    /**
    * @brief Returns the information of what method was used to construct the tree.
    */
    bvh_construction_method bvh_tree::get_build_method() const
    {
        return mBuildMethod;
    }


    /**
    * @brief Returns the root node pointer.
    */
    bvh_node * bvh_tree::get_root() const
    {
        return mRoot;
    }


    /**
    * @brief Returns false if the tree has at least one node, true otherwise.
    */
    bool bvh_tree::is_empty() const
    {
        return mRoot == nullptr;
    }


    /**
    * @brief Recursively frees all the nodes of the
    *        tree passed as parameter.
    * @param tree   The tree to destroy.
    */
    void bvh_tree::destroy_tree(bvh_node * tree)
    {
        // Base case, the current node is null
        if (tree == nullptr)
            return;

        // Post order traversal to free the nodes
        destroy_tree(tree->mLeft);
        destroy_tree(tree->mRight);
        free_node(tree);
    }


    /**
    * @brief Recursively frees all the nodes of the tree.
    */
    void bvh_tree::clear()
    {
        destroy_tree(mRoot);
        nodeCounter = 0;
    }


    /**
    * @brief Helper function that creates a new node with the type passed as parameter.
    * @param bv          The bounding volume of the node to create.
    * @param node_type   The type of the node to create (defaults to internal).
    */
    bvh_node * bvh_tree::create_node(const aabb & bv, tree_node_type node_type)
    {
        bvh_node * newNode = new bvh_node;
        newNode->mBV = bv;
        newNode->mNodeType = node_type;
        newNode->mId = nodeCounter;
        ++nodeCounter;
        return newNode;
    }


    /**
    * @brief Helper function that frees the memory of the node passes as parameter.
    * @param to_free   The node to free.
    */
    void bvh_tree::free_node(bvh_node * to_free)
    {
        delete to_free;
        to_free = nullptr;
        --nodeCounter;
    }
}