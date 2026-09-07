#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Engine.hpp>
#include <string_view>

namespace app {
    struct DirLightData {
        glm::vec3 direction{-0.3f, -1.0f, -0.2f};
        glm::vec3 ambient{0.15f, 0.15f, 0.15f};
        glm::vec3 diffuse{0.5f, 0.5f, 0.5f};
        glm::vec3 specular{0.4f, 0.4f, 0.4f};
    };

    struct PointLightData {
        glm::vec3 position{1.5f, 8.0f, -5.0f};
        float constant{1.0f};
        float linear{0.09f};
        float quadratic{0.032f};
        glm::vec3 ambient{0.1f, 0.1f, 0.1f};
        glm::vec3 diffuse{0.8f, 0.8f, 0.8f};
        glm::vec3 specular{1.0f, 1.0f, 1.0f};
    };

    class MainController final : public engine::core::Controller {
    public:
        std::string_view name() const override {
            return "app::MainController";
        }

    private:
        void initialize() override;

        bool loop() override;

        void poll_events() override;

        void update() override;

        void begin_draw() override;

        void draw() override;

        void end_draw() override;

        void draw_scene();

        void update_camera();

        bool m_cursor_enabled{true};
        bool m_mouse_initialized{false};

        DirLightData m_dir_light;
        PointLightData m_point_light;
        float m_shininess{32.0f};
        bool m_warm_light{true};
    };
} // namespace app
#endif//MAINCONTROLLER_HPP
