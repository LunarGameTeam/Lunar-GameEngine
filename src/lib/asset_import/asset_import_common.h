#pragma once
#ifndef ASSET_IMPORT_API
#ifdef ASSET_IMPORT_EXPORT
#define ASSET_IMPORT_API __declspec( dllexport )//�궨��
#else
#define ASSET_IMPORT_API __declspec( dllimport )
#endif
#endif
namespace luna::assetimport
{

}

