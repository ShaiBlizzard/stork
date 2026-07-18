#ifndef STORK_SHARED_H
#define STORK_SHARED_H
#include <qdir.h>

enum class StorkAction {
    Stow,
    Adopt,
    Force,
    Unstow
};

inline QSet StorkActions = {StorkAction::Stow, StorkAction::Unstow};

struct StorkPaths {
    QDir parentPath;
    QDir targetPath;

    QString relativePath(const QString& filePath) const {
        return QDir(QFileInfo( targetPath.absoluteFilePath(filePath)).absoluteDir().path()).relativeFilePath(parentPath.absoluteFilePath(filePath));
    }

    QString absParentFilePath(const QString& fileName) const { return parentPath.absoluteFilePath(fileName); }
    QString absTargetFilePath(const QString& filename) const { return targetPath.absoluteFilePath(filename); }
    // QDir targetRelPath;
};

// adopt and force should only work on stow, not unstow
enum class ConflictAction { Cancel, Adopt, Ignore, Force };

struct storkOptions {
    bool dryRun;
    bool recursive;
    bool verbose; // might change to allow diff verb levels

    ConflictAction conflictAct;
    QString rulesPath = ".stork-rules";
};

struct storkArgs {
    StorkPaths paths;
    QHash<StorkAction, QSet<QString>> packages;
    storkOptions options{};
};

#endif // STORK_SHARED_H
