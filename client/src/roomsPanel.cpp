#include <client/roomsPanel.h>
#include <client/mainWidget.h>
#include <client/wsClient.h>
#include <common/utils/limits.h>

enum { ID_JOIN = wxID_HIGHEST+20, ID_CREATE, ID_LOGOUT };

wxBEGIN_EVENT_TABLE(RoomsPanel, wxPanel)
    EVT_BUTTON(ID_JOIN, RoomsPanel::OnJoin)
    EVT_BUTTON(ID_CREATE, RoomsPanel::OnCreate)
    EVT_BUTTON(ID_LOGOUT, RoomsPanel::OnLogout)
wxEND_EVENT_TABLE()

RoomsPanel::RoomsPanel(MainWidget* parent) : wxPanel(parent), mainWin(parent) {
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Rooms:"), 0, wxALL, 5);
    roomList = new wxListBox(this, wxID_ANY);
    sizer->Add(roomList, 1, wxALL | wxEXPAND, 5);
    // Join and Create sizer
    auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    joinButton = new wxButton(this, ID_JOIN, "Join");
    createButton = new wxButton(this, ID_CREATE, "Create");
    btnSizer->Add(joinButton, 1, wxALL, 5);
    btnSizer->Add(createButton, 1, wxALL, 5);
    sizer->Add(btnSizer, 0, wxALIGN_CENTER);
    // Logout button
    sizer->AddStretchSpacer();
    logoutButton = new wxButton(this, ID_LOGOUT, "Logout");
    sizer->Add(logoutButton, 0, wxALL | wxALIGN_CENTER, 10);
    SetSizer(sizer);
}

void RoomsPanel::UpdateRoomList(const std::vector<Room*>& rooms) {
    roomList->Clear();
    for (auto* room : rooms){
        roomList->Append(room->room_name, room);
    }
}

void RoomsPanel::AddRoom(Room* room) {
    roomList->Append(room->room_name, room);
}

void RoomsPanel::RemoveRoom(uint32_t room_id) {
    for (int i = 0; i < roomList->GetCount(); ++i) {
        Room* roomData = dynamic_cast<Room*>(roomList->GetClientObject(i));
        if (roomData && roomData->room_id == room_id) {
            roomList->Delete(i);
            break;
        }
    }
}

void RoomsPanel::OnJoin(wxCommandEvent &)
{
    int sel = roomList->GetSelection();
    if(sel != wxNOT_FOUND) {
        Room* roomData = dynamic_cast<Room*>(roomList->GetClientObject(sel));
        if (roomData){
            mainWin->wsClient->joinRoom(roomData->room_id);
        }
    }
}

void RoomsPanel::OnCreate(wxCommandEvent&) {
    wxTextEntryDialog dlg(this, "Room name?", "Create Room");
    dlg.SetMaxLength(limits::MAX_ROOMNAME_LENGTH);
    if(dlg.ShowModal() == wxID_OK) {
        wxString roomName = dlg.GetValue();
        if (roomName.IsEmpty()) {
            wxMessageBox("Room name cannot be empty!", "Error", wxOK | wxICON_ERROR);
            return;
        }
        mainWin->wsClient->createRoom(roomName.utf8_string());
    }
}

void RoomsPanel::OnLogout(wxCommandEvent &) {
    mainWin->wsClient->logout();
}
