#pragma once

class ProcessWidget
{
public:
	void OnPaint()
	{
        if (DataSource_.size() == 0)
        {
            ProcessItem::EnumCurtAllProcess(DataSource_);
        }
        if (ImGui::BeginTable("#processlist", 9, ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_Sortable | ImGuiTableFlags_BordersV))
        {
            ImGui::TableSetupColumn("##icon", ImGuiTableColumnFlags_NoSort, 0.0f);
            ImGui::TableSetupColumn(u8"����ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, ProcessItem::EInfo::PID);
            ImGui::TableSetupColumn(u8"������", ImGuiTableColumnFlags_WidthFixed, 0.0f, ProcessItem::EInfo::NAME);
            ImGui::TableSetupColumn(u8"������ID", ImGuiTableColumnFlags_WidthFixed, 0.0f, ProcessItem::EInfo::PPID);
            ImGui::TableSetupColumn(u8"����ʱ��", ImGuiTableColumnFlags_WidthFixed, 0.0f, ProcessItem::EInfo::STARTUPTIME);
            ImGui::TableSetupColumn(u8"����", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn(u8"�ļ�����", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn(u8"�ļ��汾", ImGuiTableColumnFlags_WidthFixed, 0.0f);
            ImGui::TableSetupColumn(u8"����·��", ImGuiTableColumnFlags_WidthFixed, 0.0f, ProcessItem::EInfo::FULLPATH);
            ImGui::TableHeadersRow();
            if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
            {
                if (sorts_specs->SpecsDirty)
                {
                    ProcessItem::SetSortSpecs(sorts_specs); // Store in variable accessible by the sort function.
                    if (DataSource_.size() > 1)
                        qsort(&DataSource_[0], (size_t)DataSource_.size(), sizeof(DataSource_[0]), ProcessItem::CompareWithSortSpecs);
                    ProcessItem::SetSortSpecs(NULL);
                    sorts_specs->SpecsDirty = false;
                }
            }
            ImGuiListClipper clipper;
            clipper.Begin(DataSource_.size());
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    ProcessItem* item = &DataSource_[row_n];
                    ImGui::PushID(item->GetPid());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (item->GetIcon())
                        ImGui::Image(item->GetIcon(), ImVec2(16, 16));
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(std::to_string(item->GetPid()).c_str(), select_ == row_n, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        select_ = row_n;
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetName().append(item->IsWow64() ? "*32" : "")).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", item->GetPPid());
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetStartUpTime()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetDecription()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetCompanyName()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetFileVersion()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetFullPath()).c_str());
                    ImGui::PopID();
                }
            }

            if (select_ != -1 && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseClicked(1))
                ImGui::OpenPopup("process_option");

            if (ImGui::BeginPopup("process_option"))
            {
                switch (int s = render::get_instasnce()->DrawItemBlock({ u8"ˢ�½���" }))
                {
                    case 0:
                    {
                        DataSource_.clear();
                        break;
                    }
                }
                ImGui::Separator();
                switch (int s = render::get_instasnce()->DrawItemBlock({ u8"ö�ٴ���",u8"�ڴ��б�",u8"�߳��б�",u8"���̾��",u8"�쳣�ص�" }))
                {
                    case 1:
                    {
                        config::process::memory::bShow = true;
                        config::process::memory::pid = DataSource_[select_].GetPid();
                        break;
                    }
                    case 2:
                    {
                        config::process::thread::bShow = true;
                        config::process::thread::pid = DataSource_[select_].GetPid();
                        break;
                    }
                    case 4:
                    {
                        config::process::veh::bShow = true;
                        config::process::veh::pid = DataSource_[select_].GetPid();;
                        break;
                    }
                }
                ImGui::Separator();
                switch (int s = render::get_instasnce()->DrawItemBlock({ u8"��������",u8"���ؽ���",u8"��������" }))
                {
                    case 0:
                    {
                        HANDLE handLe = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DataSource_[select_].GetPid());
                        if (handLe)
                        {
                            TerminateProcess(handLe, 0);
                            CloseHandle(handLe);
                        }
                        break;
                    }
                    case 2:
                    {
                        utils::file::OpenFilePropertyDlg(DataSource_[select_].GetFullPath().c_str());
                        break;
                    }
                }
                ImGui::Separator();
                switch (int s = render::get_instasnce()->DrawItemBlock({ u8"ת���ڴ�",u8"�����ļ�",u8"�ļ���λ" }))
                {
                    case 0:
                    {

                        break;
                    }
                    case 2:
                    {
                        utils::file::OpenFolderAndSelectFile(DataSource_[select_].GetFullPath().c_str());
                        break;
                    }
                }
                ImGui::Separator();
                //u8"ע��Dll",
                if (ImGui::BeginMenu(u8"ע��Dll"))
                {

                    int s = render::get_instasnce()->DrawItemBlock({ u8"Զ�߳�ע��",u8"�ڴ�ע��",u8"APCע��" });
                    if (s >= 0)
                    {
                        std::string dllPath{};
                        dllPath = utils::file::openFileDlg();
                        if (dllPath.empty())
                        {
                            MessageBoxA(NULL, "Dll·������ע��ʧ��", "pjark", NULL);
                        }
                        else
                        {
                            switch (s)
                            {
                                case 0:
                                {

                                    if (Mem::RemoteInjectDLL(DataSource_[select_].GetPid(), dllPath.c_str()))
                                    {
                                        MessageBoxA(NULL, "ע��ɹ�!", "pjark", NULL);
                                    }
                                    else
                                    {
                                        MessageBoxA(NULL, "ע��ʧ��!", "pjark", NULL);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
                switch (int s = render::get_instasnce()->DrawItemBlock({ u8"���Բ��" }))
                {
                    case 0:
                    {
                        static const char* plugin_name = DataSource_[select_].IsWow64() ? "pjveh_32.dll" : "pjveh_64.dll";
                        static uint64_t  Offset_Dispatch = 0;
                        static char plugin_path[MAX_PATH]{};

                        bool can = true;
                        HMODULE plugin_base = NULL;
                        //��ȡ����ص�ƫ��
                        if (Offset_Dispatch <= 0)
                        {
                            HMODULE plugin_base = GetModuleHandleA(plugin_name);
                            if (plugin_base == NULL)
                            {
                                plugin_base = LoadLibraryA(plugin_name);
                                if (!plugin_base)
                                {
                                    MessageBoxA(NULL, "����ļ�����ʧ��,�����ļ����Ƿ����!", "pjark", NULL);
                                    break;//��ǰ��¼������ע��Ľ���һ�£���ֹע��
                                }
                            }

                            Offset_Dispatch = (uint64_t)(reinterpret_cast<uint8_t*>(GetProcAddress(plugin_base, "Dispatch")) - (uint8_t*)plugin_base);
                            GetModuleFileNameA(plugin_base, plugin_path, MAX_PATH);
                            FreeLibrary(plugin_base);
                        }

                        //���Ŀ������Ƿ��й�ע����
                        plugin_base = Mem::GetProcessModuleHandle(DataSource_[select_].GetPid(), utils::conver::string_to_wstirng(plugin_name).c_str());
                        if (plugin_base)
                        {
                            //���»ص���ַ
                            config::global::lpPluginDispatch = reinterpret_cast<uint64_t>(plugin_base) + Offset_Dispatch;
                            if (config::global::injectProcess.GetPid() == DataSource_[select_].GetPid())
                            {
                                MessageBoxA(NULL, "Ŀ������Ѿ����ò��,�����ظ�����!", "pjark", NULL);
                                break;//��ǰ��¼������ע��Ľ���һ�£���ֹע��
                            }
                            else
                            {
                                if (config::global::injectProcess.GetPid() <= 0)
                                    config::global::injectProcess = DataSource_[select_];
                                //������һ������
                                do
                                {
                                    //1)�����ȫ���ϵ�
                                    for (int i = 0; i < 4; i++)
                                        config::dbg::Dr->statue = 0; //��������0��OnUpdate���Զ�ɾ���ϵ�
                                    Sleep(500);
                                    //2)�����veh
                                    if (!InvokePluginFunction(config::global::injectProcess.GetPid(), { ECMD::veh_uninstall }))
                                    {
                                        MessageBoxA(NULL, "���ò��ʧ��[1]!", NULL, NULL);
                                        can = false;
                                        break;
                                    }
                                    //3)�����syscall
                                    if (!InvokePluginFunction(config::global::injectProcess.GetPid(), { ECMD::syscallmonitor_uninstall }))
                                    {
                                        MessageBoxA(NULL, "���ò��ʧ��[2]!", NULL, NULL);
                                        can = false;
                                        break;
                                    }
                                    //3)�����pipe
                                    if (!InvokePluginFunction(config::global::injectProcess.GetPid(), { ECMD::pipe_client_close }))
                                    {
                                        MessageBoxA(NULL, "���ò��ʧ��[3]!", NULL, NULL);
                                        can = false;
                                        break;
                                    }
                                    //3)�����plugin
                                    if (!InvokePluginFunction(config::global::injectProcess.GetPid(), { ECMD::plugin_uninstall }))
                                    {
                                        MessageBoxA(NULL, "���ò��ʧ��[4]!", NULL, NULL);
                                        can = false;
                                        break;
                                    }
                                    //�����ɹ�
                                    config::global::injectProcess = ProcessItem();
                                } while (false);
                            }
                        }
                        if (can)
                        {
                            char caption[256]{};
                            sprintf_s(caption, "[%d]%s\n�Ƿ���Ҫ���ò��?", DataSource_[select_].GetPid(), DataSource_[select_].GetName().c_str());
                            if (MessageBoxA(NULL, caption, NULL, MB_OKCANCEL) == IDOK)
                            {
                                do
                                {
                                    Mem::RemoteInjectDLL(DataSource_[select_].GetPid(), plugin_path);
                                    plugin_base = Mem::GetProcessModuleHandle(DataSource_[select_].GetPid(), utils::conver::string_to_wstirng(plugin_name).c_str());
                                    if (!plugin_base)
                                    {
                                        MessageBoxA(NULL, "ע����ʧ��!", "pjark", NULL);
                                        break;
                                    }
                                    if (!config::global::plugin_.init_pipe())
                                    {
                                        MessageBoxA(NULL, "�����ܵ�ʧ��!", "pjark", NULL);
                                        break;
                                    }
                                    config::global::lpPluginDispatch = reinterpret_cast<uint64_t>(plugin_base) + Offset_Dispatch;
                                    if (!InvokePluginFunction(DataSource_[select_].GetPid(), { ECMD::pipe_client_connect }))
                                    {
                                        MessageBoxA(NULL, "���ӹܵ�ʧ��!", NULL, NULL);
                                        break;
                                    }
                                    if (!InvokePluginFunction(DataSource_[select_].GetPid(), { ECMD::veh_init }))
                                    {
                                        MessageBoxA(NULL, "��ʼ�����ʧ��!", NULL, NULL);
                                        break;
                                    }
                                    MessageBoxA(NULL, "�����ʼ���ɹ�!", NULL, NULL);
                                    config::global::injectProcess = DataSource_[select_];
                                } while (false);
                            }
                        }
                        else
                        {
                            MessageBoxA(NULL, "��ʼ�����ʧ��!", NULL, NULL);
                        }
                        break;
                    }
                }
                ImGui::Separator();
                if (ImGui::BeginMenu(u8"����"))
                {
                    switch (int s = render::get_instasnce()->DrawItemBlock({ u8"����ID",u8"������",u8"������ID",u8"����ʱ��"  ,u8"����" ,u8"�ļ�����" ,u8"�ļ��汾",u8"����·��" }))
                    {
                        case 0:
                        {
                            utils::normal::CopyStringToClipboard(std::to_string(DataSource_[select_].GetPid()).c_str());
                            break;
                        }
                        case 1:
                        {
                            utils::normal::CopyStringToClipboard(DataSource_[select_].GetName().c_str());
                            break;
                        }
                        case 2:
                        {
                            utils::normal::CopyStringToClipboard(std::to_string(DataSource_[select_].GetPPid()).c_str());
                            break;
                        }
                        case 3:
                        {
                            utils::normal::CopyStringToClipboard(DataSource_[select_].GetStartUpTime().c_str());
                            break;
                        }
                        case 4:
                        {
                            utils::normal::CopyStringToClipboard(DataSource_[select_].GetDecription().c_str());
                            break;
                        }
                        case 5:
                        {
                            utils::normal::CopyStringToClipboard(DataSource_[select_].GetCompanyName().c_str());
                            break;
                        }
                        case 6:
                        {
                            utils::normal::CopyStringToClipboard(DataSource_[select_].GetFileVersion().c_str());
                            break;
                        }
                        case 7:
                        {
                            utils::normal::CopyStringToClipboard(DataSource_[select_].GetFullPath().c_str());
                            break;
                        }
                    }
                    ImGui::Separator();
                    switch (int s = render::get_instasnce()->DrawItemBlock({ u8"����" ,u8"������" }))
                    {
                        case 0:
                        {
                            char buff[8192]{};
                            sprintf_s(buff, "%d | %s | %d | %s | %s | %s | %s | %s", DataSource_[select_].GetPid(),
                                DataSource_[select_].GetName().c_str(), DataSource_[select_].GetPPid(),
                                DataSource_[select_].GetStartUpTime().c_str(), DataSource_[select_].GetDecription().c_str(),
                                DataSource_[select_].GetCompanyName().c_str(), DataSource_[select_].GetFileVersion().c_str(),
                                DataSource_[select_].GetFullPath().c_str());
                            utils::normal::CopyStringToClipboard(buff);
                            break;
                        }
                        case 1:
                        {
                            std::string ret{};
                            for (auto p : DataSource_)
                            {
                                char buff[8192]{};
                                sprintf_s(buff, "%d | %s | %d | %s | %s | %s | %s | %s\n", p.GetPid(),
                                    p.GetName().c_str(), p.GetPPid(),
                                    p.GetStartUpTime().c_str(), p.GetDecription().c_str(),
                                    p.GetCompanyName().c_str(), p.GetFileVersion().c_str(),
                                    p.GetFullPath().c_str());
                                ret += buff;
                            }
                            utils::normal::CopyStringToClipboard(ret.c_str());
                            break;
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
            ImGui::EndTable();
        }
	}

    auto GetCurtProcessItem()
    {
        return DataSource_[select_];
    }

private:
    uint32_t select_;
	std::vector<ProcessItem> DataSource_;
};