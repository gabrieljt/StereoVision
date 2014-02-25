#include <SV/Application.hpp>

#include <stdexcept>
#include <iostream>


int main()
{
    try
    {
        Application app("Left Camera", "Right Camera");
        app.run();
    }
    catch (std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
    }
}
