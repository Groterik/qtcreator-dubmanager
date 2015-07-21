#ifndef DUBPROJECTMANAGERCONSTANTS_H
#define DUBPROJECTMANAGERCONSTANTS_H

namespace DubProjectManager {
namespace Constants {

const char ACTION_ID[] = "DubProjectManager.Action";
const char MENU_ID[] = "DubProjectManager.Menu";

const char * const PROJECTCONTEXT = "DubProject.ProjectContext";
const char * const DUBMIMETYPE  = "text/x-dub";
const char * const DUB_EDITOR_ID = "DubProject.DubEditor";
const char * const DUB_EDITOR_DISPLAY_NAME = "Dub Editor";
const char * const C_DUBEDITOR = "DubProject.Context.DubEditor";

// Project
const char * const DUBPROJECT_ID = "DubProjectManager.DubProject";

// Buildconfiguration
const char DUB_BC_ID[] = "DubProjectManager.DubBuildConfiguration";

// Saved
const char * const S_SOURCE_TREE_CONFIG = "DubProjectManager.DubProject.SourceTreeConfiguration";

// Context menu button
const char * const DUBUPDATECONTEXTMENU = "DubProjectManager.DubProject.UpdateMenu";

// No configuration string
const char* const DUB_NO_CONFIG = "<no config>";

} // namespace DubProjectManager
} // namespace Constants

#endif // DUBPROJECTMANAGERCONSTANTS_H

