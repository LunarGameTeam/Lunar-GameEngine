#pragma once
#include "core/math/math.h"
#include "core/foundation/container.h"
#include "core/foundation/string.h"
#ifndef RESOURCE_IMPORT_API
#ifdef RESOURCE_IMPORT_EXPORT
#define RESOURCE_IMPORT_API __declspec( dllexport )//∫Í∂®“Â
#else
#define RESOURCE_IMPORT_API __declspec( dllimport )
#endif
#endif
namespace luna::ImportData
{

}

