#include <iostream>
#include <Window/Window.h>


int main() {
    
    SDL_SetAppMetadata("My Game", "1.0", "com.xenitype.zoltraak");
    
    try
    {
        Zoltraak::Window window("Zoltraak", 640, 480);
        
        while (!window.shouldClose())
        {
            window.update();
        }
    }catch (std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
