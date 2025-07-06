#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/Commands/MouseCommands.h"
#include "../../modules/materials/Materials.h"
#include "../../modules/world/World.h"

using namespace Funhouse;

TEST_CASE("PlaceMaterialCommand functionality", "[MouseCommands][PlaceMaterial]") {
    ::World world(100, 100);
    
    SECTION("PlaceMaterialCommand can be created and executed") {
        auto command = std::make_unique<PlaceMaterialCommand>(&world, 10, 20, MaterialType::Sand);
        
        REQUIRE(command->GetName() == "PlaceMaterial");
        REQUIRE(world.GetPixel(10, 20) == MaterialType::Air);
        
        command->Execute();
        
        REQUIRE(world.GetPixel(10, 20) == MaterialType::Sand);
    }
    
    SECTION("PlaceMaterialCommand handles different materials") {
        auto sandCommand = std::make_unique<PlaceMaterialCommand>(&world, 5, 10, MaterialType::Sand);
        auto waterCommand = std::make_unique<PlaceMaterialCommand>(&world, 15, 25, MaterialType::Water);
        auto stoneCommand = std::make_unique<PlaceMaterialCommand>(&world, 35, 45, MaterialType::Stone);
        
        sandCommand->Execute();
        REQUIRE(world.GetPixel(5, 10) == MaterialType::Sand);
        
        waterCommand->Execute();
        REQUIRE(world.GetPixel(15, 25) == MaterialType::Water);
        
        stoneCommand->Execute();
        REQUIRE(world.GetPixel(35, 45) == MaterialType::Stone);
    }
    
    SECTION("PlaceMaterialCommand can be cloned") {
        auto original = std::make_unique<PlaceMaterialCommand>(&world, 50, 60, MaterialType::Water);
        auto clone = original->Clone();
        
        REQUIRE(clone->GetName() == original->GetName());
        
        // Both should work independently
        original->Execute();
        REQUIRE(world.GetPixel(50, 60) == MaterialType::Water);
        
        // Reset pixel
        world.SetPixel(50, 60, MaterialType::Air);
        
        clone->Execute();
        REQUIRE(world.GetPixel(50, 60) == MaterialType::Water);
    }
    
    SECTION("PlaceMaterialCommand handles null world gracefully") {
        auto command = std::make_unique<PlaceMaterialCommand>(nullptr, 10, 20, MaterialType::Sand);
        
        // Should not crash
        REQUIRE_NOTHROW(command->Execute());
    }
}

TEST_CASE("RemoveMaterialCommand functionality", "[MouseCommands][RemoveMaterial]") {
    ::World world(100, 100);
    
    SECTION("RemoveMaterialCommand can be created and executed") {
        // First place some material
        world.SetPixel(50, 75, MaterialType::Sand);
        REQUIRE(world.GetPixel(50, 75) == MaterialType::Sand);
        
        auto command = std::make_unique<RemoveMaterialCommand>(&world, 50, 75);
        
        REQUIRE(command->GetName() == "RemoveMaterial");
        
        command->Execute();
        
        REQUIRE(world.GetPixel(50, 75) == MaterialType::Air);
    }
    
    SECTION("RemoveMaterialCommand can be cloned") {
        // Place material first
        world.SetPixel(80, 90, MaterialType::Stone);
        REQUIRE(world.GetPixel(80, 90) == MaterialType::Stone);
        
        auto original = std::make_unique<RemoveMaterialCommand>(&world, 80, 90);
        auto clone = original->Clone();
        
        REQUIRE(clone->GetName() == original->GetName());
        
        // Both should work independently
        original->Execute();
        REQUIRE(world.GetPixel(80, 90) == MaterialType::Air);
        
        // Reset pixel
        world.SetPixel(80, 90, MaterialType::Stone);
        
        clone->Execute();
        REQUIRE(world.GetPixel(80, 90) == MaterialType::Air);
    }
    
    SECTION("RemoveMaterialCommand handles null world gracefully") {
        auto command = std::make_unique<RemoveMaterialCommand>(nullptr, 10, 20);
        
        // Should not crash
        REQUIRE_NOTHROW(command->Execute());
    }
}

TEST_CASE("Mouse commands inheritance", "[MouseCommands][Inheritance]") {
    ::World world(100, 100);
    
    SECTION("Commands are replayable by default") {
        auto placeCommand = std::make_unique<PlaceMaterialCommand>(&world, 10, 20, MaterialType::Sand);
        auto removeCommand = std::make_unique<RemoveMaterialCommand>(&world, 30, 40);
        
        REQUIRE(placeCommand->IsReplayable());
        REQUIRE(removeCommand->IsReplayable());
    }
    
    SECTION("Commands have timestamps") {
        auto before = std::chrono::steady_clock::now();
        auto command = std::make_unique<PlaceMaterialCommand>(&world, 10, 20, MaterialType::Sand);
        auto after = std::chrono::steady_clock::now();
        
        auto timestamp = command->GetTimestamp();
        REQUIRE(timestamp >= before);
        REQUIRE(timestamp <= after);
    }
}
