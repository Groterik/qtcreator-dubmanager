#ifndef DUBWIZARD_H
#define DUBWIZARD_H

#include <coreplugin/iwizardfactory.h>

namespace DubProjectManager {

class DubWizard : public Core::IWizardFactory
{
public:
    DubWizard();

    virtual void runWizard(const QString &path, QWidget *parent,
                           const QString &platform, const QVariantMap &variables) Q_DECL_OVERRIDE;
};

} // namespace DubProjectManager

#endif // DUBWIZARD_H
