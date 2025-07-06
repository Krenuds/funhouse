#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/InputContextManager.h"
#include <SDL2/SDL.h>

using namespace Funhouse;

class TestCommand : public InputCommand {
public:
    TestCommand(const std::string& name) : name_(name) {}
    
    void Execute() override {}
    
    std::string GetName() const override { return name_; }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TestCommand>(name_);
    }
    
private:
    std::string name_;
};

TEST_CASE("InputContextManager", "[input]") {
    SECTION("Add and retrieve contexts") {
        InputContextManager manager;
        
        auto ctx1 = std::make_shared<InputContext>("game");
        auto ctx2 = std::make_shared<InputContext>("menu");
        
        manager.AddContext(ctx1);
        manager.AddContext(ctx2);
        
        REQUIRE(manager.GetContext("game") == ctx1);
        REQUIRE(manager.GetContext("menu") == ctx2);
        REQUIRE(manager.GetContext("nonexistent") == nullptr);
    }
    
    SECTION("Remove context") {
        InputContextManager manager;
        
        auto ctx = std::make_shared<InputContext>("test");
        manager.AddContext(ctx);
        
        REQUIRE(manager.GetContext("test") == ctx);
        
        manager.RemoveContext("test");
        REQUIRE(manager.GetContext("test") == nullptr);
    }
    
    SECTION("Context activation") {
        InputContextManager manager;
        
        auto ctx = std::make_shared<InputContext>("test");
        ctx->SetActive(false);
        manager.AddContext(ctx);
        
        REQUIRE(!ctx->IsActive());
        
        manager.ActivateContext("test");
        REQUIRE(ctx->IsActive());
        
        manager.DeactivateContext("test");
        REQUIRE(!ctx->IsActive());
        
        manager.ToggleContext("test");
        REQUIRE(ctx->IsActive());
    }
    
    SECTION("Priority-based processing") {
        InputContextManager manager;
        
        // Lower priority context
        auto lowCtx = std::make_shared<InputContext>("low");
        lowCtx->SetPriority(10);
        lowCtx->BindKey(SDL_SCANCODE_A, []() {
            return std::make_unique<TestCommand>("low");
        });
        
        // Higher priority context
        auto highCtx = std::make_shared<InputContext>("high");
        highCtx->SetPriority(20);
        highCtx->BindKey(SDL_SCANCODE_A, []() {
            return std::make_unique<TestCommand>("high");
        });
        
        manager.AddContext(lowCtx);
        manager.AddContext(highCtx);
        
        // Higher priority should win
        auto cmd = manager.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd != nullptr);
        REQUIRE(cmd->GetName() == "high");
    }
    
    SECTION("Inactive contexts are skipped") {
        InputContextManager manager;
        
        auto activeCtx = std::make_shared<InputContext>("active");
        activeCtx->BindKey(SDL_SCANCODE_A, []() {
            return std::make_unique<TestCommand>("active");
        });
        
        auto inactiveCtx = std::make_shared<InputContext>("inactive");
        inactiveCtx->SetActive(false);
        inactiveCtx->SetPriority(100); // Higher priority but inactive
        inactiveCtx->BindKey(SDL_SCANCODE_A, []() {
            return std::make_unique<TestCommand>("inactive");
        });
        
        manager.AddContext(activeCtx);
        manager.AddContext(inactiveCtx);
        
        // Active context should win despite lower priority
        auto cmd = manager.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd != nullptr);
        REQUIRE(cmd->GetName() == "active");
    }
    
    SECTION("Get active contexts sorted by priority") {
        InputContextManager manager;
        
        auto ctx1 = std::make_shared<InputContext>("ctx1");
        ctx1->SetPriority(10);
        
        auto ctx2 = std::make_shared<InputContext>("ctx2");
        ctx2->SetPriority(30);
        
        auto ctx3 = std::make_shared<InputContext>("ctx3");
        ctx3->SetPriority(20);
        ctx3->SetActive(false);
        
        manager.AddContext(ctx1);
        manager.AddContext(ctx2);
        manager.AddContext(ctx3);
        
        auto active = manager.GetActiveContexts();
        REQUIRE(active.size() == 2);
        REQUIRE(active[0] == ctx2); // Highest priority
        REQUIRE(active[1] == ctx1); // Lower priority
    }
    
    SECTION("Process all input types") {
        InputContextManager manager;
        auto ctx = std::make_shared<InputContext>("test");
        
        bool keyPressed = false;
        bool mousePressed = false;
        bool wheelScrolled = false;
        bool mouseMoved = false;
        
        ctx->BindKey(SDL_SCANCODE_A, [&keyPressed]() {
            keyPressed = true;
            return std::make_unique<TestCommand>("key");
        });
        
        ctx->BindMouseButton(SDL_BUTTON_LEFT, [&mousePressed]() {
            mousePressed = true;
            return std::make_unique<TestCommand>("mouse");
        });
        
        ctx->BindMouseWheel(
            [&wheelScrolled]() {
                wheelScrolled = true;
                return std::make_unique<TestCommand>("wheel");
            },
            nullptr
        );
        
        ctx->BindMouseMotion([&mouseMoved]() {
            mouseMoved = true;
            return std::make_unique<TestCommand>("motion");
        });
        
        manager.AddContext(ctx);
        
        manager.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(keyPressed);
        
        manager.ProcessMouseButtonDown(SDL_BUTTON_LEFT, KMOD_NONE);
        REQUIRE(mousePressed);
        
        manager.ProcessMouseWheel(1);
        REQUIRE(wheelScrolled);
        
        manager.ProcessMouseMotion();
        REQUIRE(mouseMoved);
    }
}