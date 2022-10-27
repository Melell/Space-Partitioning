#include "pch.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "imgui.hpp"
#include "demo_octree.hpp"

namespace cs350 {

    namespace {

        /**
         * @brief
		 *  Renders a mesh using the plain color shader. (specific to this demo)		 
		 * @param mesh
		 * @param m2w
		 * @param color
		 */
        void debug_draw_phong(mesh * mesh, const glm::mat4& m2w, glm::vec4 const& color)
        {
            // Objects
            //auto& inst   = renderer::instance();
            debug_draw_plain_color(mesh, m2w, color);
            /*auto& shd    = inst.resources().shaders.phong;
            auto& camera = inst.camera();

            // Parameters
            auto const& v              = camera.get_view_mtx();
            auto const& p              = camera.get_proj_mtx();
            auto        mv             = v * m2w;
            auto        mvp            = p * mv;
            auto        normal_matrix  = glm::mat3(glm::transpose(glm::inverse(mv)));
            auto        view_pos       = glm::vec3(500, 500, 100);
            auto        view_light_pos = v * glm::vec4(view_pos, 1);

            // OpenGL set
            shd->Use();
            shd->SetUniform("uniform_mvp", mvp);                                  // Note: expensive
            shd->SetUniform("uniform_color", color);                              // Note: expensive
            shd->SetUniform("uniform_mv", mv);                                    // Note: expensive
            shd->SetUniform("uniform_normal_matrix", normal_matrix);              // Note: expensive
            shd->SetUniform("uniform_view_light_pos", glm::vec3(view_light_pos)); // Note: expensive

            // Draw
            mesh->bind();
            glDrawElements(GL_TRIANGLES, mesh->indices_number(), GL_UNSIGNED_INT, nullptr);*/
        }

        /**
		 * @param window
		 * @param key
		 * @param scancode
		 * @param action
		 * @param mods
		 */
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            // ImGui has priority
            if (ImGui::GetIO().WantCaptureKeyboard) {
                return;
            }

