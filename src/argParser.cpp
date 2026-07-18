#include "argParser.h"
#include "qdebug.h"
#include "shared.h"

#include <QCommandLineParser>
#include <qdir.h>

storkArgs ArgParser::parse(const QCoreApplication& core_arguments) {
    QCommandLineParser parser;
    parser.setApplicationDescription("todo_descr");
    parser.addHelpOption();
    // always found -d confusing, mixing em up
    QCommandLineOption parentPathOption({"s", "src"}, "parent package dir to stow", "package_path");
    QCommandLineOption targetPathOption({"d", "dest"}, "target", "target_path");

    QCommandLineOption dryRunOption({"simulate", "n", "no"}, "Do not modify FS");
    QCommandLineOption adoptOption({"adopt", "a"}, "Adopt");
    QCommandLineOption forceOption({"force", "f"}, "Force");

    QCommandLineOption stowOption({"S", "stow"}, "explicitly stow", "package");
    QCommandLineOption deleteOption({"D", "delete"}, "delet dis", "package");
    QCommandLineOption restowOption({"R", "restow"}, "both", "package");

    parser.addPositionalArgument("package", "path to symlink");

    parser.addOptions({
        dryRunOption,
        adoptOption,
        forceOption,
        stowOption,
        deleteOption,
        restowOption,
        parentPathOption,
        targetPathOption
    });

    parser.process(core_arguments);
    if (!parser.unknownOptionNames().isEmpty()) { qCritical() << parser.errorText(); }

    storkArgs actions;

    //----------------- handle pre-set/positionals
    {
        auto parsedStow = parser.values(stowOption) + parser.values(restowOption);
        actions.packages[StorkAction::Stow] = QSet(parsedStow.begin(), parsedStow.end());
        auto parsedUnstow = parser.values(deleteOption) + parser.values(restowOption);
        actions.packages[StorkAction::Unstow] = QSet(parsedUnstow.begin(), parsedUnstow.end());
        if (auto _ = parser.positionalArguments(); actions.packages.isEmpty()) {
            actions.packages[StorkAction::Stow] = QSet(_.begin(), _.end());
        } else if (!parser.positionalArguments().isEmpty())
            qFatal() << "bad:" << parser.positionalArguments();
    }
    // --------------- handle dirs
    {
        const QString parentStr = parser.value(parentPathOption);
        actions.paths.parentPath = QDir(parentStr.isEmpty() ? QDir::currentPath() : parentStr);
        actions.paths.parentPath.makeAbsolute();
        if (!actions.paths.parentPath.exists())
            qFatal() << "invalid parent path";

        const QString targetStr = parser.value(targetPathOption);
        actions.paths.targetPath = QDir(targetStr.isEmpty() ? QDir(actions.paths.parentPath).path() : targetStr);
        actions.paths.targetPath.makeAbsolute();
        if (!actions.paths.targetPath.exists())
            qFatal() << "invalid target path";
    }

    // the rest
    actions.options.dryRun = parser.isSet(dryRunOption);
    if (parser.isSet(adoptOption) && parser.isSet(forceOption))
        qFatal() << "no.";
    actions.options.conflictAct = parser.isSet(adoptOption) ? ConflictAction::Adopt : parser.isSet(forceOption)
                                      ? ConflictAction::Force : ConflictAction::Cancel;

    return actions;
}
