#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/Commands/KeyboardCommands.h"
#include "../../modules/materials/Materials.h"
#include "../../modules/world/World.h"

using namespace Funhouse;

TEST_CASE("SelectMaterialCommand functionality", "[KeyboardCommands][SelectMaterial]") {
    MaterialType selectedMaterial = MaterialType::Air;
    bool callbackCalled = false;
    
    auto callback = [&selectedMaterial, &callbackCalled](MaterialType material) {
        selectedMaterial = material;
        callbackCalled = true;
    };
    
    SECTION("SelectMaterialCommand can be created and executed") {
        auto command = std::make_unique<SelectMaterialCommand>(MaterialType::Sand, callback);
        
        REQUIRE(command->GetName() == "SelectMaterial");
        REQUIRE_FALSE(callbackCalled);
        REQUIRE(selectedMaterial == MaterialType::Air);
        
        command->Execute();
        
        REQUIRE(callbackCalled);
        REQUIRE(selectedMaterial == MaterialType::Sand);
    }
    
    SECTION("SelectMaterialCommand works with different materials") {
        auto sandCommand = std::make_unique<SelectMaterialCommand>(MaterialType::Sand, callback);
        auto waterCommand = std::make_unique<SelectMaterialCommand>(MaterialType::Water, callback);
        auto stoneCommand = std::make_unique<SelectMaterialCommand>(MaterialType::Stone, callback);
        
        sandCommand->Execute();
        REQUIRE(selectedMaterial == MaterialType::Sand);
        
        waterCommand->Execute();
        REQUIRE(selectedMaterial == MaterialType::Water);
        
        stoneCommand->Execute();
        REQUIRE(selectedMaterial == MaterialType::Stone);
    }
    
    SECTION("SelectMaterialCommand can be cloned") {
        auto original = std::make_unique<SelectMaterialCommand>(MaterialType::Water, callback);
        auto clone = original->Clone();
        
        REQUIRE(clone->GetName() == original->GetName());
        
        // Reset state
        selectedMaterial = MaterialType::Air;
        callbackCalled = false;
        
        clone->Execute();
        REQUIRE(callbackCalled);
        REQUIRE(selectedMaterial == MaterialType::Water);
    }
    
    SECTION("SelectMaterialCommand handles null callback gracefully") {
        auto command = std::make_unique<SelectMaterialCommand>(MaterialType::Sand, nullptr);
        
        // Should not crash
        REQUIRE_NOTHROW(command->Execute());
    }
}

TEST_CASE("ToggleRecordingCommand functionality", "[KeyboardCommands][ToggleRecording]") {
    bool toggleCalled = false;
    int toggleCount = 0;
    
    auto callback = [&toggleCalled, &toggleCount]() {
        toggleCalled = true;
        toggleCount++;
    };
    
    SECTION("ToggleRecordingCommand can be created and executed") {
        auto command = std::make_unique<ToggleRecordingCommand>(callback);
        
        REQUIRE(command->GetName() == "ToggleRecording");
        REQUIRE_FALSE(toggleCalled);
        REQUIRE(toggleCount == 0);
        
        command->Execute();
        
        REQUIRE(toggleCalled);
        REQUIRE(toggleCount == 1);
    }
    
    SECTION("ToggleRecordingCommand can be executed multiple times") {
        auto command = std::make_unique<ToggleRecordingCommand>(callback);
        
        command->Execute();
        REQUIRE(toggleCount == 1);
        
        command->Execute();
        REQUIRE(toggleCount == 2);
        
        command->Execute();
        REQUIRE(toggleCount == 3);
    }
    
    SECTION("ToggleRecordingCommand can be cloned") {
        auto original = std::make_unique<ToggleRecordingCommand>(callback);
        auto clone = original->Clone();
        
        REQUIRE(clone->GetName() == original->GetName());
        
        // Both should work independently
        original->Execute();
        REQUIRE(toggleCount == 1);
        
        clone->Execute();
        REQUIRE(toggleCount == 2);
    }
    
    SECTION("ToggleRecordingCommand handles null callback gracefully") {
        auto command = std::make_unique<ToggleRecordingCommand>(nullptr);
        
        // Should not crash
        REQUIRE_NOTHROW(command->Execute());
    }
    
    SECTION("ToggleRecordingCommand is not replayable") {
        auto command = std::make_unique<ToggleRecordingCommand>(callback);
        REQUIRE_FALSE(command->IsReplayable());
    }
}

TEST_CASE("ClearWorldCommand functionality", "[KeyboardCommands][ClearWorld]") {
    ::World world(10, 10);
    
    // Set some pixels first
    world.SetPixel(5, 5, MaterialType::Sand);
    world.SetPixel(3, 7, MaterialType::Water);
    
    SECTION("ClearWorldCommand can be created and executed") {
        auto command = std::make_unique<ClearWorldCommand>(&world);
        
        REQUIRE(command->GetName() == "ClearWorld");
        
        // Verify pixels are set before clearing
        REQUIRE(world.GetPixel(5, 5) == MaterialType::Sand);
        REQUIRE(world.GetPixel(3, 7) == MaterialType::Water);
        
        command->Execute();
        
        // After clearing, all pixels should be Air
        REQUIRE(world.GetPixel(5, 5) == MaterialType::Air);
        REQUIRE(world.GetPixel(3, 7) == MaterialType::Air);
    }
    
    SECTION("ClearWorldCommand can be cloned") {
        auto original = std::make_unique<ClearWorldCommand>(&world);
        auto clone = original->Clone();
        
        REQUIRE(clone->GetName() == original->GetName());
        
        // Set a pixel again
        world.SetPixel(2, 3, MaterialType::Stone);
        REQUIRE(world.GetPixel(2, 3) == MaterialType::Stone);
        
        clone->Execute();
        REQUIRE(world.GetPixel(2, 3) == MaterialType::Air);
    }
    
    SECTION("ClearWorldCommand handles null world gracefully") {
        auto command = std::make_unique<ClearWorldCommand>(nullptr);
        
        // Should not crash
        REQUIRE_NOTHROW(command->Execute());
    }
    
    SECTION("ClearWorldCommand is replayable") {
        auto command = std::make_unique<ClearWorldCommand>(&world);
        REQUIRE(command->IsReplayable());
    }
}
