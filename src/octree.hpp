#ifndef CS350_OCTREE_TEST_OCTREE_HPP
#define CS350_OCTREE_TEST_OCTREE_HPP

namespace cs350 {

    // Helper function to print number in binary
    void print_binary(uint32_t number);

    template <int dimension = 3>
    uint32_t compute_locational_code(glm::vec<dimension, int> world_position, uint32_t root_size, uint32_t levels);
    uint32_t compute_locational_code(aabb const& bv, uint32_t root_size, uint32_t levels);
    aabb     compute_bv(uint32_t locational_code, uint32_t root_size);
    uint32_t locational_code_depth(uint32_t lc);
    uint32_t common_locational_code(uint32_t lc1, uint32_t lc2);

    /**
     * @brief
     * 	Linear octree, each node stores a head for a linked list of T
     * @tparam T
     */
    template <typename T>
    class octree
    {
      public:
        struct node
        {
            uint32_t locational_code{0}; 
            uint8_t  children_active{0};
            T*       first{nullptr};

            void push_front(T * object);
            void remove(T * object);
        };

      private:
        std::unordered_map<uint32_t, node*> m_nodes;
        uint32_t                            m_root_size;
        uint32_t                            m_levels;

      public:
        octree();
        ~octree();
        void        destroy();
        node*       find_create_node(aabb const& bv);
        node*       find_node(aabb const& bv);
        node const* find_node(aabb const& bv) const;
        node*       find_create_node(uint32_t locational_code);
        node*       find_node(uint32_t locational_code);
        node const* find_node(uint32_t locational_code) const;
        node*       create_node(uint32_t locational_code);
        void        delete_node(uint32_t locational_code);
        void        delete_node_rec(uint32_t locational_code);
        void        debug_draw_levels(int highlight_level);

        const std::unordered_map<uint32_t, node*> & get_map() const { return m_nodes; }
        [[nodiscard]] uint32_t root_size() const { return m_root_size; }
        [[nodiscard]] uint32_t levels() const { return m_levels; }
        void                   set_root_size(uint32_t size) { m_root_size = size; }
        void                   set_levels(uint32_t levels) { m_levels = levels; }
    };
}

#include "octree.inl" 

#endif //CS350_OCTREE_TEST_OCTREE_HPP
