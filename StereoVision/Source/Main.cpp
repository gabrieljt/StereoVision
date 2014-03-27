#include <SV/Application.hpp>

#include <stdexcept>
#include <iostream>


int main()
{
	int exitCode = 0;

    try
    {
        Application app;
        app.run();
    }
    catch (std::exception& e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
		exitCode = 1;
        std::cout << std::endl << "Press Enter key to exit.";
        std::cin.get();
    }
    std::cout << "Bye!" << std::endl;

	return exitCode;
}
