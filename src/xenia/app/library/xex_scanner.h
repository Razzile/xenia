#ifndef XENIA_APP_XEX_SCANNER_H_
#define XENIA_APP_XEX_SCANNER_H_

#include "xenia/kernel/util/xdbf_utils.h"
#include "xenia/kernel/util/xex2_info.h"
#include "xenia/vfs/file.h"

#include <string>

namespace xe {
namespace app {

using kernel::util::XdbfGameData;
using vfs::File;

typedef xex2_header XexHeader;
typedef xex2_opt_header XexOptHeader;
typedef xex2_game_ratings_t XGameRatings;
typedef xex2_region_flags XGameRegions;
typedef xe_xex2_version_t XGameVersion;

static const uint8_t xex2_retail_key[16] = {0x20, 0xB1, 0x85, 0xA5, 0x9D, 0x28,
                                            0xFD, 0xC3, 0x40, 0x58, 0x3F, 0xBB,
                                            0x08, 0x96, 0xBF, 0x91};
static const uint8_t xex2_devkit_key[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};

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

  XexInfo() = default;
  XexInfo(const XexInfo& other) = delete;
  XexInfo& operator=(const XexInfo& other) = delete;
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

class XexScanner {
 public:
  static X_STATUS ScanXex(File* xex_file, XexInfo* out_info);
};

}  // namespace app
}  // namespace xe

#endif