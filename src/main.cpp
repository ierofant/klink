#include "app.hpp"

int main(int _argc, char *_argv[])
{
    auto app = App::create(_argc, _argv);
    return app->run();
}
