#include "core.h"
#include <ostream>
#include "ui.h"
#include "update.h"

int main() {
    ProtoThiApp app;
    app.setUICallback(UI);
    app.setUpdateCallback(update);
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}