#include "Application.hpp"

int main(int argc, char** argv) {
    TAApplication app(new Application(argc, argv));
    return app->exec();
}
