#include "../modules/core/Application.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Application app("Funhouse - Falling Sand Engine", 1280, 720);
    
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize application!" << std::endl;
        return -1;
    }

    app.Run();
    
    return 0;
}