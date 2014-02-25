#ifndef SV_APPLICATION_HPP
#define SV_APPLICATION_HPP

class Application
{
    public:
                Application();
        void    run();

    private:
        void    update();
        void    render();
};

#endif // SV_APPLICATION_HPP
