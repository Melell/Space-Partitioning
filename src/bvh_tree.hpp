/**
* @file binary_tree.hpp
* @author Miguel Echeverria , 540000918 , miguel.echeverria@digipen.edu
* @date 2020/21/10
* @brief Contains the declaration for the binary_tree structure, as well as
*        the definition of a node.
*
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#pragma once
#include "geometry.hpp"


namespace cs350
{
    struct mesh_data;


    enum class tree_node_type
    {
        internal,
        leaf
    };


    struct bvh_node
    {
        bvh_node();

        aabb mBV;
        int mId;
        tree_node_type mNodeType = tree_node_type::internal;
        std::vector<unsigned> mIndices;
        bvh_node * mLeft;
        bvh_node * mRight;
    };


    enum class bvh_construction_method
    {
        top_down,
        bottom_up,
        insertion,
        not_constructed
    };

    
    class bvh_tree
    {
    public:

        bvh_tree();
        ~bvh_tree();

        void construct_bvh(const mesh_data & vert_data, bvh_construction_method building_method);

        bvh_construction_method get_build_method() const;
        bvh_node * get_root() const;
        bool is_empty() const;

        void destroy_tree(bvh_node * tree);
        void clear();

    private:
        bvh_node * mRoot;
        bvh_construction_method mBuildMethod;

        int nodeCounter = 0;

        aabb compute_bv(const std::vector<glm::vec3> & positions, const std::vector<unsigned> & posIndices) const;

        void top_down_bvh(bvh_node ** tree, const std::vector<glm::vec3> & positions, const std::vector<unsigned> & posIndices);
        bool top_down_partition(bvh_node * nodeToDiv,
                                const std::vector<glm::vec3> & inPositions,
                                const std::vector<unsigned> & inIndices,
                                std::vector<unsigned> & outLeftIndices,
                                std::vector<unsigned> & outRightIndices);


        void bottom_up_bvh(const std::vector<glm::vec3> & positions, const std::vector<unsigned> & posIndices);
        aabb find_merge_candidates(const std::vector<glm::vec3> & positions, 
                                   const std::vector<bvh_node*> & nodes,
                                   int numberOfNodes,
                                   int & dest,
                                   int & src);


        bvh_node * create_node(const aabb & bv, tree_node_type node_type = tree_node_type::internal);
        void free_node(bvh_node * to_free);
    };
}