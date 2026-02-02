#include "Camera.h"

#include <cmath>
#include <iostream>

// Project the point in [-1, 1] screen space onto the arcball sphere
static qf32 screen_to_arcball(const vf2& p);

// ArcballCamera Camera Implementation
ArcballCamera::ArcballCamera(const vf3& eye, const vf3& center, const vf3& up, f32 fov, f32 aspect, f32 near, f32 far)
{
    field_of_view = fov;
    aspect_ratio = aspect;
    near_plane = near;
    far_plane = far;

    init(eye, center, up);
}

void ArcballCamera::init(const vf3& eye, const vf3& center, const vf3& up)
{
    const vf3 dir = center - eye;
    vf3 z = glm::normalize(dir);
    vf3 x = glm::normalize(glm::cross(z, glm::normalize(up)));
    vf3 y = glm::normalize(glm::cross(x, z));
    x = glm::normalize(glm::cross(z, y));

    center_translation = glm::inverse(glm::translate(center));
    translation        = glm::translate(vf3(0.0f, 0.0f, -glm::length(dir)));
    rotation           = glm::normalize(glm::quat_cast(glm::transpose(mf3x3(x, y, -z))));
    update_camera();
}

void ArcballCamera::rotate(vf2 prev_mouse, vf2 cur_mouse)
{
    // Clamp mouse positions
    cur_mouse  = glm::clamp(cur_mouse,  vf2{ -1.0f, -1.0f }, vf2{ 1.0f, 1.0f });
    prev_mouse = glm::clamp(prev_mouse, vf2{ -1.0f, -1.0f }, vf2{ 1.0f, 1.0f });
    const qf32 mouse_cur_ball  = screen_to_arcball(cur_mouse);
    const qf32 mouse_prev_ball = screen_to_arcball(prev_mouse);
    rotation = mouse_cur_ball * mouse_prev_ball * rotation;
    update_camera();
}

void ArcballCamera::pan(vf2 mouse_delta)
{
    const f32 zoom_amount = std::abs(translation[3][2]);
    vf4 motion = { mouse_delta.x * zoom_amount, mouse_delta.y * zoom_amount, 0.0f, 0.0f };

    // Compute the panning in the world space
    motion = inv_camera * motion;
    center_translation = glm::translate(vf3(motion)) * center_translation;
    update_camera();
}

void ArcballCamera::zoom(const f32 zoom_amount)
{
    if (m_orthographic)
    {
        m_ortho_size -= zoom_amount * 0.1f;
        m_ortho_size = glm::max(m_ortho_size, 0.1f);
    }
    else
    {
        const vf3 motion = { 0.0f, 0.0f, zoom_amount };
        translation = glm::translate(motion) * translation;
    }
    update_camera();
}

void ArcballCamera::translate(vf3 position)
{
    translation = glm::translate(position);
    update_camera();
}

const mf4x4& ArcballCamera::transform() const
{
    return camera;
}

const mf4x4& ArcballCamera::inv_transform() const
{
    return inv_camera;
}

vf3 ArcballCamera::eye() const
{
    return vf3{ inv_camera * vf4{0, 0, 0, 1} };
}

vf3 ArcballCamera::dir() const
{
    return glm::normalize(vf3{ inv_camera * vf4{0, 0, -1, 0} });
}

vf3 ArcballCamera::up() const
{
    return glm::normalize(vf3{ inv_camera * vf4{0, 1, 0, 0} });
}

vf3 ArcballCamera::center() const
{
    return -vf3{ center_translation[3][0], center_translation[3][1], center_translation[3][2] };
}

const mf4x4& ArcballCamera::projection() const
{
    return proj;
}

const mf4x4& ArcballCamera::inv_projection() const
{
    return inv_proj;
}

const mf4x4 ArcballCamera::proj_camera() const
{
    return proj * camera;
}

void ArcballCamera::update_camera()
{
    camera = translation * mat4_cast(rotation) * center_translation;
    inv_camera = glm::inverse(camera);

    update_projection(field_of_view, aspect_ratio, near_plane, far_plane);
}

void ArcballCamera::update_projection(f32 fov, f32 aspect, f32 near, f32 far)
{
    if (m_orthographic)
    {
        f32 half_height = m_ortho_size;
        f32 half_width = half_height * aspect;
        proj = glm::ortho(-half_width, half_width, -half_height, half_height, near, far);
    }
    else
    {
        proj = glm::perspective(glm::radians(fov), aspect, near, far);
    }
    inv_proj = glm::inverse(proj);
}

