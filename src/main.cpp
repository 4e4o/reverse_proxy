#include "Application.hpp"

int main(int argc, char** argv) {
    std::unique_ptr<Application> app(new Application(argc, argv));
    return app->exec();
}
