#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/InputCommand.h"
#include <chrono>
#include <thread>

using namespace Funhouse;

// Test command implementation
class TestCommand : public InputCommand {
public:
    TestCommand(int& counter) : counter_(counter), executeCalled_(false) {}
    
    void Execute() override {
        counter_++;
        executeCalled_ = true;
    }
    
    void Undo() override {
        counter_--;
    }
    
    std::string GetName() const override {
        return "TestCommand";
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TestCommand>(counter_);
    }
    
    bool WasExecuted() const { return executeCalled_; }
    
private:
    int& counter_;
    bool executeCalled_;
};

TEST_CASE("InputCommand", "[input]") {
    SECTION("Basic command execution") {
        int counter = 0;
        TestCommand cmd(counter);
        
        REQUIRE(counter == 0);
        REQUIRE(!cmd.WasExecuted());
        
        cmd.Execute();
        
        REQUIRE(counter == 1);
        REQUIRE(cmd.WasExecuted());
    }
    
    SECTION("Command undo") {
        int counter = 5;
        TestCommand cmd(counter);
        
        cmd.Execute();
        REQUIRE(counter == 6);
        
        cmd.Undo();
        REQUIRE(counter == 5);
    }
    
    SECTION("Command name") {
        int counter = 0;
        TestCommand cmd(counter);
        
        REQUIRE(cmd.GetName() == "TestCommand");
    }
    
    SECTION("Command timestamp") {
        int counter = 0;
        auto before = std::chrono::steady_clock::now();
        
        // Small delay to ensure timestamp difference
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        TestCommand cmd(counter);
        auto cmdTime = cmd.GetTimestamp();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto after = std::chrono::steady_clock::now();
        
        REQUIRE(cmdTime > before);
        REQUIRE(cmdTime < after);
    }
    
    SECTION("Command cloning") {
        int counter = 0;
        TestCommand original(counter);
        
        auto clone = original.Clone();
        REQUIRE(clone != nullptr);
        REQUIRE(clone->GetName() == "TestCommand");
        
        // Execute clone should affect same counter
        clone->Execute();
        REQUIRE(counter == 1);
    }
    
    SECTION("IsReplayable default") {
        int counter = 0;
        TestCommand cmd(counter);
        
        REQUIRE(cmd.IsReplayable() == true);
    }
}