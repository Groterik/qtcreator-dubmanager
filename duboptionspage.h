#ifndef DUBOPTIONSPAGE_H
#define DUBOPTIONSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

#include <QWidget>

class DubValidator;

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

signals:

public slots:

private:
    QWidget* m_widget;

    DubValidator *m_dubValidatorForUser;
    DubValidator *m_dubValidatorForSystem;

};

namespace Utils {
QT_FORWARD_DECLARE_CLASS(PathChooser)
}

class DubOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    DubOptionsWidget(QWidget *parent = 0);
    virtual ~DubOptionsWidget() {}
private slots:

private:
    Utils::PathChooser *m_pathChooser;
};

#endif // DUBOPTIONSPAGE_H
