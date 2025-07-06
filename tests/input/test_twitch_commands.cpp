#include "../external/catch_amalgamated.hpp"
#include "../../modules/input/Commands/TwitchCommands.h"
#include "../../modules/input/Commands/TwitchCommandAdapter.h"
#include "../../modules/input/InputSystem.h"
#include "../../modules/world/World.h"
#include <memory>

using namespace Funhouse;

// Mock TwitchIrcClient for testing
class MockTwitchIrcClient {
public:
    MockTwitchIrcClient() : command_buffer_(100) {}
    
    TwitchCommandBuffer& getCommandBuffer() { return command_buffer_; }
    const TwitchCommandBuffer& getCommandBuffer() const { return command_buffer_; }
    bool isConnected() const { return true; }
    
    // Helper method for tests
    void addTestCommand(const std::string& username, const std::string& command, const std::string& params = "") {
        TwitchCommand cmd(username, command, params);
        command_buffer_.addCommand(cmd);
    }
    
private:
    TwitchCommandBuffer command_buffer_;
};

// Concrete test implementation of TwitchInputCommand
class TestTwitchCommand : public TwitchInputCommand {
public:
    TestTwitchCommand(const TwitchCommand& cmd) : TwitchInputCommand(cmd) {}
    
    void Execute() override {
        // Test implementation
    }
    
    std::unique_ptr<InputCommand> Clone() const override {
        return std::make_unique<TestTwitchCommand>(
            TwitchCommand(username_, command_, parameters_)
        );
    }
};

TEST_CASE("TwitchInputCommand base functionality", "[TwitchCommands]") {
    TwitchCommand twitchCmd("testuser", "jump", "high");
    TestTwitchCommand cmd(twitchCmd);
    
    SECTION("Stores Twitch metadata correctly") {
        REQUIRE(cmd.GetUsername() == "testuser");
        REQUIRE(cmd.GetCommand() == "jump");
        REQUIRE(cmd.GetParameters() == "high");
    }
    
    SECTION("GetName returns proper format") {
        REQUIRE(cmd.GetName() == "TwitchCommand:jump");
    }
    
    SECTION("Preserves timestamp from original command") {
        REQUIRE(cmd.GetTimestamp() == twitchCmd.timestamp);
    }
}

TEST_CASE("TwitchChatCommand executes callbacks", "[TwitchCommands]") {
    bool callbackExecuted = false;
    std::string capturedUsername;
    std::string capturedCommand;
    std::string capturedParams;
    
    TwitchCommand twitchCmd("alice", "test", "param1 param2");
    
    auto callback = [&](const std::string& username, const std::string& command, const std::string& params) {
        callbackExecuted = true;
        capturedUsername = username;
        capturedCommand = command;
        capturedParams = params;
    };
    
    TwitchChatCommand chatCmd(twitchCmd, callback);
    
    SECTION("Executes callback with correct parameters") {
        chatCmd.Execute();
        
        REQUIRE(callbackExecuted == true);
        REQUIRE(capturedUsername == "alice");
        REQUIRE(capturedCommand == "test");
        REQUIRE(capturedParams == "param1 param2");
    }
    
    SECTION("Handles null callback gracefully") {
        TwitchChatCommand nullCallbackCmd(twitchCmd);
        REQUIRE_NOTHROW(nullCallbackCmd.Execute());
    }
    
    SECTION("Clone preserves callback") {
        auto cloned = chatCmd.Clone();
        REQUIRE(cloned != nullptr);
        
        callbackExecuted = false;
        cloned->Execute();
        REQUIRE(callbackExecuted == true);
    }
}

TEST_CASE("TwitchCommandAdapter structure and registration", "[TwitchCommands]") {
    auto world = std::make_unique<World>(800, 600);
    auto inputSystem = std::make_unique<InputSystem>();
    inputSystem->SetWorld(world.get());
    
    // We'll test the adapter structure without actually using a real client
    // This avoids the threading issues with mocking
    
    SECTION("Can create adapter with null client") {
        auto adapter = std::make_unique<TwitchCommandAdapter>(nullptr, inputSystem.get());
        
        // Should handle null client gracefully
        REQUIRE_NOTHROW(adapter->Update());
        REQUIRE(adapter->IsConnected() == false);
    }
    
    SECTION("Command handler registration") {
        auto adapter = std::make_unique<TwitchCommandAdapter>(nullptr, inputSystem.get());
        
        // Test that we can register handlers without crashing
        REQUIRE_NOTHROW(adapter->RegisterCommandHandler("test", 
            [](const TwitchCommand&) -> InputCommandPtr {
                return nullptr;
            }));
        
        REQUIRE_NOTHROW(adapter->RegisterCommandCallback("test2",
            [](const std::string&, const std::string&, const std::string&) {}));
            
        REQUIRE_NOTHROW(adapter->SetProcessUnregisteredCommands(false));
    }
}

TEST_CASE("TwitchSpawnCommand structure", "[TwitchCommands]") {
    TwitchCommand twitchCmd("player", "spawn", "sand 100 200");
    TwitchSpawnCommand spawnCmd(twitchCmd);
    
    SECTION("Inherits from TwitchInputCommand") {
        REQUIRE(spawnCmd.GetUsername() == "player");
        REQUIRE(spawnCmd.GetCommand() == "spawn");
        REQUIRE(spawnCmd.GetParameters() == "sand 100 200");
    }
    
    SECTION("Can be cloned") {
        auto cloned = spawnCmd.Clone();
        REQUIRE(cloned != nullptr);
        
        auto clonedSpawn = dynamic_cast<TwitchSpawnCommand*>(cloned.get());
        REQUIRE(clonedSpawn != nullptr);
        REQUIRE(clonedSpawn->GetParameters() == "sand 100 200");
    }
    
    SECTION("Execute is callable") {
        REQUIRE_NOTHROW(spawnCmd.Execute());
    }
}

TEST_CASE("TwitchCommandBuffer integration", "[TwitchCommands]") {
    TwitchCommandBuffer buffer(10);
    
    SECTION("Commands can be added and retrieved") {
        TwitchCommand cmd1("user1", "cmd1", "");
        TwitchCommand cmd2("user2", "cmd2", "params");
        
        buffer.addCommand(cmd1);
        buffer.addCommand(cmd2);
        
        auto commands = buffer.getNewCommands();
        REQUIRE(commands.size() == 2);
        REQUIRE(commands[0].username == "user1");
        REQUIRE(commands[1].username == "user2");
    }
    
    SECTION("getNewCommands returns only new commands") {
        buffer.addCommand(TwitchCommand("user", "cmd", ""));
        
        auto first = buffer.getNewCommands();
        REQUIRE(first.size() == 1);
        
        auto second = buffer.getNewCommands();
        REQUIRE(second.size() == 0);
        
        buffer.addCommand(TwitchCommand("user2", "cmd2", ""));
        auto third = buffer.getNewCommands();
        REQUIRE(third.size() == 1);
    }
}