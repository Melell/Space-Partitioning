#pragma once

#include "camera.hpp"
#include "window.hpp"
#include "octree.hpp"

namespace cs350 {
    /**
     * @brief
     *  Describes a basics physical object
     */
    struct physics_object
    {
        // Object data
        glm::vec3 position;
        float     radius;
        glm::vec3 velocity;
        aabb      bv_world;

        // Space partitioning data
        octree<physics_object>::node* octree_node{nullptr};
        physics_object*               octree_next_object{nullptr};
        physics_object*               octree_prev_object{nullptr};
    };

    /**
     * @brief
     *  Specific demo for this assignment
     */
    class demo_octree
    {
      private:
        // Others
        glm::vec2 m_cursor_pos = {};

        //
        octree<physics_object>       m_octree_dynamic;
        std::vector<physics_object*> m_dynamic_objects;

        // Imgui options
        struct
        {
            bool debug_octree{true};
            bool debug_intersections{true};
            bool physics_enabled{true};
            int  octree_size_bit{7};
            int  octree_levels{3};
            bool brute_force{false};
            int  highlight_level{-1};

            // Performance counters
            int                checks_this_frame{};
            std::vector<float> checks_history;
        } m_options;

      public:
        ~demo_octree();
        void create();
        bool update();
        void destroy();
        void shoot(float v);
        void check_intersection(physics_object const* a, physics_object const* b);
        void update_camera(float dt);

        // Recursive function to check collision between a node its children nodes it overlaps with
        void top_down_collision_testing(octree<physics_object>::node * start, octree<physics_object>::node * original);

        decltype(m_options)& options() { return m_options; }
    };
}
