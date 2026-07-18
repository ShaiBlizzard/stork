#ifndef STORK_ACTWORKER_H
#define STORK_ACTWORKER_H
#include "shared.h"

class QFileInfo;


struct Task {
    StorkAction Atype {};
    const QString& parcel;
};


class actWorker {
public:
    explicit actWorker(storkArgs args);
    // QFileInfo getConflicts();

private:
    void processTask(Task task) const;
    void parseTasks(StorkAction action);
    bool hasConflicts(const QString& package) const;
    void processConflict(const QString& package);

    storkArgs m_args;
    QList<Task> m_tasks;
};

#endif // STORK_ACTWORKER_H
