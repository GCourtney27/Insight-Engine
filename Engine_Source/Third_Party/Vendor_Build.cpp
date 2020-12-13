#include <Engine_pch.h>

// ImGui
#if defined (IE_PLATFORM_BUILD_WIN32)
#include <examples/imgui_impl_win32.cpp>
#include <misc/cpp/imgui_stdlib.cpp>

// ImGuizmo
#pragma message ("ImGuizmo is not compiling. Issue with vendor source code.")
//#include "ImGuizmo.cpp"
//#include "ImGradient.cpp"
//#include "ImCurveEdit.cpp"
#endif // IE_PLATFORM_BUILD_WIN32

// RapidJson
#include <rapidjson/json.cpp>

// DXR
#include "DXR/nv_helpers_dx12/TopLevelASGenerator.cpp"
#include "DXR/nv_helpers_dx12/BottomLevelASGenerator.cpp"
#include "DXR/nv_helpers_dx12/RootSignatureGenerator.cpp"
#include "DXR/nv_helpers_dx12/RaytracingPipelineGenerator.cpp"
#include "DXR/nv_helpers_dx12/ShaderBindingTableGenerator.cpp"

// OpenFBX
#if defined (IE_PLATFORM_BUILD_UWP)
#include "miniz.c"
#include "miniz.h"
#include "ofbx.cpp"
#endif

