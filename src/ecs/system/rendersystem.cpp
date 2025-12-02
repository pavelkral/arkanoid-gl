#include "rendersystem.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void Ecs::RenderSystem::Init() {
    size_t maxInstances = 2000;
    vboInstance = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
    vboInstance->allocate(maxInstances * sizeof(glm::mat4));
    vboColor = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
    vboColor->allocate(maxInstances * sizeof(glm::vec4));
}

void Ecs::RenderSystem::SetupVAO(const VertexArray &vao) {
    vao.bind();
    vboInstance->bind();
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(1 + i, 1);
    }
    vboColor->bind();
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1);
    vao.unbind();
}

void Ecs::RenderSystem::Update(EntityManager &manager, Shader &shader) {
    shader.use();
    std::unordered_map<Mesh*, std::vector<Entity>> batch;
    for (auto& [e, rend] : manager.renderables) {
        if (!rend.visible || !rend.mesh) continue;
        batch[rend.mesh].push_back(e);
    }
    for (auto& [mesh, entities] : batch) {
        matrices.clear(); colors.clear();
        for (Entity e : entities) {
            auto* trans = manager.getEntityComponent<TransformComponent>(e);
            auto* rend = manager.getEntityComponent<RenderComponent>(e);
            if (trans && rend) {
                matrices.push_back(trans->getMatrix());
                colors.push_back(rend->color);
            }
        }
        if (!matrices.empty()) {
            vboInstance->setSubData(matrices);
            vboColor->setSubData(colors);
            mesh->drawInstanced((GLsizei)matrices.size());
        }
    }
}

void Ecs::RenderSystem::DrawUI(EntityManager &manager, Stats &stats, std::function<void ()> onRestart, std::function<void ()> onQuit) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    stats.drawUI();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("GameInfo", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Score: %d", manager.globalState.score);
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Lives: %d", manager.globalState.lives);
    ImGui::End();

    if (manager.globalState.gameOver) {
        if (!ImGui::IsPopupOpen("GameOver") && !ImGui::IsPopupOpen("GameWon")) {
            ImGui::OpenPopup(manager.globalState.gameWon ? "GameWon" : "GameOver");
        }
    }

    if (ImGui::BeginPopupModal("GameOver", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("GAME OVER");
        ImGui::Text("Final Score: %d", manager.globalState.score);
        ImGui::Separator();

        if (ImGui::Button("Restart Game", ImVec2(120, 0))) {
            onRestart();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Quit App", ImVec2(120, 0))) {
            onQuit();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("GameWon", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "CONGRATULATIONS!");
        ImGui::Text("You destroyed all bricks!");
        ImGui::Text("Final Score: %d", manager.globalState.score);
        ImGui::Separator();

        if (ImGui::Button("Play Again", ImVec2(120, 0))) {
            onRestart();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Quit App", ImVec2(120, 0))) {
            onQuit();
        }
        ImGui::EndPopup();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
