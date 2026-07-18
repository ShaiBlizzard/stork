
#include "actWorker.h"
#include "argHandler.h"
#include "shared.h"

#include <qcoreapplication.h>

int main(int argc, char* argv[]) {
    const QCoreApplication app(argc, argv);
    const storkArgs actions = ArgHandler(app).process();
    // auto asdf = actWorker(actions);
    auto asdf = actWorker(actions);  // needs to be turned into a singleton

    return 0;
}
