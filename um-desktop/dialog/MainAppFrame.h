#pragma once
#include "../ui/ui.h"

#include <wx/dnd.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include <um-crypto/types.h>

enum class FileProcessStatus {
  kNotProcessed = 0,
  kProcessing,
  kProcessedOk,
  kProcessFailed,
  kProcessNotEncrypted,
  kProcessNotSupported,
};

struct FileEntry {
  FileProcessStatus status;
  umc::Path file_path;
  long index;
  long process_time_ms;
  wxString error;
};

class MainAppFrame;

class MainAppDropTarget : public wxFileDropTarget {
 public:
  MainAppDropTarget(MainAppFrame* app_frame)
      : wxFileDropTarget(), app_frame_(app_frame) {}
  bool OnDropFiles(wxCoord x,
                   wxCoord y,
                   const wxArrayString& filenames) override;

 private:
  MainAppFrame* app_frame_;
};

class MainAppFrame : public umd::ui_base::uiMainAppFrame {
 public:
  MainAppFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
  void SetDecryptionInProgress(bool in_progress);

 protected:
  std::atomic<unsigned int> file_entry_process_idx_ = 0;
  std::atomic<unsigned int> file_entry_complete_count_ = 0;
  std::vector<std::shared_ptr<FileEntry>> file_entries_;

  void uiMainAppFrameOnSize(wxSizeEvent& event) override;
  void OnBtnClickOptions(wxCommandEvent& event) override;
  void OnButtonClick_AddFile(wxCommandEvent& event) override;
  void OnButtonClick_AddDirectory(wxCommandEvent& event) override;
  void OnButtonClick_ClearLogs(wxCommandEvent& event) override;
  void OnButtonClick_ProcessFiles(wxCommandEvent& event) override;
  void HandleAddFilesToQueue(const wxArrayString& filenames);

  std::mutex update_status_mutex_;
  void UpdateFileStatus(int idx, FileProcessStatus status);
  void ProcessNextFile();
  void OnProcessSingleFileComplete();

 private:
  friend class MainAppDropTarget;
};
