#ifndef STORK_ARGHANDLER_H
#define STORK_ARGHANDLER_H
#include "shared.h"

enum class StorkAction;
class QString;
class QCoreApplication;
struct storkArgs;

class ArgHandler {
public:
    explicit ArgHandler(const QCoreApplication& core_arguments);
    storkArgs& process();
private:
    void sanitizePaths();
    void nodeParser();
    void nodeLoop(const QString& nodePath, StorkAction action);

    storkArgs s_args;
};

#endif // STORK_ARGHANDLER_H
