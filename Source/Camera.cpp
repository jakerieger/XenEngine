// Author: Jake Rieger
// Created: 11/25/2024.
//

#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Xen {
    /*******************************************
     *                                         *
     *               OrthoCamera               *
     *                                         *
     *******************************************/

    OrthoCamera::OrthoCamera(f32 width, f32 height, f32 near, f32 far)
        : mView(glm::mat4(1.0f)), mProjection(glm::mat4(1.0f)), mPosition({0.f, 0.f, 0.f}),
          mLeft(0), mRight(0), mBottom(0), mTop(0), mNear(near), mFar(far), mZoom(1.f) {
        WHToLRTB(width, height, mLeft, mRight, mBottom, mTop);
        Update(0.f);
    }

    glm::mat4 OrthoCamera::GetViewProjection() const {
        return mProjection * mView;
    }

    void OrthoCamera::Update(f32) {
        UpdateView();
        UpdateProjection();
    }

    void OrthoCamera::SetPosition(const glm::vec3& position) {
        mPosition = position;
        UpdateView();
    }

    void OrthoCamera::SetZoom(f32 zoom) {
        mZoom = zoom;
        UpdateProjection();
    }

    void OrthoCamera::SetBounds(f32 left, f32 right, f32 bottom, f32 top) {
        mLeft   = left;
        mRight  = right;
        mBottom = bottom;
        mTop    = top;
        UpdateProjection();
    }

    void OrthoCamera::SetZBounds(f32 near, f32 far) {
        mNear = near;
        mFar  = far;
        UpdateProjection();
    }

    void OrthoCamera::ResizeViewport(f32 width, f32 height) {
        WHToLRTB(width, height, mLeft, mRight, mBottom, mTop);
        UpdateProjection();
    }

    glm::vec3 OrthoCamera::ScreenToWorld(const glm::vec2& screenPos,
                                         const glm::vec2& screenSize) const {
        // Get our normalized device coordinates for our screen resolution
        const glm::vec4 ndc((screenPos.x / screenSize.x) * 2.0f - 1.0f,
                            (screenPos.y / screenSize.y) * 2.0f - 1.0f,
                            0.0f,
                            1.0f);
        const glm::vec4 world = glm::inverse(GetViewProjection()) * ndc;
        return glm::vec3(world.x, world.y, world.z) / world.w;
    }

    glm::vec2 OrthoCamera::WorldToScreen(const glm::vec3& worldPos,
                                         const glm::vec2& screenSize) const {
        glm::vec4 screen = GetViewProjection() * glm::vec4(worldPos, 1.f);
        screen /= screen.w;
        return {(screen.x + 1.0f) * 0.5f * screenSize.x, (screen.y + 1.0f) * 0.5f * screenSize.y};
    }

    void OrthoCamera::UpdateView() {
        mView = glm::translate(glm::mat4(1.f), -mPosition);
    }

    void OrthoCamera::UpdateProjection() {
        mProjection =
          glm::ortho(mLeft * mZoom, mRight * mZoom, mBottom * mZoom, mTop * mZoom, mNear, mFar);
    }

    void
    OrthoCamera::WHToLRTB(f32 width, f32 height, f32& left, f32& right, f32& bottom, f32& top) {
        left   = 1.f - (width / 2.f);
        right  = height / 2.f;
        bottom = width / 2.f;
        top    = 1.f - (height / 2.f);
    }
}  // namespace Xen