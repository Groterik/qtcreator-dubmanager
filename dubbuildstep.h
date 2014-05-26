#ifndef DUBBUILDSTEP_H
#define DUBBUILDSTEP_H

#include <projectexplorer/abstractprocessstep.h>

class DubBuildStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    explicit DubBuildStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id);
    
signals:
    
public slots:
    
};

#endif // DUBBUILDSTEP_H
