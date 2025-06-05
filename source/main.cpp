#include "core/application.h"

int main() {
    Application app = Application("v0.1");
    app.Initialize();
    app.Run();
    app.Close();
}