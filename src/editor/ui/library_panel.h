#pragma once

#include "editor/ui/base_panel.h"
#include "render/asset/texture_asset.h"
#include "core/file/file.h"
#include "render/rhi/rhi_descriptor.h"
#include "render/rhi/rhi_ptr.h"


namespace luna::editor
{
class LibraryEditor : public EditorBase
{
	RegisterTypeEmbedd(LibraryEditor, EditorBase);
public:
	LibraryEditor()
	{
		mTitle = "Library";
	}

	void Init() override;
	void OnGUI() override;

	void FolderRecursive(LFileInfo* info);
	void OnDrawFolder();
	void OnDrawLibrary();
protected:
	const LFileInfo* m_cur_folder = nullptr;
	const LFileInfo* m_temp_folder = nullptr;
	render::Texture2D* m_p;
	void Item(const LFileInfo& info);
	const LFileInfo* m_select_file = nullptr;
	render::TRHIPtr<render::RHIView> m_file_descriptor;
};
}
