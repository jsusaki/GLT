// based on arcball-cpp: https://github.com/Twinklebear/arcball-cpp
#include "Core/Common.h"

class ArcballCamera
{
public:
    ArcballCamera() = default;
    ArcballCamera(const vf3& eye, const vf3& center, const vf3& up, f32 fov, f32 aspect, f32 near, f32 far);

public:
    void init(const vf3& eye, const vf3& center, const vf3& up);

    void rotate(vf2 prev_mouse, vf2 cur_mouse);
    void pan(vf2 mouse_delta);
    void zoom(const f32 zoom_amount);
    void translate(vf3 position);

    const mf4x4& transform()      const;
    const mf4x4& inv_transform()  const;
    const mf4x4& projection()     const;
    const mf4x4& inv_projection() const;
    const mf4x4  proj_camera()    const;

    vf3 eye()    const;
    vf3 dir()    const;
    vf3 up()     const;
    vf3 center() const;

private:
    void update_camera();
    void update_projection(f32 fov, f32 aspect, f32 near, f32 far);

private:
    mf4x4 center_translation;
    mf4x4 translation;
    qf32  rotation;

    mf4x4 camera;
    mf4x4 inv_camera;
    mf4x4 proj;
    mf4x4 inv_proj;

    f32 field_of_view;
    f32 aspect_ratio;
    f32 near_plane;
    f32 far_plane;
};


class PerspectiveCamera
{
public:
    PerspectiveCamera() = default;
    PerspectiveCamera(const vf3& eye, const vf3& center, const vf3& up, f32 fov, f32 aspect, f32 near, f32 far);

public:
    void init(const vf3& eye, const vf3& center, const vf3& up);
    void rotate(vf2 prev_mouse, vf2 cur_mouse);
    void pan(vf2 mouse_delta);
    void zoom(const f32 zoom_amount);
    void translate(vf3 offset);

    const mf4x4& view()           const;
    const mf4x4& inv_view()       const;
    const mf4x4& projection()     const;
    const mf4x4& inv_projection() const;
    const mf4x4  proj_camera()    const;
    vf3 eye()    const;
    vf3 front()  const;
    vf3 up()     const;
    vf3 center() const;
    vf3 right()  const;

private:
    /*
    void update_camera()
    {
        // recalculate forward/right/up from yaw/pitch
        vf3 f;
        f.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        f.y = sin(glm::radians(m_pitch));
        f.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        forward = glm::normalize(f);

        right = glm::normalize(glm::cross(m_forward, m_world_up));
        m_up = glm::normalize(glm::cross(m_right, m_forward));

        view = glm::inverse(glm::lookAt(m_position, m_position + m_forward, m_up));
        inv_view = glm::inverse(m_view);

        update_projection(m_field_of_view, m_aspect_ratio, m_near_plane, m_far_plane);
    }
    */

    void update_camera();
    void update_projection(f32 fov, f32 aspect, f32 near, f32 far);

private:
    // transform state
    vf3 m_position;
    vf3 m_forward;
    vf3 m_right;
    vf3 m_up;
    vf3 m_world_up = { 0, 1, 0 };

    f32 m_yaw   = -90.0f;
    f32 m_pitch = 0.0f;

    // camera matrices
    mf4x4 m_view;
    mf4x4 m_inv_view;
    mf4x4 m_proj;
    mf4x4 m_inv_proj;

    // projection params
    f32 m_field_of_view;
    f32 m_aspect_ratio;
    f32 m_near_plane;
    f32 m_far_plane;
};
