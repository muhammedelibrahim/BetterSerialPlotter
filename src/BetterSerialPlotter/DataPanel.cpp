#include <BetterSerialPlotter/DataPanel.hpp>
#include <BetterSerialPlotter/BSP.hpp>
#include <iostream>

namespace bsp{

DataPanel::DataPanel(BSP* gui_): Widget(gui_){}

void DataPanel::render(){
    constexpr ImGuiWindowFlags padding_flag = ImGuiWindowFlags_AlwaysUseWindowPadding;

    ImGui::BeginChild("IncomingData", ImVec2(200, -1), false, padding_flag);
    ImGui::Text("Incoming Data");
    ImGui::Separator();
    if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_Resizable)) {
        if(gui->serial_manager.baud_status){
            for (int i = 0; i < gui->all_data.size(); ++i) {
                if(!gui->all_data[i].Data.empty()){
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImPlot::ItemIcon(gui->get_color(gui->all_data[i].identifier)); ImGui::SameLine();
                    ImGui::Selectable((gui->get_name(gui->all_data[i].identifier) + "##" + std::to_string(i)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery)) {
                        ImGui::SetDragDropPayload("DND_PLOT", &i, sizeof(int));
                        ImGui::TextUnformatted(gui->get_name(gui->all_data[i].identifier).c_str());
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginPopupContextItem()){
                        static char name[24];
                        if(i != editing_num){
                            editing_num = i;
                            strcpy(name,gui->get_name(gui->all_data[i].identifier).c_str());
                        }
                        ImGui::Text("Edit name:");
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120);
                        ImGui::InputText(("##edit" + std::to_string(i)).c_str(), name, IM_ARRAYSIZE(name));
                        ImGui::PopItemWidth();
                        ImGui::Text("Edit color:");
                        ImGui::SameLine();
                        ImGui::ColorEdit4("##Color", (float*)&gui->all_data_info[gui->all_data[i].identifier].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                        ImGui::Text("Plots Active:");
                        ImGui::Separator();
                        for (auto &plot : gui->plot_monitor.all_plots){
                            auto curr_identifier = gui->all_data[i].identifier;
                            if(ImGui::BeginMenu(plot.name.c_str())){
                                for (auto y = 0; y < 2; y++){
                                    char menu_item_name[10];
                                    sprintf(menu_item_name, "Y-Axis %i", y);
                                    bool is_axis = plot.y_axis.count(curr_identifier) ? plot.y_axis[curr_identifier] == y : false;
                                    if(ImGui::MenuItem(menu_item_name, 0, is_axis)){
                                        if (is_axis) plot.remove_identifier(curr_identifier);
                                        else plot.add_identifier(curr_identifier,y);
                                    }
                                }
                                // add to x axis
                                bool is_x_axis = plot.x_axis == curr_identifier;
                                if(ImGui::MenuItem("X-Axis", 0, is_x_axis)){
                                    if (is_x_axis) {
                                        plot.x_axis = -1;
                                        plot.other_x_axis = false;
                                    }
                                    else {
                                        plot.x_axis = curr_identifier;
                                        plot.other_x_axis = true;
                                    }
                                }
                                ImGui::EndMenu();
                            }
                        }
                        
                        if (ImGui::Button("Save") || (ImGui::IsKeyPressed(257)) || (ImGui::IsKeyPressed(335))){ // would change for mac/linux
                            gui->all_data_info[gui->all_data[i].identifier].set_name(name);
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::TableNextColumn();
                    char buff[16];
                    sprintf(buff, "%4.3f", gui->all_data[i].get_back().y);
                    ImGui::Text(buff);
                }
            }
        }   
        ImGui::EndTable();
    }
    
    ImGui::EndChild();
    // std::cout << "end data panel\n";
}
void MainWindow::setupInputField() {
    auto* inputLabel = new QLabel("Send to Serial:", this);
    auto* inputField = new QLineEdit(this);
    auto* sendButton = new QPushButton("Send", this);

    // Layout
    auto* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(inputField);
    inputLayout->addWidget(sendButton);

    mainLayout->addLayout(inputLayout);

    // Connect button to sendData
    connect(sendButton, &QPushButton::clicked, [this, inputField]() {
        std::string input = inputField->text().toStdString();
        serialManager.sendData(input);  // Call the new sendData method
    });
}

} // namespace bsp
