// Author: Jake Rieger
// Created: 11/14/2024.
//

#include <Types/Types.h>
#include <OpenGLTK/Include/Glfw/GlfwApp.h>

class Pong final : public GLTK::IGlfwApp {
public:
    Pong(int width, int height) : IGlfwApp("Pong", width, height) {}
    void CreateGLResources() override;
    void DestroyGLResources() override;
    void Update() override;
    void Render() override;
};

int main() {
    Pong app(1280, 720);
    app.Run();
}