#ifndef XENIA_CONFIG_H_
#define XENIA_CONFIG_H_
#include <string>

namespace xe {
namespace config {

void SetupConfig(const std::wstring& config_folder);
void LoadGameConfig(const std::wstring& title_id);
void SaveConfig();

}  // namespace config
}  // namespace xe

#endif  // XENIA_CONFIG_H_
