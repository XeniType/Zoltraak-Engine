#include <zoltraak_engine.h>

int main(int argc, char** argv) 
{
    Zoltraak::zoltraak_engine engine;
    
    engine.init();
    
    engine.run();
    
    engine.cleanup();
    
    return 0;
}
