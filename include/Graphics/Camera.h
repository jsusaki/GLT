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
    PerspectiveCamera(const vf3& eye, const vf3& center, const vf3& up, f32 fov, f32 aspect, f32 near, f32 far)
    {
        m_field_of_view = fov;
        m_aspect_ratio = aspect;
        m_near_plane = near;
        m_far_plane = far;

        init(eye, center, up);
    }

public:
    void init(const vf3& eye, const vf3& center, const vf3& up)
    {
        m_position = eye;
        m_forward = glm::normalize(center - eye);
        m_right = glm::normalize(glm::cross(m_forward, up));
        m_world_up = glm::normalize(up);

        m_yaw = glm::degrees(std::atan2(m_forward.z, m_forward.x));
        m_pitch = glm::degrees(std::asin(m_forward.y));

        update_camera();
    }

    void rotate(vf2 prev_mouse, vf2 cur_mouse)
    {
        vf2 delta = cur_mouse - prev_mouse;

        const f32 sensitivity = 0.2f;
        m_yaw += delta.x * sensitivity;
        m_pitch += delta.y * sensitivity;

        m_pitch = glm::clamp(m_pitch, -89.9f, 89.9f);

        update_camera();
    }

    void pan(vf2 mouse_delta)
    {
        f32 pan_speed = 0.002f * glm::length(m_position - (m_position + m_forward));
        m_position -= m_right * mouse_delta.x * pan_speed;
        m_position += m_world_up * mouse_delta.y * pan_speed;
        update_camera();
    }

    void zoom(const f32 zoom_amount)
    {
        m_position += m_forward * zoom_amount;
        update_camera();
    }

    void translate(vf3 offset)
    {
        m_position += offset;
        update_camera();
    }

    const mf4x4& view()           const { return m_view; }
    const mf4x4& inv_view()       const { return m_inv_view; }
    const mf4x4& projection()     const { return m_proj; }
    const mf4x4& inv_projection() const { return m_inv_proj; }
    const mf4x4  proj_camera()    const { return m_proj * m_view; }

    vf3 eye()     const { return m_position; }
    vf3 front() const { return m_forward; }
    vf3 up()      const { return m_up; }
    vf3 center()  const { return m_position + m_forward; }
    vf3 right()   const { return m_right; }

private:
    /*
    void update_camera()
    {
        // recalculate forward/right/up from yaw/pitch
        vf3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward = glm::normalize(f);

        right = glm::normalize(glm::cross(forward, world_up));
        up_vec = glm::normalize(glm::cross(right, forward));

        camera = glm::inverse(glm::lookAt(position, position + forward, up_vec));
        inv_camera = glm::inverse(camera);

        update_projection(field_of_view, aspect_ratio, near_plane, far_plane);
    }
    */

    void update_camera()
    {
        // recalc forward/right/up from yaw/pitch
        vf3 f;
        f.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        f.y = sin(glm::radians(m_pitch));
        f.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_forward = glm::normalize(f);

        m_right = glm::normalize(glm::cross(m_forward, m_world_up));
        m_up = glm::normalize(glm::cross(m_right, m_forward));

        m_view     = glm::lookAt(m_position, m_position + m_forward, m_up);
        m_inv_view = glm::inverse(m_view);

        update_projection(m_field_of_view, m_aspect_ratio, m_near_plane, m_far_plane);
    }

    void update_projection(f32 fov, f32 aspect, f32 near, f32 far)
    {
        m_proj = glm::perspective(glm::radians(fov), aspect, near, far);
        m_inv_proj = glm::inverse(m_proj);
    }

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