void Camera::set_orthographic(bool enabled, f32 size)
{
    m_orthographic = enabled;
    m_ortho_size = size;
    update_camera();
}

glm::quat screen_to_arcball(const vf2& p)
{
    const f32 dist = glm::dot(p, p);
    // If we're on/in the sphere return the point on it
    if (dist <= 1.0f)
    {
        return qf32(0.0f, p.x, p.y, std::sqrt(1.0f - dist));
    }
    else
    {
        // otherwise we project the point onto the sphere
        const vf2 proj = glm::normalize(p);
        return qf32(0.0f, proj.x, proj.y, 0.f);
    }
}



// Camera Implementation
Camera::Camera(const vf3& eye, const vf3& center, const vf3& up, f32 fov, f32 aspect, f32 near, f32 far)
{
    m_field_of_view = fov;
    m_aspect_ratio = aspect;
    m_near_plane = near;
    m_far_plane = far;

    init(eye, center, up);
}

void Camera::init(const vf3& eye, const vf3& center, const vf3& up)
{
    m_position = eye;
    m_forward = glm::normalize(center - eye);
    m_right = glm::normalize(glm::cross(m_forward, up));
    m_world_up = glm::normalize(up);

    m_yaw = glm::degrees(std::atan2(m_forward.z, m_forward.x));
    m_pitch = glm::degrees(std::asin(m_forward.y));

    update_camera();
}

void Camera::rotate(vf2 prev_mouse, vf2 curr_mouse)
{
    vf2 delta = curr_mouse - prev_mouse;

    const f32 sensitivity = 0.1f;
    m_yaw   -= delta.x * sensitivity;
    m_pitch += delta.y * sensitivity;

    m_pitch = glm::clamp(m_pitch, -89.9f, 89.9f);

    update_camera();
}

void Camera::pan(vf2 mouse_delta)
{
    f32 pan_speed = 0.002f   * glm::length(m_position - (m_position + m_forward));
    m_position -= m_right    * mouse_delta.x * pan_speed;
    m_position += m_world_up * mouse_delta.y * pan_speed;
    update_camera();
}

void Camera::translate(vf3 offset)
{
    m_position += offset;
    update_camera();
}

void Camera::zoom(const f32 zoom_amount)
{
    if (m_orthographic)
    {
        m_ortho_size -= zoom_amount * 0.1f;
        m_ortho_size = glm::max(m_ortho_size, 0.1f);
    }
    else
    {
        m_position += m_forward * zoom_amount;
    }
    update_camera();
}

void ArcballCamera::set_orthographic(bool enabled, f32 size)
{
    m_orthographic = enabled;
    m_ortho_size = size;
    update_camera();
}

const mf4x4& Camera::view()           const { return m_view; }
const mf4x4& Camera::inv_view()       const { return m_inv_view; }
const mf4x4& Camera::projection()     const { return m_proj; }
const mf4x4& Camera::inv_projection() const { return m_inv_proj; }
const mf4x4  Camera::proj_camera()    const { return m_proj * m_view; }

vf3 Camera::eye()     const { return m_position; }
vf3 Camera::front()   const { return m_forward; }
vf3 Camera::up()      const { return m_up; }
vf3 Camera::center()  const { return m_position + m_forward; }
vf3 Camera::right()   const { return m_right; }
f32 Camera::pitch()   const { return m_pitch; }
f32 Camera::yaw()     const { return m_yaw; }

void Camera::update_camera()
{
    // recalc forward/right/up from yaw/pitch
    vf3 f;
    f.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    f.y = sin(glm::radians(m_pitch));
    f.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(f);

    m_right = glm::normalize(glm::cross(m_forward, m_world_up));
    m_up    = glm::normalize(glm::cross(m_right, m_forward));

    m_view     = glm::lookAt(m_position, m_position + m_forward, m_up);
    m_inv_view = glm::inverse(m_view);

    update_projection(m_field_of_view, m_aspect_ratio, m_near_plane, m_far_plane);
}

void Camera::update_projection(f32 fov, f32 aspect, f32 near, f32 far)
{
    if (m_orthographic)
    {
        f32 half_height = m_ortho_size;
        f32 half_width  = half_height * aspect;
        m_proj = glm::ortho(-half_width, half_width, -half_height, half_height, near, far);
    }
    else
    {
        m_proj = glm::perspective(glm::radians(fov), aspect, near, far);
    }
    m_inv_proj = glm::inverse(m_proj);
}

