#pragma once
class ThreadWidget
{
public:
	void OnPaint()
	{
        if (!show_)
            return;
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = { 0.f,0.f };
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin(u8"�߳��б�", &show_, ImGuiWindowFlags_NoCollapse);
        if (ImGui::BeginTable("#threadlist", 4, ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SortMulti | ImGuiTableFlags_Sortable | ImGuiTableFlags_BordersV, ImVec2(0.0f, 0), 0.0f))
        {
            ImGui::TableSetupColumn(u8"�߳�ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, ThreadItem::EInfo::THREAID);
            ImGui::TableSetupColumn(u8"�߳����", ImGuiTableColumnFlags_WidthFixed, 0.0f, ThreadItem::EInfo::STARTADDR);
            ImGui::TableSetupColumn(u8"���ȼ�", ImGuiTableColumnFlags_WidthFixed, 0.0f, ThreadItem::EInfo::PRIORITY);
            ImGui::TableSetupColumn(u8"����ģ��", ImGuiTableColumnFlags_WidthFixed, 0.0f, ThreadItem::EInfo::MODULEPATH);
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
                    // Display a data item
                    ThreadItem* item = &DataSource_[row_n];
                    ImGui::PushID(item->GetThreadId());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Selectable(std::to_string(item->GetThreadId()).c_str(), selected_ == row_n, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        selected_ = row_n;
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::IntegerTohex(item->GetThreadEntry()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", item->GetPrority());
                    ImGui::TableNextColumn();
                    ImGui::Text(utils::conver::string_To_UTF8(item->GetModulePath()).c_str());
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }
        ImGui::End();
	}

    void SetDataSource(uint32_t pid)
    {
        DataSource_.clear();
        ThreadItem::EnumPidThread(pid, DataSource_);
        selected_ = -1;
    }

    void Load()
    {
        show_ = true;
    }
private:

    std::vector<ThreadItem> DataSource_{};
    int selected_ = -1;
    bool show_;
};