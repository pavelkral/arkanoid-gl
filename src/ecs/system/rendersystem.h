#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H


#include "../entity/entitymanager.h"
#include "../../utils/stats.h"
#include "../../gl/shader.h"


namespace Ecs {
class RenderSystem {
    std::unique_ptr<Buffer> vboInstance;
    std::unique_ptr<Buffer> vboColor;
    std::vector<glm::mat4> matrices;
    std::vector<glm::vec4> colors;
public:
    void Init();
    void SetupVAO(const VertexArray& vao);
    void Update(EntityManager& manager, Shader& shader);
    void DrawUI(EntityManager& manager, Stats& stats, std::function<void()> onRestart, std::function<void()> onQuit);
};
}

#endif // RENDERSYSTEM_H
