#include "actWorker.h"

#include "magic_enum/magic_enum_utility.hpp"

#include <qdir.h>

actWorker::actWorker(storkArgs args) : m_args(std::move(args)) {
    for (const auto action : StorkActions) parseTasks(action);

    // list conflicts?
    if (m_args.options.dryRun) {
        qWarning() << "Dry-run, no FS changes here";
        return;
    }

    qWarning() << "Abt to process";
    for (const auto& task : m_tasks) processTask(task);
}

void actWorker::processTask(const Task task) const {
    const auto target = m_args.paths.absTargetFilePath(task.parcel);
    const auto relPath = m_args.paths.relativePath(task.parcel);
    qWarning() << relPath;
    switch (task.Atype) {
    case StorkAction::Unstow: {
        QFile::moveToTrash(target);
        qWarning() << "trashing.." << task.parcel;
        return;
    }
    case StorkAction::Adopt: {
        const auto source = m_args.paths.absParentFilePath(task.parcel);
        QFile::moveToTrash(source);
        QFile::rename(target, source);
        qWarning() << "adopting.." << task.parcel;
        goto stowTask;
        return;
    }
    case StorkAction::Force: {
        QFile::moveToTrash(target);
        qWarning() << "forcing.." << task.parcel;
        goto stowTask;
        return;
    }
    case StorkAction::Stow: {
    stowTask:
        QFile::link(relPath, target);
        qWarning() << "linking.." << task.parcel;
        break;
    }
    }

}

void actWorker::parseTasks(const StorkAction action) {
    auto& packages = m_args.packages[action];
    if (packages.empty()) return;

    // const auto actionStr = magic_enum::enum_name(action);

    for (const QString& package : packages) {
        if (hasConflicts(package)) processConflict(package);
        else m_tasks.append({action, package});
    }
}

bool actWorker::hasConflicts(const QString& package) const {
    const auto pkgInfo = QFileInfo(m_args.paths.absTargetFilePath(package));
    if (!pkgInfo.exists()) return false;
    if (!pkgInfo.isSymLink()) return true;
    if (pkgInfo.symLinkTarget() != m_args.paths.relativePath(package)) return true;

    return false;
}

void actWorker::processConflict(const QString& package) {
    switch (m_args.options.conflictAct) {
    case ConflictAction::Cancel:
        qCritical() << "conflict found, canceling";
    case ConflictAction::Ignore:
        break;
    case ConflictAction::Adopt: {
        m_tasks.append({StorkAction::Adopt, package});
        break;
    }
    case ConflictAction::Force: {
        m_tasks.append({StorkAction::Force, package});
        break;
    }
    }
}
