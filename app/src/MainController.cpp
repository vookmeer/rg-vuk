#include <app/MainController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string_view>

namespace app {
    void MainController::initialize() {
        engine::graphics::OpenGL::enable_depth_testing();

        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto cam      = graphics->camera();
        cam->Position = glm::vec3(0.0f, 3.0f, 12.0f);
        cam->Yaw      = -90.0f;
        cam->Pitch    = -10.0f;

        m_cursor_enabled = false;
        platform->set_enable_cursor(false);

        auto resources     = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto skybox_shader = resources->shader("skybox");
        skybox_shader->use();
        skybox_shader->set_int("skybox", 0);
    }

    bool MainController::loop() {
        const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() ==
            engine::platform::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void MainController::poll_events() {
        const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_F1).state() ==
            engine::platform::Key::State::JustPressed) {
            m_cursor_enabled = !m_cursor_enabled;
            platform->set_enable_cursor(m_cursor_enabled);
            m_mouse_initialized = false;
        }

        if (platform->key(engine::platform::KeyId::KEY_L).state() ==
            engine::platform::Key::State::JustPressed) {
            m_warm_light = !m_warm_light;
            if (m_warm_light) {
                m_point_light.diffuse  = glm::vec3(1.0f, 0.7f, 0.4f);
                m_point_light.specular = glm::vec3(1.0f, 0.7f, 0.4f);
            } else {
                m_point_light.diffuse  = glm::vec3(0.6f, 0.7f, 1.0f);
                m_point_light.specular = glm::vec3(0.6f, 0.7f, 1.0f);
            }
        }

        if (platform->key(engine::platform::KeyId::KEY_EQUAL).state() ==
            engine::platform::Key::State::Pressed) {
            m_dir_light.diffuse = glm::clamp(m_dir_light.diffuse + glm::vec3(0.01f), glm::vec3(0.0f), glm::vec3(1.5f));
        }
        if (platform->key(engine::platform::KeyId::KEY_MINUS).state() ==
            engine::platform::Key::State::Pressed) {
            m_dir_light.diffuse = glm::clamp(m_dir_light.diffuse - glm::vec3(0.01f), glm::vec3(0.0f), glm::vec3(1.5f));
        }
    }

    void MainController::update() {
        update_camera();
    }

    void MainController::begin_draw() {
        engine::graphics::OpenGL::clear_buffers();
    }

    void MainController::draw() {
        draw_scene();

        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        graphics->draw_skybox(resources->shader("skybox"),
                              resources->skybox("skybox", "app/resources/skyboxes/skybox"));
    }

    void MainController::end_draw() {
        engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
    }

    void MainController::draw_scene() {
        auto graphics  = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto shader    = resources->shader("basic");

        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());

        shader->set_vec3("viewPos", graphics->camera()->Position);
        shader->set_float("shininess", m_shininess);

        shader->set_vec3("dirLight.direction", m_dir_light.direction);
        shader->set_vec3("dirLight.ambient", m_dir_light.ambient);
        shader->set_vec3("dirLight.diffuse", m_dir_light.diffuse);
        shader->set_vec3("dirLight.specular", m_dir_light.specular);

        shader->set_vec3("pointLight.position", m_point_light.position);
        shader->set_float("pointLight.constant", m_point_light.constant);
        shader->set_float("pointLight.linear", m_point_light.linear);
        shader->set_float("pointLight.quadratic", m_point_light.quadratic);
        shader->set_vec3("pointLight.ambient", m_point_light.ambient);
        shader->set_vec3("pointLight.diffuse", m_point_light.diffuse);
        shader->set_vec3("pointLight.specular", m_point_light.specular);

        // Auto  - centralna pozicija, na podu
        {
            auto car = resources->model("car");
            glm::mat4 model_matrix(1.0f);
            model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, -5.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(0.03f));
            shader->set_mat4("model", model_matrix);
            car->draw(shader);
        }

        // Motocikl - pored auta
        {
            auto motorbike = resources->model("motorbike");
            glm::mat4 model_matrix(1.0f);
            model_matrix = glm::translate(model_matrix, glm::vec3(3.0f, -2.8f, -5.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(0.004f));
            shader->set_mat4("model", model_matrix);
            motorbike->draw(shader);
        }
        // Viseca lampa - iznad scene
        {
            auto hanginglight = resources->model("hanginglight");
            glm::mat4 model_matrix(1.0f);
            model_matrix = glm::translate(model_matrix, glm::vec3(1.5f, 8.0f, -5.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(6.0f));
            shader->set_mat4("model", model_matrix);
            hanginglight->draw(shader);
        }
    }

    void MainController::update_camera() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto camera   = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        float dt      = platform->dt();

        if (platform->key(engine::platform::KEY_W).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
        }
        if (platform->key(engine::platform::KEY_S).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
        }
        if (platform->key(engine::platform::KEY_A).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
        }
        if (platform->key(engine::platform::KEY_D).state() == engine::platform::Key::State::Pressed) {
            camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
        }

        auto mouse = platform->mouse();
        if (!m_cursor_enabled) {
            if (!m_mouse_initialized) {
                if (mouse.dx != 0.0f || mouse.dy != 0.0f) {
                    m_mouse_initialized = true; // preskoci prvi skok nakon "hvatanja" kursora
                }
            } else {
                camera->rotate_camera(mouse.dx, mouse.dy);
            }
        }
        camera->zoom(mouse.scroll);
    }
} // namespace app
