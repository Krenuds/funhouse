#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/InputContext.h"
#include <SDL2/SDL.h>

using namespace Funhouse;

class CountingCommand : public InputCommand {
public:
    CountingCommand(int& counter, int increment = 1) 
        : counter_(counter), increment_(increment) {}
    
    void Execute() override {
        counter_ += increment_;
    }
    
    std::string GetName() const override {
        return "CountingCommand";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<CountingCommand>(counter_, increment_);
    }
    
private:
    int& counter_;
    int increment_;
};

TEST_CASE("InputContext", "[input]") {
    SECTION("Basic key binding") {
        InputContext context("test");
        int counter = 0;
        
        context.BindKey(SDL_SCANCODE_A, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        
        auto cmd = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd != nullptr);
        cmd->Execute();
        REQUIRE(counter == 1);
        
        // Unbound key should return nullptr
        auto nullCmd = context.ProcessKeyDown(SDL_SCANCODE_B, KMOD_NONE);
        REQUIRE(nullCmd == nullptr);
    }
    
    SECTION("Key binding with modifiers") {
        InputContext context("test");
        int counter = 0;
        
        // Bind Ctrl+A
        context.BindKey(SDL_SCANCODE_A, [&counter]() {
            return std::make_unique<CountingCommand>(counter, 10);
        }, KMOD_CTRL);
        
        // Regular A should not trigger
        auto cmd1 = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd1 == nullptr);
        
        // Ctrl+A should trigger
        auto cmd2 = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_CTRL);
        REQUIRE(cmd2 != nullptr);
        cmd2->Execute();
        REQUIRE(counter == 10);
    }
    
    SECTION("Mouse button binding") {
        InputContext context("test");
        int counter = 0;
        
        context.BindMouseButton(SDL_BUTTON_LEFT, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        
        auto cmd = context.ProcessMouseButtonDown(SDL_BUTTON_LEFT, KMOD_NONE);
        REQUIRE(cmd != nullptr);
        cmd->Execute();
        REQUIRE(counter == 1);
    }
    
    SECTION("Mouse wheel binding") {
        InputContext context("test");
        int upCount = 0;
        int downCount = 0;
        
        context.BindMouseWheel(
            [&upCount]() { return std::make_unique<CountingCommand>(upCount); },
            [&downCount]() { return std::make_unique<CountingCommand>(downCount); }
        );
        
        auto upCmd = context.ProcessMouseWheel(1);
        REQUIRE(upCmd != nullptr);
        upCmd->Execute();
        REQUIRE(upCount == 1);
        REQUIRE(downCount == 0);
        
        auto downCmd = context.ProcessMouseWheel(-1);
        REQUIRE(downCmd != nullptr);
        downCmd->Execute();
        REQUIRE(upCount == 1);
        REQUIRE(downCount == 1);
    }
    
    SECTION("Context activation") {
        InputContext context("test");
        int counter = 0;
        
        context.BindKey(SDL_SCANCODE_A, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        
        // Active by default
        REQUIRE(context.IsActive());
        auto cmd1 = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd1 != nullptr);
        
        // Deactivate
        context.SetActive(false);
        REQUIRE(!context.IsActive());
        auto cmd2 = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd2 == nullptr);
    }
    
    SECTION("Unbinding") {
        InputContext context("test");
        int counter = 0;
        
        context.BindKey(SDL_SCANCODE_A, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        
        // Should work initially
        auto cmd1 = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd1 != nullptr);
        
        // Unbind
        context.UnbindKey(SDL_SCANCODE_A);
        
        // Should no longer work
        auto cmd2 = context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE);
        REQUIRE(cmd2 == nullptr);
    }
    
    SECTION("Clear all bindings") {
        InputContext context("test");
        int counter = 0;
        
        // Add multiple bindings
        context.BindKey(SDL_SCANCODE_A, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        context.BindKey(SDL_SCANCODE_B, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        context.BindMouseButton(SDL_BUTTON_LEFT, [&counter]() {
            return std::make_unique<CountingCommand>(counter);
        });
        
        // Clear all
        context.ClearBindings();
        
        // None should work
        REQUIRE(context.ProcessKeyDown(SDL_SCANCODE_A, KMOD_NONE) == nullptr);
        REQUIRE(context.ProcessKeyDown(SDL_SCANCODE_B, KMOD_NONE) == nullptr);
        REQUIRE(context.ProcessMouseButtonDown(SDL_BUTTON_LEFT, KMOD_NONE) == nullptr);
    }
    
    SECTION("Context priority") {
        InputContext context("test");
        
        REQUIRE(context.GetPriority() == 0);
        
        context.SetPriority(100);
        REQUIRE(context.GetPriority() == 100);
    }
}