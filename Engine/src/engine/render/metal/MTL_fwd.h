#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"

// for some reason the Appkit headers try to drag in se types unless we forward declare the NS version.
namespace NS
{
    class String;
    class Application;
}

#include "AppKit/NSApplication.hpp"
#include "AppKit/AppKit.hpp"
#include "MetalKit/MetalKit.hpp"
#include "MetalKit/MTKView.hpp"

#pragma clang diagnostic pop