            auto demo = reinterpret_cast<demo_octree*>(glfwGetWindowUserPointer(renderer::instance().get_window().handle()));
            if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
                demo->options().physics_enabled ^= 1u;
            }
            if (key == 'R' && action == GLFW_PRESS) {
                demo->destroy();
            }
        }

        /**
		 *
		 * @param window
		 * @param button
		 * @param action
		 * @param mods
		 */
        void mouse_callback(GLFWwindow* window, int button, int action, int mods)
        {
            // ImGui has priority
            if (ImGui::GetIO().WantCaptureMouse) {
                return;
            }

            auto demo = reinterpret_cast<demo_octree*>(glfwGetWindowUserPointer(renderer::instance().get_window().handle()));

            // Shoot on click
            if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
                demo->shoot(glm::linearRand(1.0f, 5.0f));
            }
            if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
                demo->shoot(0.0f);
            }
        }
    }

    /**
	 * @brief
	 */
    demo_octree::~demo_octree()
    {
        destroy();
    }

    /**
	 *
	 */
    void demo_octree::create()
    {
        { // Window/GFX
            renderer::instance().create(1280, 720, "CS350", false);

            // ImGui
            /*IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(renderer::instance().window().handle(), true);
            ImGui_ImplOpenGL3_Init("#version 150");*/

            // Input
            glfwSetWindowUserPointer(renderer::instance().get_window().handle(), this);
            glfwSetKeyCallback(renderer::instance().get_window().handle(), key_callback);
            glfwSetMouseButtonCallback(renderer::instance().get_window().handle(), mouse_callback);
        }

        renderer::instance().get_camera().set_position({0.0f, 0.0f, 64.0f});
        renderer::instance().get_camera().set_target({0.0f, 0.0f, 0.0f});
        renderer::instance().get_camera().set_projection(45, {1280, 720}, 0.1f, 1000.0f);

        // Octree
        // Set the initial root size and levels
        m_octree_dynamic.set_root_size(1u << m_options.octree_size_bit);
        m_octree_dynamic.set_levels(m_options.octree_levels);
    }

    /**
	 *
	 * @return true Everything is ok
	 * @return false Window closed
	 */
    bool demo_octree::update()
    {
        auto& window = renderer::instance().get_window();

        if (!window.update()) {
            return false;
        }

        // Frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glDepthMask(GL_TRUE);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, window.size().x, window.size().y);

        float dt = ImGui::GetIO().DeltaTime;

        // Camera update
        update_camera(dt);

        // Physics update
        for (auto* obj : m_dynamic_objects) {
            if (m_options.physics_enabled) {
                // Phy
                obj->position = obj->position + obj->velocity * dt;
            }

            // Clamping and bouncing on walls
            float boundary = m_octree_dynamic.root_size() * 0.5f;
            boundary -= 5.0f; // Make them bounce before boundary
            for (int i = 0; i < 3; ++i) {
                if (obj->position[i] > boundary && obj->velocity[i] > 0) {
                    obj->position[i] = boundary;
                    obj->velocity[i] *= -1;
                }
                if (obj->position[i] < -boundary && obj->velocity[i] < 0) {
                    obj->position[i] = -boundary;
                    obj->velocity[i] *= -1;
                }
            }

            // BV (Compute BV)
            obj->bv_world.mMinPos = obj->position - glm::vec3(obj->radius);
            obj->bv_world.mMaxPos = obj->position + glm::vec3(obj->radius);

            // Debug draw BV
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            debug_draw_aabb(obj->bv_world, {1,1,1,0.5f}, debug_draw_type::wireframe);

            {   // OCTREE UPDATE

                uint32_t currentCode = compute_locational_code(obj->bv_world, m_octree_dynamic.root_size(), m_octree_dynamic.levels());

                // If the current object doesn't belong to any octree node, add it
                if (obj->octree_node == nullptr)
                {
                    obj->octree_node = m_octree_dynamic.create_node(currentCode);
                    obj->octree_node->push_front(obj);
                }

                // If the current object has a different locational code than the one it used to have (it moved), update it
                if (obj->octree_node != nullptr &&
                    obj->octree_node->locational_code != currentCode)
                {
                    // Save the node the current obj belongs to
                    octree<physics_object>::node* oldNode = obj->octree_node;

                    // Remove it from the object linked list of the node it currently belongs to
                    obj->octree_node->remove(obj);

                    // If the node obj used to belong to has now 0 objs, and 0 child nodes active, then we can delete it
                    if (oldNode->first == nullptr && oldNode->children_active == 0)
                        m_octree_dynamic.delete_node_rec(oldNode->locational_code);

                    // Add it to the existing node it moved to, or to a newly created one if it doesn't exist
                    obj->octree_node = m_octree_dynamic.create_node(currentCode);
                    obj->octree_node->push_front(obj);
                }
            }
        }

        // Render each object
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for (auto* obj : m_dynamic_objects) {
            auto  m2w   = glm::translate(obj->position) * glm::scale(glm::vec3(obj->radius));
            auto& mesh  = renderer::instance().resources().meshes.sphere;
            auto  color = glm::vec4(0.2f, 0.0f, 0.8f, 1.0f);

            // If object in the levels specified to highlight, draw it with a green color
            if (m_options.highlight_level != -1 &&
                obj->octree_node &&
                locational_code_depth(obj->octree_node->locational_code) == m_options.highlight_level) {
                color = {0, 1, 0, 1};
            }
            // Actually uses the previous shader
            debug_draw_phong(&mesh, m2w, color);
        }

        // DEBUG DRAW of the specified levels' nodes
        if (m_options.debug_octree) {
            m_octree_dynamic.debug_draw_levels(m_options.highlight_level);
        }

        { // All pairs debug
            if (m_options.brute_force) {
                for (size_t i = 0; i < m_dynamic_objects.size(); ++i) {
                    for (size_t j = i + 1; j < m_dynamic_objects.size(); ++j) {
                        check_intersection(m_dynamic_objects[i], m_dynamic_objects[j]);
                    }
                }
            } else {
              // Octree all pairs (top-down or bottom up)

              // TOP-DOWN

              // Function to test all pairs collisions between the objects in nodeToTest
              auto all_pairs_test = [this](octree<physics_object>::node * nodeToTest)
              {
                  physics_object * traverser = nodeToTest->first;
                  // For each object in the current node
                  while (traverser != nullptr)
                  {
                      // For every other object in the current node
                      physics_object * traverserNext = traverser->octree_next_object;
                      while (traverserNext != nullptr)
                      {
                          // Perform intersection test (debug)
                          check_intersection(traverser, traverserNext);
                          traverserNext = traverserNext->octree_next_object;
                      }
                      traverser = traverser->octree_next_object;
                  }
              };

              const auto & map = m_octree_dynamic.get_map();

              // For each existing node, do all pairs test and then check intersection with child nodes
              for (auto it = map.begin(); it != map.end(); ++it)
              {
                all_pairs_test(it->second);
                top_down_collision_testing(it->second, it->second);
              }
            }
        }

        // Help
        if (ImGui::Begin("Help")) {
            ImGui::Text("Help: \n"
                        "\tLook:      Right click + mouse\n"
                        "\tMove:      WASD\n"
                        "\tMove fast: SHIFT+WASD\n"
                        "\tPause:     SPACE\n"
                        "\tRecreate:  r");

            if (ImGui::SliderInt("Octree size", &m_options.octree_size_bit, 1, 32)) {
                if (m_options.octree_levels > m_options.octree_size_bit)
                {
                    m_options.octree_levels = m_options.octree_size_bit;
                    m_octree_dynamic.set_levels(m_options.octree_levels);
                }

                // If octree size changes, orphan everything (forces reinsertion)
                m_octree_dynamic.destroy();
                m_octree_dynamic.set_root_size(1u << m_options.octree_size_bit);
                for (auto obj : m_dynamic_objects) {
                    obj->octree_node        = nullptr;
                    obj->octree_next_object = nullptr;
                    obj->octree_prev_object = nullptr;
                }
            }

            if (m_options.octree_levels > m_options.octree_size_bit)
            {
                m_options.octree_levels = m_options.octree_size_bit;
                m_octree_dynamic.set_levels(m_options.octree_levels);
            }

            if (ImGui::SliderInt("Octree levels", &m_options.octree_levels, 1, glm::min(10, m_options.octree_size_bit))) {
                // If octree max levels changes, orphan everything (forces reinsertion)
                m_octree_dynamic.destroy();
                m_octree_dynamic.set_levels(m_options.octree_levels);
                for (auto obj : m_dynamic_objects) {
                    obj->octree_node        = nullptr;
                    obj->octree_next_object = nullptr;
                    obj->octree_prev_object = nullptr;
                }
            }

            ImGui::SliderInt("Highlight level", &m_options.highlight_level, -1, m_options.octree_levels);

            ImGui::Checkbox("Octree debug render", &m_options.debug_octree);
            ImGui::Checkbox("Pair debug render", &m_options.debug_intersections);
            ImGui::Checkbox("Physics enabled", &m_options.physics_enabled);
            ImGui::Checkbox("Brute force", &m_options.brute_force);
            if (ImGui::Button("Random")) {
                for (int i = 0; i < 10; ++i) {
                    float boundary = m_octree_dynamic.root_size();
                    boundary -= 5.0f; // Make them not bounce outside
                    auto  p = glm::linearRand(glm::vec3(boundary) * -0.5f, glm::vec3(boundary) * 0.5f);
                    auto  v = glm::ballRand(glm::linearRand(1.0f, 5.0f));
                    float r = glm::linearRand(0.5f, 2.0f);

                    // Create
                    auto obj      = new physics_object;
                    obj->position = p;
                    obj->velocity = v;
                    obj->radius   = r;
                    m_dynamic_objects.push_back(obj);
                }
            }

            // Keep track of checks
            m_options.checks_history.push_back((float)m_options.checks_this_frame);
            if (m_options.checks_history.size() > 500) {
                m_options.checks_history.erase(m_options.checks_history.begin());
            }
            m_options.checks_this_frame = 0;

            ImGui::Text("Objects: %d", int(m_dynamic_objects.size()));
            ImGui::Text("Intersection checks: %d", int(m_options.checks_history.back()));
            ImGui::PlotLines("", m_options.checks_history.data(), m_options.checks_history.size(), 0, "", 0, FLT_MAX, ImVec2(0, 64));
            ImGui::Text("Max: %d", static_cast<int>(*std::max_element(m_options.checks_history.begin(), m_options.checks_history.end())));
        }
        ImGui::End();

        // Frame end
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window.handle());

        return true;
    }

    /**
	 * @brief
	 */
    void demo_octree::destroy()
    {
        // Delete the memory of the created objects
        for (auto it = m_dynamic_objects.begin(); it != m_dynamic_objects.end(); ++it)
        {
            delete *it;
            *it = nullptr;
        }

        m_dynamic_objects.clear();
        m_octree_dynamic.destroy();
    }

    /**
	 * @brief
	 */
    void demo_octree::shoot(float v)
    {
        auto const& camera          = renderer::instance().get_camera();
        auto        camera_dir      = camera.get_target() - camera.get_position();
        auto        camera_position = camera.get_position();

        auto obj = new physics_object;

        obj->position = camera_position;
        obj->velocity = camera_dir * v;
        obj->radius   = glm::linearRand(0.5f, 2.0f);

        m_dynamic_objects.push_back(obj);
    }

    /**
	 * @brief
     *  Placeholder intersection test, only draws and keeps track of statistics
	 * @param a
	 * @param b
	 */
    void demo_octree::check_intersection(physics_object const* a, physics_object const* b)
    {
        if (m_options.debug_intersections) {
            if (m_options.highlight_level == -1 ||
                (b->octree_node && locational_code_depth(b->octree_node->locational_code) == m_options.highlight_level) ||
                (a->octree_node && locational_code_depth(a->octree_node->locational_code) == m_options.highlight_level)) {
                glDisable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(GL_FALSE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                debug_draw_segment({a->position + glm::vec3{0, a->radius, 0}, b->position}, {1, 0, 1, 0.5f});
            }
        }
        m_options.checks_this_frame++;
    }

    /**
     * @brief
     *  Provided camera controls
     * @param dt 
     */
    void demo_octree::update_camera(float dt)
    {
        auto& camera          = renderer::instance().get_camera();
        auto& window          = renderer::instance().get_window();
        auto* glfwWindow      = window.handle();
        auto  camera_dir      = camera.get_target() - camera.get_position();
        auto  camera_position = camera.get_position();

        // Cursor
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        glfwGetCursorPos(glfwWindow, &cursor_x, &cursor_y);
        { // Input
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_2)) {
                float speed = 10.0f;
                auto  side  = glm::normalize(glm::cross(camera_dir, {0, 1, 0}));

                if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT)) {
                    speed *= 4.0f;
                }
                // Move
                if (glfwGetKey(glfwWindow, GLFW_KEY_W)) {
                    camera_position += glm::normalize(camera_dir) * dt * speed;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_S)) {
                    camera_position -= glm::normalize(camera_dir) * dt * speed;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_A)) {
                    camera_position -= glm::normalize(side) * dt * speed;
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_D)) {
                    camera_position += glm::normalize(side) * dt * speed;
                }

                // View
                glm::vec2 cursor_delta = {(float)cursor_x - m_cursor_pos.x, (float)cursor_y - m_cursor_pos.y};
                camera_dir             = glm::vec3(glm::vec4(camera_dir, 0) * glm::rotate(glm::radians(15.0f) * 0.01f * cursor_delta.y, side));
                camera_dir             = glm::vec3(glm::vec4(camera_dir, 0) * glm::rotate(glm::radians(15.0f) * 0.01f * cursor_delta.x, glm::vec3(0, 1, 0)));
            }
            m_cursor_pos       = {(float)cursor_x, (float)cursor_y};
            auto camera_target = camera_position + camera_dir;

            // Submit changes
            camera.set_position(camera_position);
            camera.set_target(camera_target);
            camera.set_projection(60.0f, window.size(), 0.01f, 10000.0f);
            camera.update_all_mtx();
        }
    }

    
    /**
    * @brief Recursive function to check collision between a node its children nodes it overlaps with.
    * @param start      The pointer to the current node whose childs we are to test against (with original).
    *                   (This goes down the hierarchy as recursion continues)
    * @param original   The original node before starting recursion, to check with it against each sub-child.
    */
    void demo_octree::top_down_collision_testing(octree<physics_object>::node * start, octree<physics_object>::node * original)
    {
      const int dimension = 3;
      uint32_t maxChilds = 1u << dimension;

      // For each child node of start
      for (uint32_t i = 0; i < maxChilds; ++i)
      {
        // If the current child node exists
        uint8_t childExists = start->children_active & (1u << i);
        if (childExists)
        {
          // Get the code, node and bounding volume of the current child
          uint32_t childCode = (start->locational_code << dimension) + i;
          octree<physics_object>::node * childNode = m_octree_dynamic.find_node(childCode);
          const aabb & childBV = compute_bv(childCode, m_octree_dynamic.root_size());

          // For each object in the original node
          physics_object * traverser = original->first;
          while (traverser != nullptr)
          {
            // If bv of the current object in start, intersects the bv of the current child node
            if (intersection_aabb_aabb(traverser->bv_world, childBV))
            {
              // For each object in the current child node
              physics_object * childTraverser = childNode->first;
              while (childTraverser != nullptr)
              {
                check_intersection(traverser, childTraverser);
                childTraverser = childTraverser->octree_next_object;
              }
            }

            traverser = traverser->octree_next_object;
          }

          // Recurse into the current child
          top_down_collision_testing(childNode, original);
        }
      }
    }
}
