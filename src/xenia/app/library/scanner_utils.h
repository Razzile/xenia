#ifndef XENIA_APP_SCANNER_UTILS_H_
#define XENIA_APP_SCANNER_UTILS_H_

#include "xenia/base/filesystem.h"
#include "xenia/base/string_util.h"
#include "xenia/kernel/util/xex2_info.h"
#include "xenia/vfs/device.h"
#include "xenia/vfs/devices/disc_image_device.h"
#include "xenia/vfs/devices/host_path_device.h"
#include "xenia/vfs/devices/stfs_container_device.h"
#include "xenia/vfs/file.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

namespace xe {
namespace app {
using std::wstring;
using vfs::Device;
using vfs::File;

enum XGameFormat {
  kUnknown,
  kIso,
  kStfs,
  kXex,
};

typedef xex2_header XexHeader;
typedef xex2_opt_header XexOptHeader;
typedef xex2_game_ratings_t XGameRatings;
typedef xex2_region_flags XGameRegions;
typedef xe_xex2_version_t XGameVersion;

struct XexInfo {
  std::string game_title;
  uint8_t* icon = nullptr;
  size_t icon_size;

  uint32_t* alt_title_ids = nullptr;
  uint32_t alt_title_ids_count;
  uint32_t base_address;
  xex2_opt_execution_info execution_info;
  xex2_opt_file_format_info* file_format_info = nullptr;
  xex2_game_ratings_t game_ratings;
  uint32_t header_count;
  uint32_t header_size;
  xex2_module_flags module_flags;
  xex2_multi_disc_media_id_t* multi_disc_media_ids = nullptr;
  uint32_t multi_disc_media_ids_count;
  xex2_opt_original_pe_name* original_pe_name = nullptr;
  xex2_page_descriptor* page_descriptors = nullptr;
  uint32_t page_descriptors_count;
  xex2_resource* resources = nullptr;
  uint32_t resources_count;
  xex2_security_info security_info;
  uint32_t security_offset;
  uint8_t session_key[0x10];
  xex2_system_flags system_flags;

  ~XexInfo() {
    delete[] icon;
    delete[] alt_title_ids;
    delete file_format_info;
    delete[] multi_disc_media_ids;
    delete original_pe_name;
    delete[] page_descriptors;
    delete[] resources;
  }
};

struct NxeInfo {
  std::string game_title;
  uint8_t* icon = nullptr;
  size_t icon_size;
  uint8_t* nxe_background_image = nullptr;  // TODO
  size_t nxe_background_image_size;         // TODO
  uint8_t* nxe_slot_image = nullptr;        // TODO
  size_t nxe_slot_image_size;               // TODO

  NxeInfo() = default;

  ~NxeInfo() {
    delete[] icon;
    delete[] nxe_background_image;
    delete[] nxe_slot_image;
  }
};

struct GameInfo {
  XGameFormat format;
  wstring path;
  wstring filename;
  NxeInfo nxe_info;
  XexInfo xex_info;

  GameInfo() = default;
};

inline const bool CompareCaseInsensitive(const wstring& left,
                                         const wstring& right) {
  // Copy strings for transform
  wstring a(left);
  wstring b(right);

  std::transform(a.begin(), a.end(), a.begin(), toupper);
  std::transform(b.begin(), b.end(), b.begin(), toupper);

  return a == b;
}

inline const wstring GetFileExtension(const wstring& path) {
  wstring path_(path);

  // Chop off all but filename
  size_t index = path_.find_last_of(xe::kWPathSeparator);
  if (index != wstring::npos) {
    path_ = path.substr(index);
  }

  // Find the index of the file extension
  index = path_.find_last_of('.');
  if (index == wstring::npos) {
    return L"";
  }

  return path_.substr(index + 1);
}

inline const wstring GetFileName(const wstring& path) {
  size_t index = path.find_last_of(xe::kWPathSeparator);
  if (index == wstring::npos) {
    return path;
  }

  return path.substr(index + 1);
}

inline const wstring GetParentDirectory(const wstring& path) {
  auto index = path.find_last_of(xe::kWPathSeparator);
  return path.substr(0, index);
}

inline X_STATUS Read(File* file, void* buffer, size_t offset = 0,
                     size_t length = 0) {
  if (!buffer) {
    return X_STATUS_UNSUCCESSFUL;
  }

  if (!length) {
    length = file->entry()->size();
  }

  size_t bytes_read;
  file->ReadSync(buffer, length, offset, &bytes_read);

  if (length == bytes_read) {
    return X_STATUS_SUCCESS;
  }
  return X_STATUS_UNSUCCESSFUL;
}

inline void ReadStfsMagic(const wstring& path, char out[4]) {
  using namespace xe::filesystem;
  auto file = OpenFile(path, "r");
  if (file) {
    size_t read = 0;
    fread(out, 4, 1, file);
    fclose(file);
  }
}

inline const XGameFormat ResolveFormat(const wstring& path) {
  const std::wstring& extension = GetFileExtension(path);

  if (CompareCaseInsensitive(extension, L"iso")) return XGameFormat::kIso;
  if (CompareCaseInsensitive(extension, L"xex")) return XGameFormat::kXex;

  // STFS Container
  char magic[4];
  if (extension.length() == 0) {
    ReadStfsMagic(path, magic);

    if (memcmp(&magic, "LIVE", 4) == 0 || memcmp(&magic, "CON ", 4) == 0 ||
        memcmp(&magic, "PIRS", 4) == 0)
      return XGameFormat::kStfs;
  }

  return XGameFormat::kUnknown;
}

inline std::unique_ptr<Device> CreateDevice(const wstring& path) {
  std::string mount_path = "\\SCAN";
  XGameFormat format = ResolveFormat(path);

  switch (format) {
    case XGameFormat::kIso:
      return std::make_unique<vfs::DiscImageDevice>(mount_path, path);
    case XGameFormat::kXex:
      return std::make_unique<vfs::HostPathDevice>(
          mount_path, GetParentDirectory(path), true);
    case XGameFormat::kStfs:
      return std::make_unique<vfs::StfsContainerDevice>(mount_path, path);
    default:
      return nullptr;
  }
}

template <typename T>
inline T Read(File* file, size_t offset) {
  uint8_t data[sizeof(T)];
  Read(file, data, offset, sizeof(T));
  T swapped = xe::load_and_swap<T>(data);

  return swapped;
}

}  // namespace app
}  // namespace xe

#endif  // !define XENIA_APP_SCANNER_UTILS_H_
