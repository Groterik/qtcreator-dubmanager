#ifndef DUBOPTIONSPAGE_H
#define DUBOPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

#include <QWidget>

namespace Utils {
class PathChooser;
}

namespace DubProjectManager {

class DubValidator;
class DubOptionsWidget;

class DubOptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    explicit DubOptionsPage();
    virtual ~DubOptionsPage();

    // pure Core::IOptionsPage

    virtual QWidget *widget();
    virtual void apply();
    virtual void finish();

    // others

    QString findDubExecutable() const;
    static QString executable();

signals:

public slots:

private:
    DubOptionsWidget* m_widget;

    DubValidator *m_dubValidatorForUser;
    DubValidator *m_dubValidatorForSystem;
};

class DubOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    DubOptionsWidget(QWidget *parent = 0);
    virtual ~DubOptionsWidget() {}
    QString path() const;
private slots:

private:
    Utils::PathChooser *m_pathChooser;
};

} // namespace DubProjectManager

#endif // DUBOPTIONSPAGE_H
