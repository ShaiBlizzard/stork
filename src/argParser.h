#ifndef STORK_ARGPARSER_H
#define STORK_ARGPARSER_H

class QCoreApplication;
struct storkArgs;

class ArgParser {
public:
    static storkArgs parse(const QCoreApplication& core_arguments);
};

#endif // STORK_ARGPARSER_H
