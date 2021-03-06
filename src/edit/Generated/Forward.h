

#pragma once

#include <obj/Config.h>

#include <obj/Generated/Forward.h>
#include <math/Generated/Forward.h>
#include <geom/Generated/Forward.h>
#include <lang/Generated/Forward.h>
#include <ui/Generated/Forward.h>
#include <uio/Generated/Forward.h>
#include <gfx/Generated/Forward.h>

#if defined MUD_EDIT_LIB
#define MUD_EDIT_EXPORT MUD_EXPORT
#else
#define MUD_EDIT_EXPORT MUD_IMPORT
#endif


    
    

namespace std {

    
    
}

namespace mud {
namespace ui {

    
    
}
}

namespace glm {

    
    
}

namespace bgfx {

    
    
}

namespace mud {
namespace gfx {

    
    
}
}

namespace mud {

    enum class ToolState : unsigned int;
    
    
    class EditorAction;
    struct ToolContext;
    class ToolOption;
    class Tool;
    class ViewportTool;
    class SpatialTool;
    struct Gizmo;
    class TransformAction;
    class TransformTool;
    class ActionStack;
    class UndoTool;
    class RedoTool;
    class Brush;
    class PlaneSnapOption;
    class WorldSnapOption;
    class PlaceBrush;
    class CircleBrush;
    class ScriptedBrush;
    class TranslateAction;
    class TranslateTool;
    class RotateAction;
    class RotateTool;
    class ScaleAction;
    class ScaleTool;
    class CopyAction;
    class CopyTool;
    class ViewAction;
    class ViewTool;
    class EditContext;
    class WorkPlaneAction;
    class WorkPlaneTool;
}

namespace mud {
namespace detail {

    
    
}
}

namespace bimg {

    
    
}

namespace bx {

    
    
}

