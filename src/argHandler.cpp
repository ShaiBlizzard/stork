#include "argHandler.h"

#include "argParser.h"

#include <qlogging.h>

ArgHandler::ArgHandler(const QCoreApplication& core_arguments) : s_args(ArgParser::parse(core_arguments)) {
    // qDebug() <<  s_args.paths.parentPath;
    sanitizePaths();
    nodeParser();
    qDebug() << s_args.packages[StorkAction::Stow];
}

storkArgs& ArgHandler::process() {
    // auto& actions = ArgParser::parse(core_arguments);
    // todo: config file? and something about args from past me
    // sanitizePaths(actions);

    return s_args;
}

void ArgHandler::sanitizePaths() {
    if (s_args.packages.isEmpty()) { qFatal() << ":No Packages to Stow or Unstow"; }

    // magic_enum::enum_for_each<StorkAction>([this](const StorkAction action)
    for (auto action : StorkActions) {
        for (auto& package : s_args.packages[action]) {
            if (package.contains(QString("/")))
                qFatal() << Q_FUNC_INFO << ": Slashes not permitted in package names " << package;
            if (!s_args.paths.parentPath.exists(package))
                qFatal() << Q_FUNC_INFO << ": Package not found in parent path:  " << package;
        }
    }
}

void ArgHandler::nodeParser() {
    for (auto action : StorkActions) {
        for (auto& package : s_args.packages[action]) {
            auto pkgPath = s_args.paths.absParentFilePath(package);
            if (!QFileInfo(pkgPath).isDir()) return;
            nodeLoop(package, action);
        }
    }
}

/*
 * nodePath should be a dir path relative to parentPath
 * ex: config, config/extras
 */
void ArgHandler::nodeLoop(const QString& nodePath, const StorkAction action) {
    const auto nodeParentPath = s_args.paths.absParentFilePath(nodePath);
    const auto nodeTgtPath = s_args.paths.absTargetFilePath(nodePath);

    // check if can just stow it directly
    if (const auto _ = QFileInfo(nodeTgtPath); !_.exists() || _.isSymLink()) {
        s_args.packages[action].insert(s_args.paths.parentPath.relativeFilePath(nodePath));
        return;
    }

    // if we're here then we can't sym the folder directly
    // todo: check if symlinks included
    s_args.packages[action].remove(nodePath);
    for (auto entry : QDirListing(nodeParentPath, QDirListing::IteratorFlag::FilesOnly)) {
        auto relPath = s_args.paths.parentPath.relativeFilePath(entry.filePath());
        s_args.packages[action].insert(relPath);
    }
    // [welcome to loop iteration]
    for (auto entry : QDirListing(nodeParentPath, QDirListing::IteratorFlag::DirsOnly)) {
        auto relPath = s_args.paths.parentPath.relativeFilePath(entry.filePath());
        nodeLoop(relPath, action);
    }
}
