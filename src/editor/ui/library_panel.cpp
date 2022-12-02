#include "editor/ui/library_panel.h"
#include "editor/editor_module.h"
#include "render/asset/material_template.h"
#include "render/render_module.h"
#include "core/asset/asset_module.h"
#include "core/file/file.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"
#include "icon_font.h"


namespace luna::editor
{

RegisterTypeEmbedd_Imp(LibraryEditor)
{
	cls->Ctor<LibraryEditor>();
	cls->Binding<LibraryEditor>();
	LBindingModule::Get("luna.editor")->AddType(cls);
};

void LibraryEditor::Init()
{
	sAssetModule->CollectAssets();
	auto& root = sAssetModule->GetResRoot();
	m_cur_folder = &root;
	//	m_p = g_asset_sys->LoadAsset<render::Texture2D>("assets/white.dds");
	//m_p->Init();
	//m_p->CreateDescriptor(g_editor_sys->GetImguiRsvHeap());
}

void LibraryEditor::OnGUI()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Material"))
			{
				render::MaterialTemplateAsset* asset = TCreateObject<render::MaterialTemplateAsset>();
				sAssetModule->SaveAsset(asset, "assets/new_material.mat");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	OnDrawFolder();
}

void LibraryEditor::FolderRecursive(LFileInfo* info)
{
	int flags = 0;
	if (info->mParent == nullptr)
	{
		for (auto& f : info->mFolderContents)
		{
			FolderRecursive(&(f.second));
		}
		return;
	}
	const char* icon = info->mIsFolder ? ICON_FA_FOLDER : ICON_FA_FILE;

	if (info->mIsFolder && info->mFolderContents.size() == 0)
		flags = flags | ImGuiTreeNodeFlags_Leaf;
	if (!info->mIsFolder)
		flags = flags | ImGuiTreeNodeFlags_Leaf;
	if (info->mName.EndsWith("png"))
	{
		icon = ICON_FA_FILE_IMAGE;
	}

	if (m_select_file == info)
		flags = flags | ImGuiTreeNodeFlags_Selected;

	if (CustomTreeNode(info->mPath.c_str(), flags, [&](bool hoverd, bool clicked)
	{
		bool opened = info == m_select_file;
		LString name = LString::Format("{0} {1}", icon, info->mName.c_str());
		if (ImGui::BeginDragDropSource())
		{
			ImGui::Text(info->mName.c_str());
			ImGui::SetDragDropPayload("LIBRARY", &info, sizeof(LFileInfo*));
			ImGui::EndDragDropSource();
		}
		ImGui::Text(name.c_str());
		if (clicked)
		{
			m_select_file = info;
		}
		const char* id = info->mPath.c_str();
		if (clicked)
		{
			Log("Editor", "123");
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(id);
		}
		if (ImGui::BeginPopupContextItem(id, ImGuiMouseButton_Right))
		{
			ImGui::MenuItem("打开");
			ImGui::MenuItem("删除");
			ImGui::EndPopup();
		}
	}))
	{
		for (auto& f : info->mFolderContents)
		{
			FolderRecursive(&(f.second));
		}
		ImGui::TreePop();
	}
}

void LibraryEditor::OnDrawFolder()
{
	auto& root = sAssetModule->GetResRoot();

	ImGui::BeginGroup();
	ImGui::PushID("FolderPanel");
	FolderRecursive(&root);
	ImGui::PopID();
	ImGui::EndGroup();
}

void LibraryEditor::OnDrawLibrary()
{
	ImGui::BeginGroup();
	if (m_temp_folder)
		m_cur_folder = m_temp_folder;
	m_temp_folder = nullptr;
	auto size = ImGui::GetWindowSize();
	if (m_cur_folder)
	{
		ImGui::Selectable("...");
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			if (m_cur_folder && m_cur_folder->mParent)
			{
				m_cur_folder = m_cur_folder->mParent;
			}
		}
		for (auto& it : m_cur_folder->mFolderContents)
		{
			Item(it.second);
		}
	}
	ImGui::EndGroup();
}

void LibraryEditor::Item(const LFileInfo& info)
{
	// 	render::RHITexture* tex = m_p->GetRHITexture();
	// 	render::DX12Descriptor* desc = m_file_descriptor->As<render::DX12Descriptor>();
	// 	ImGui::Image((void*)desc->GetGpuHandle(0).ptr, ImVec2(100, 100));
}
}
