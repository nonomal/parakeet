#include "MainAppFrame.h"

#include "../utils/audio_decryptor.h"
#include "../utils/threading.h"
#include "OptionsDialog.h"

#include <wx/filedlg.h>
#include <wx/propgrid/propgrid.h>
#include <wx/wfstream.h>

#include <boost/chrono.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/thread/thread.hpp>

#include <functional>

const int state_width = 80;
const int kThreadCount = 4;

MainAppFrame::MainAppFrame(wxWindow* parent, wxWindowID id)
    : uiMainAppFrame(parent, id) {
  m_decryptLogs->InsertColumn(0, _("State"), wxLIST_FORMAT_LEFT, state_width);
  m_decryptLogs->InsertColumn(1, _("Path"), wxLIST_FORMAT_LEFT, 200);
}

void MainAppFrame::uiMainAppFrameOnSize(wxSizeEvent& event) {
  event.Skip();

  int w, h;
  this->m_decryptLogs->GetSize(&w, &h);
  this->m_decryptLogs->SetColumnWidth(1, w - state_width);
};

void MainAppFrame::OnBtnClickOptions(wxCommandEvent& event) {
  auto optionsDialog = new OptionsDialog(this);
  optionsDialog->ShowModal();
  optionsDialog->Destroy();
}

void MainAppFrame::SetInProcess(bool in_process) {
  m_btnClearLogs->Enable(!in_process);
}

void MainAppFrame::OnButtonClick_AddFile(wxCommandEvent& event) {
  event.Skip();

  wxFileDialog openFileDialog(
      this, _("Open encrypted music files"), "", "",
      "QMCv2 Files (*.mgg;*.mflac)|*.mgg;*.mgg0;*.mgg1;*.mflac;*.mflac0"
      "|All files (*.*)|*",
      wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;  // the user changed idea...

  wxArrayString paths;
  openFileDialog.GetPaths(paths);
  auto len = paths.GetCount();
  for (int i = 0; i < len; i++) {
    wxListItem new_item;
    new_item.SetText("-");
    new_item.SetId(m_decryptLogs->GetItemCount());

    auto rowIndex = m_decryptLogs->InsertItem(new_item);
    file_entries_.push_back(std::make_shared<FileEntry>(FileEntry{
        FileProcessStatus::kNotProcessed,
        paths.Item(i),
        rowIndex,
    }));
    m_decryptLogs->SetItem(rowIndex, 1, paths.Item(i));
  }
}

void MainAppFrame::OnButtonClick_AddDirectory(wxCommandEvent& event) {
  event.Skip();
}

void MainAppFrame::OnButtonClick_ClearLogs(wxCommandEvent& event) {
  m_decryptLogs->ClearAll();
  file_entries_.clear();
}

void MainAppFrame::OnButtonClick_ProcessFiles(wxCommandEvent& event) {
  SetInProcess(true);

  const int len = file_entries_.size() - file_entry_complete_count_;
  if (len == 0) {
    SetInProcess(false);
    return;
  }

  for (int i = len - 1; i >= 0; i--) {
    umd::io_service.post([this]() { this->ProcessNextFile(); });
  }
}

void MainAppFrame::UpdateFileStatus(int idx, FileProcessStatus status) {
  std::lock_guard<std::mutex> guard(update_status_mutex_);

  auto entry = file_entries_.at(idx);
  entry->status = status;

  wxString status_text("?");

  switch (status) {
    case FileProcessStatus::kNotProcessed:
      status_text = _("---");
      break;
    case FileProcessStatus::kProcessedOk:
      status_text = _("OK");
      break;
    case FileProcessStatus::kProcessFailed:
      status_text = _("FAIL");
      break;
    case FileProcessStatus::kProcessing:
      status_text = _("...");
      break;
  }

  m_decryptLogs->SetItem(entry->index, 0, status_text);
}

void MainAppFrame::ProcessNextFile() {
  auto current_index = file_entry_process_idx_.fetch_add(1);
  auto entry = file_entries_.at(current_index);

  UpdateFileStatus(current_index, FileProcessStatus::kProcessing);

  AudioDecryptor decryptor;
  decryptor.Open(boost::nowide::narrow(entry->file_path.t_str()));

  auto encryption = decryptor.SniffEncryption();
  if (encryption == EncryptionType::kNotEncrypted) {
    UpdateFileStatus(current_index, FileProcessStatus::kProcessNotSupported);
  } else if (encryption == EncryptionType::kUnsupported) {
    UpdateFileStatus(current_index, FileProcessStatus::kProcessNotSupported);
  } else {
    // TODO: show encryption type
    if (decryptor.DecryptAudioFile()) {
      UpdateFileStatus(current_index, FileProcessStatus::kProcessedOk);
    } else {
      UpdateFileStatus(current_index, FileProcessStatus::kProcessFailed);
    }
  }

  OnProcessSingleFileComplete();
}

void MainAppFrame::OnProcessSingleFileComplete() {
  int completion_count = file_entry_complete_count_.fetch_add(1) + 1;
  if (completion_count == file_entries_.size()) {
    SetInProcess(false);
  }
}
