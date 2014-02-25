#include <SV/Application.hpp>

#include <stdexcept>
#include <iostream>


int main()
{
    try
    {
        Application app;
        app.run();
    }
    catch (std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
    }
}
