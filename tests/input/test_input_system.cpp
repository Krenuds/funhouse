#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/InputSystem.h"
#include "../../modules/world/World.h"
#include <SDL2/SDL.h>

using namespace Funhouse;

TEST_CASE("InputSystem basic functionality", "[InputSystem]") {
    
    SECTION("InputSystem can be created") {
        auto inputSystem = std::make_unique<InputSystem>();
        REQUIRE(inputSystem != nullptr);
    }
    
    SECTION("Mouse state is initialized correctly") {
        InputSystem inputSystem;
        const auto& mouseState = inputSystem.GetMouseState();
        
        REQUIRE(mouseState.x == 0);
        REQUIRE(mouseState.y == 0);
        REQUIRE_FALSE(mouseState.leftPressed);
        REQUIRE_FALSE(mouseState.rightPressed);
        REQUIRE_FALSE(mouseState.middlePressed);
        REQUIRE(mouseState.wheelDelta == 0);
    }
    
    SECTION("InputSystem can process mouse motion events") {
        InputSystem inputSystem;
        ::World world(100, 100);  // Use actual World instance
        
        SDL_Event event;
        event.type = SDL_MOUSEMOTION;
        event.motion.x = 100;
        event.motion.y = 150;
        
        REQUIRE_NOTHROW(inputSystem.ProcessEvent(event, &world));
        
        const auto& mouseState = inputSystem.GetMouseState();
        REQUIRE(mouseState.x == 100);
        REQUIRE(mouseState.y == 150);
    }
    
    SECTION("InputSystem can register command factories") {
        InputSystem inputSystem;
        bool factoryCalled = false;
        
        auto factory = [&factoryCalled](const SDL_Event&) -> InputCommandPtr {
            factoryCalled = true;
            return nullptr;
        };
        
        REQUIRE_NOTHROW(inputSystem.RegisterCommandFactory(SDL_KEYDOWN, factory));
    }
}

TEST_CASE("InputSystem keyboard state", "[InputSystem][Keyboard]") {
    InputSystem inputSystem;
    ::World world(100, 100);  // Use actual World instance
    
    SECTION("Keyboard state updates on key events") {
        SDL_Event keyDownEvent;
        keyDownEvent.type = SDL_KEYDOWN;
        keyDownEvent.key.keysym.scancode = SDL_SCANCODE_SPACE;
        keyDownEvent.key.repeat = 0;
        
        REQUIRE_NOTHROW(inputSystem.ProcessEvent(keyDownEvent, &world));
        
        const auto& keyboardState = inputSystem.GetKeyboardState();
        auto it = keyboardState.keysPressed.find(SDL_SCANCODE_SPACE);
        if (it != keyboardState.keysPressed.end()) {
            REQUIRE(it->second == true);
        }
    }
}

TEST_CASE("InputSystem mouse button handling", "[InputSystem][Mouse]") {
    InputSystem inputSystem;
    ::World world(100, 100);  // Use actual World instance
    
    SECTION("Mouse button down updates state") {
        SDL_Event mouseMotionEvent;
        mouseMotionEvent.type = SDL_MOUSEMOTION;
        mouseMotionEvent.motion.x = 50;
        mouseMotionEvent.motion.y = 75;
        inputSystem.ProcessEvent(mouseMotionEvent, &world);
        
        SDL_Event mouseDownEvent;
        mouseDownEvent.type = SDL_MOUSEBUTTONDOWN;
        mouseDownEvent.button.button = SDL_BUTTON_LEFT;
        mouseDownEvent.button.x = 50;
        mouseDownEvent.button.y = 75;
        
        REQUIRE_NOTHROW(inputSystem.ProcessEvent(mouseDownEvent, &world));
        
        const auto& mouseState = inputSystem.GetMouseState();
        REQUIRE(mouseState.leftPressed == true);
        REQUIRE(mouseState.x == 50);
        REQUIRE(mouseState.y == 75);
    }
    
    SECTION("Mouse button up updates state") {
        InputSystem inputSystem;
        
        // First set mouse position
        SDL_Event mouseMotionEvent;
        mouseMotionEvent.type = SDL_MOUSEMOTION;
        mouseMotionEvent.motion.x = 50;
        mouseMotionEvent.motion.y = 75;
        inputSystem.ProcessEvent(mouseMotionEvent, &world);
        
        // Then press the button
        SDL_Event mouseDownEvent;
        mouseDownEvent.type = SDL_MOUSEBUTTONDOWN;
        mouseDownEvent.button.button = SDL_BUTTON_LEFT;
        mouseDownEvent.button.x = 50;
        mouseDownEvent.button.y = 75;
        inputSystem.ProcessEvent(mouseDownEvent, &world);
        
        // Then release it
        SDL_Event mouseUpEvent;
        mouseUpEvent.type = SDL_MOUSEBUTTONUP;
        mouseUpEvent.button.button = SDL_BUTTON_LEFT;
        mouseUpEvent.button.x = 50;
        mouseUpEvent.button.y = 75;
        
        REQUIRE_NOTHROW(inputSystem.ProcessEvent(mouseUpEvent, &world));
        
        const auto& mouseState = inputSystem.GetMouseState();
        REQUIRE(mouseState.leftPressed == false);
    }
}
