#include "xenia/app/library/xex_scanner.h"
#include "third_party/crypto/TinySHA1.hpp"
#include "third_party/crypto/rijndael-alg-fst.h"
#include "xenia/app/library/scanner_utils.h"
#include "xenia/base/logging.h"
#include "xenia/cpu/lzx.h"

namespace xe {
namespace app {

void aes_decrypt_buffer(const uint8_t* session_key, const uint8_t* input_buffer,
                        const size_t input_size, uint8_t* output_buffer,
                        const size_t output_size) {
  XELOGI("AesDecryptBuffer called on input of size %d", input_size);
  uint32_t rk[4 * (MAXNR + 1)];
  uint8_t ivec[16] = {0};
  int32_t Nr = rijndaelKeySetupDec(rk, session_key, 128);
  const uint8_t* ct = input_buffer;
  uint8_t* pt = output_buffer;
  for (size_t n = 0; n < input_size; n += 16, ct += 16, pt += 16) {
    // Decrypt 16 uint8_ts from input -> output.
    rijndaelDecrypt(rk, Nr, ct, pt);
    for (size_t i = 0; i < 16; i++) {
      // XOR with previous.
      pt[i] ^= ivec[i];
      // Set previous.
      ivec[i] = ct[i];
    }
  }
}

void aes_decrypt_inplace(const uint8_t* session_key, const uint8_t* buffer,
                         const size_t size) {
  XELOGI("AesDecryptInplace called on input of size %d", size);
  uint32_t rk[4 * (MAXNR + 1)];
  uint8_t ivec[0x10] = {0};
  int32_t Nr = rijndaelKeySetupDec(rk, session_key, 128);
  for (size_t n = 0; n < size; n += 0x10) {
    uint8_t* in = (uint8_t*)buffer + n;
    uint8_t out[0x10] = {0};
    rijndaelDecrypt(rk, Nr, in, out);
    for (size_t i = 0; i < 0x10; i++) {
      // XOR with previous.
      out[i] ^= ivec[i];
      // Set previous.
      ivec[i] = in[i];
    }

    // Fast copy
    *(size_t*)in = *(size_t*)out;
    *(size_t*)(in + 0x8) = *(size_t*)(out + 0x8);
  }
}

X_STATUS ReadXexImageUncompressed(File* file, XexInfo* info, size_t offset,
                                  uint32_t length, uint8_t*& out_data) {
  XELOGI("Reading uncompressed image");
  size_t file_size = file->entry()->size();
  auto format = info->file_format_info;

  const size_t exe_size = file_size - info->header_size;

  X_STATUS status;
  switch (format->encryption_type) {
    case XEX_ENCRYPTION_NONE: {
      status = Read(file, out_data, info->header_size + offset, length);
      if (status != X_STATUS_SUCCESS) {
        XELOGE("Could not read from file %ls", file->entry()->path().c_str());
        return status;
      }

      XELOGI("Successfully read unencrypted uncompressed image.");
      return X_STATUS_SUCCESS;
    }
    case XEX_ENCRYPTION_NORMAL: {
      status = Read(file, out_data, info->header_size + offset, length);
      if (status != X_STATUS_SUCCESS) {
        XELOGE("Could not read from file %ls", file->entry()->path().c_str());
        return status;
      }

      aes_decrypt_inplace(info->session_key, out_data, length);
      XELOGI("Successfully read encrypted uncompressed image.");
      return X_STATUS_SUCCESS;
    }
    default:
      XELOGI("Could not read image. Unknown encryption type.");
      return X_STATUS_UNSUCCESSFUL;
  }
}

X_STATUS ReadXexImageBasicCompressed(File* file, XexInfo* info, size_t offset,
                                     uint32_t length, uint8_t*& out_data) {
  XELOGI("Reading basic compressed image.");
  auto file_size = file->entry()->size();
  auto format = info->file_format_info;
  auto compression = &format->compression_info.basic;
  auto encryption = format->encryption_type;

  // Find proper block
  uint32_t i;
  uint32_t compressed_position = 0;
  uint32_t uncompressed_position = 0;
  uint32_t block_count = (format->info_size - 8) / 8;
  for (i = 0; i < block_count; i++) {
    const uint32_t data_size = compression->blocks[i].data_size;
    const uint32_t zero_size = compression->blocks[i].zero_size;
    const uint32_t total_size = data_size + zero_size;
    if (uncompressed_position + total_size > offset) break;

    compressed_position += data_size;
    uncompressed_position += total_size;
  }
  XELOGI("Found offset %08x at block %d", compressed_position, i);

  // For some reason the AES IV is screwing up the first 0x10 bytes,
  // so in the meantime, we're just shifting back 0x10 and skipping
  // the garbage data.
  auto block = compression->blocks[i];
  uint32_t block_size = block.data_size + 0x10;
  uint32_t block_address = info->header_size + compressed_position - 0x10;

  uint8_t* data = new uint8_t[file->entry()->size()];
  Read(file, data, block_address, block_size);

  if (encryption == XEX_ENCRYPTION_NORMAL) {
    XELOGI("Decrypting basic compressed image.");
    aes_decrypt_inplace(info->session_key, data, block_size);
  }

  // Get the requested data
  auto remaining_offset = offset - uncompressed_position;
  out_data = (uint8_t*)malloc(length);
  memcpy(out_data, data + remaining_offset + 0x10, length);

  XELOGI("Successfully read basic compressed image.");
  delete[] data;
  return X_STATUS_SUCCESS;
}

X_STATUS ReadXexImageNormalCompressed(File* file, XexInfo* info, size_t offset,
                                      uint32_t length, uint8_t*& out_data) {
  auto encryption_type = info->file_format_info->encryption_type;
  auto uncompressed_length = info->security_info.image_size;
  auto compression_info = info->file_format_info->compression_info.normal;

  sha1::SHA1 s;
  auto in_length = file->entry()->size() - info->header_size;

  uint8_t* in_buffer = new uint8_t[in_length];
  Read(file, in_buffer, info->header_size, in_length);

  if (encryption_type == XEX_ENCRYPTION_NORMAL)
    aes_decrypt_inplace(info->session_key, in_buffer, in_length);

  uint8_t* compressed_buffer = new uint8_t[in_length];

  uint8_t block_calced_digest[0x14];
  auto current_block = &compression_info.first_block;
  auto in_cursor = in_buffer;
  auto out_cursor = compressed_buffer;

  while (current_block->block_size) {
    auto next_ptr = in_cursor + current_block->block_size;
    auto next_block = (xex2_compressed_block_info*)in_cursor;

    s.reset();
    s.processBytes(in_cursor, current_block->block_size);
    s.finalize(block_calced_digest);
    if (memcmp(block_calced_digest, current_block->block_hash, 0x14) != 0) {
      delete[] in_buffer;
      delete[] compressed_buffer;
      return X_STATUS_UNSUCCESSFUL;
    }

    in_cursor += 0x18;

    while (true) {
      auto chunk_size = (in_cursor[0] << 8) | in_cursor[1];
      in_cursor += 2;
      if (!chunk_size) break;

      memcpy(out_cursor, in_cursor, chunk_size);
      in_cursor += chunk_size;
      out_cursor += chunk_size;
    }

    in_cursor = next_ptr;
    current_block = next_block;
  }

  // Decompress
  auto window_size = compression_info.window_size;
  auto decompressed_buffer = new uint8_t[uncompressed_length];
  lzx_decompress(compressed_buffer, out_cursor - compressed_buffer,
                 decompressed_buffer, uncompressed_length, window_size, nullptr,
                 0);

  out_data = (uint8_t*)malloc(length);
  memcpy(out_data, decompressed_buffer + offset, length);

  delete[] in_buffer;
  delete[] compressed_buffer;
  delete[] decompressed_buffer;
  return X_STATUS_SUCCESS;
}

X_STATUS ReadXexImage(File* file, XexInfo* info, size_t offset, uint32_t length,
                      uint8_t*& out_data) {
  auto format = info->file_format_info;

  switch (format->compression_type) {
    case XEX_COMPRESSION_NONE:
      return ReadXexImageUncompressed(file, info, offset, length, out_data);
    case XEX_COMPRESSION_BASIC:
      return ReadXexImageBasicCompressed(file, info, offset, length, out_data);
    case XEX_COMPRESSION_NORMAL:
      return ReadXexImageNormalCompressed(file, info, offset, length, out_data);
    default:
      return X_STATUS_UNSUCCESSFUL;
  }
}

inline void ReadXexAltTitleIds(uint8_t* data, XexInfo* info) {
  uint32_t length = xe::load_and_swap<uint32_t>(data);
  uint32_t count = (length - 0x04) / 0x04;

  info->alt_title_ids_count = count;
  info->alt_title_ids = new uint32_t[length];

  uint8_t* cursor = data + 0x04;
  for (uint32_t i = 0; i < length; i++, cursor += 0x04) {
    info->alt_title_ids[i] = xe::load_and_swap<uint32_t>(cursor);
  }

  XELOGI("%d alternate title ids found", count);
}

inline void ReadXexExecutionInfo(uint8_t* data, XexInfo* info) {
  uint32_t length = sizeof(xex2_opt_execution_info);
  memcpy(&info->execution_info, data, length);
  XELOGI("Read ExecutionInfo. TitleID: %s, MediaID: %s",
         xe::string_util::to_hex_string(info->execution_info.title_id),
         xe::string_util::to_hex_string(info->execution_info.media_id));
}

inline void ReadXexFileFormatInfo(uint8_t* data, XexInfo* info) {
  uint32_t length = xe::load_and_swap<uint32_t>(data);  // TODO
  info->file_format_info = (xex2_opt_file_format_info*)calloc(1, length);
  memcpy(info->file_format_info, data, length);
  XELOGI("Read FileFormatInfo. Encryption: %s, Compression: %s",
         info->file_format_info->encryption_type == XEX_ENCRYPTION_NORMAL
             ? "NORMAL"
             : "NONE",
         info->file_format_info->compression_type == XEX_COMPRESSION_NORMAL
             ? "NORMAL"
             : info->file_format_info->compression_type == XEX_COMPRESSION_BASIC
                   ? "BASIC"
                   : "NONE");
}

inline void ReadXexGameRatings(uint8_t* data, XexInfo* info) {
  uint32_t length = 0xC;
  memcpy(&info->game_ratings, data, 0xC);
  XELOGI("Read GameRatings.");
}

inline void ReadXexMultiDiscMediaIds(uint8_t* data, XexInfo* info) {
  uint32_t entry_size = sizeof(xex2_multi_disc_media_id_t);
  uint32_t length = xe::load_and_swap<uint32_t>(data);
  uint32_t count = (length - 0x04) / 0x10;

  info->multi_disc_media_ids_count = count;
  info->multi_disc_media_ids =
      (xex2_multi_disc_media_id_t*)calloc(count, entry_size);

  uint8_t* cursor = data + 0x04;
  for (uint32_t i = 0; i < count; i++, cursor += 0x10) {
    auto id = &info->multi_disc_media_ids[i];
    memcpy(id->hash, cursor, 0x0C);
    id->media_id = xe::load_and_swap<uint32_t>(cursor + 0x0C);
  }

  XELOGI("Read %d MultiDisc Media IDs. Disc is %d of %d", count,
         info->execution_info.disc_number, info->execution_info.disc_count);
}

inline void ReadXexOriginalPeName(uint8_t* data, XexInfo* info) {
  uint32_t length = xe::load_and_swap<uint32_t>(data);

  info->original_pe_name = (xex2_opt_original_pe_name*)calloc(1, length);
  memcpy(info->original_pe_name, data, length);
  XELOGI("Read OriginalPeName: %s", info->original_pe_name->name);
}

inline void ReadXexResourceInfo(uint8_t* data, XexInfo* info) {
  uint32_t size = sizeof(xex2_opt_resource_info);
  uint32_t length = xe::load_and_swap<uint32_t>(data);
  uint32_t count = (length - 0x04) / 0x10;

  info->resources_count = count;
  info->resources = (xex2_resource*)calloc(count, size);
  memcpy(info->resources, data + 0x4, count * size);
  XELOGI("Read %d resource infos", count);

  /*uint8_t* cursor = data + 0x04;
  for (uint32_t i = 0; i < count; i++, cursor += 0x10) {
    auto resource_info = &info->resources[i];
    memcpy(resource_info->name, cursor, 0x08);
    resource_info->address = xe::load_and_swap<uint32_t>(cursor + 0x08);
    resource_info->size = xe::load_and_swap<uint32_t>(cursor + 0x0C);
  }*/
}

inline void ReadXexOptHeader(xex2_opt_header* entry, uint8_t* data,
                             XexInfo* info) {
  switch (entry->key) {
    case XEX_HEADER_ALTERNATE_TITLE_IDS:
      ReadXexAltTitleIds(data + entry->offset, info);
      break;
    case XEX_HEADER_EXECUTION_INFO:
      ReadXexExecutionInfo(data + entry->offset, info);
      break;
    case XEX_HEADER_FILE_FORMAT_INFO:
      ReadXexFileFormatInfo(data + entry->offset, info);
      break;
    case XEX_HEADER_GAME_RATINGS:
      ReadXexGameRatings(data + entry->offset, info);
      break;
    case XEX_HEADER_MULTIDISC_MEDIA_IDS:
      ReadXexMultiDiscMediaIds(data + entry->offset, info);
      break;
    case XEX_HEADER_RESOURCE_INFO:
      ReadXexResourceInfo(data + entry->offset, info);
      break;
    case XEX_HEADER_ORIGINAL_PE_NAME:
      ReadXexOriginalPeName(data + entry->offset, info);
      break;
    case XEX_HEADER_IMAGE_BASE_ADDRESS:
      info->base_address = entry->value;
      break;
    case XEX_HEADER_SYSTEM_FLAGS:
      info->system_flags =
          (xex2_system_flags)xe::byte_swap<uint32_t>(entry->value.value);
      break;
  }
}

X_STATUS TryKey(const uint8_t* key, uint8_t* magic_block, XexInfo* info,
                File* file) {
  uint8_t decrypted_block[0x10];
  const uint16_t PE_MAGIC = 0x4D5A;
  auto compression_type = info->file_format_info->compression_type;
  auto aes_key = reinterpret_cast<uint8_t*>(info->security_info.aes_key);

  aes_decrypt_buffer(key, aes_key, 0x10, info->session_key, 0x10);
  aes_decrypt_buffer(info->session_key, magic_block, 0x10, decrypted_block,
                     0x10);
  memcpy(info->session_key, info->session_key, 0x10);

  // Decompress if the XEX is lzx compressed
  if (compression_type == XEX_COMPRESSION_NORMAL) {
    uint8_t* data;
    if (XFAILED(ReadXexImageNormalCompressed(file, info, 0, 0x10, data)))
      return X_STATUS_UNSUCCESSFUL;
    memcpy(decrypted_block, data, 0x10);
    delete[] data;
  }

  uint16_t found_magic = xe::load_and_swap<uint16_t>(decrypted_block);
  if (found_magic == PE_MAGIC) return X_STATUS_SUCCESS;

  return X_STATUS_UNSUCCESSFUL;
}

X_STATUS ReadSessionKey(File* file, XexInfo* info) {
  const uint16_t PE_MAGIC = 0x4D5A;
  uint8_t magic_block[0x10];
  Read(file, magic_block, info->header_size, 0x10);

  // Check if the XEX is already decrypted.
  // If decrypted with a third party tool like xextool, the encryption flag
  // is not changed, but the data is decrypted.
  uint16_t found_magic = xe::load_and_swap<uint16_t>(magic_block);
  if (found_magic == PE_MAGIC) {
    info->file_format_info->encryption_type = XEX_ENCRYPTION_NONE;
    return X_STATUS_SUCCESS;
  }

  // XEX is encrypted, derive the session key.
  if (XFAILED(TryKey(xex2_retail_key, magic_block, info, file)))
    if (XFAILED(TryKey(xex2_devkit_key, magic_block, info, file))) {
      return X_STATUS_SUCCESS;
    }

  return X_STATUS_SUCCESS;
}

X_STATUS ReadXexHeaderSecurityInfo(File* file, XexInfo* info) {
  const uint32_t length = 0x180;
  uint8_t data[length];
  Read(file, data, info->security_offset, length);

  memcpy(&info->security_info, data, sizeof(xex2_security_info));

  // XEX is still encrypted. Derive the session key.
  if (XFAILED(ReadSessionKey(file, info))) {
    return X_STATUS_UNSUCCESSFUL;
  }

  return X_STATUS_SUCCESS;
}

X_STATUS ReadXexHeaderSectionInfo(File* file, XexInfo* info) {
  uint32_t offset = info->security_offset;
  uint32_t count = Read<uint32_t>(file, offset);
  uint32_t size = sizeof(xex2_page_descriptor);
  uint32_t length = count * size;
  uint8_t* data = new uint8_t[length];

  Read(file, data, offset, length);

  info->page_descriptors_count = count;
  info->page_descriptors = (xex2_page_descriptor*)calloc(count, size);

  if (!info->page_descriptors || !count) {
    XELOGE("No xex page descriptors are present");

    delete[] data;
    return X_STATUS_UNSUCCESSFUL;
  }

  uint8_t* cursor = data + 0x04;
  for (uint32_t i = 0; i < count; i++) {
    auto section = &info->page_descriptors[i];

    section->value = xe::load<uint32_t>(cursor + 0x00);
    memcpy(section->data_digest, cursor + 0x04, sizeof(section->data_digest));
    cursor += 0x04 + sizeof(section->data_digest);
  }

  XELOGI("Section info successfully read. %d page descriptors found.", count);
  delete[] data;
  return X_STATUS_SUCCESS;
}

X_STATUS ReadXexHeader(File* file, XexInfo* info) {
  uint32_t header_size = Read<uint32_t>(file, 0x8);
  uint8_t* data = new uint8_t[header_size];
  Read(file, data, 0x0, header_size);

  // Read Main Header Data
  info->module_flags =
      (xex2_module_flags)xe::load_and_swap<uint32_t>(data + 0x04);
  info->header_size = xe::load_and_swap<uint32_t>(data + 0x08);
  info->security_offset = xe::load_and_swap<uint32_t>(data + 0x10);
  info->header_count = xe::load_and_swap<uint32_t>(data + 0x14);

  // Read Optional Headers
  uint8_t* cursor = data + 0x18;
  for (uint32_t i = 0; i < info->header_count; i++, cursor += 0x8) {
    auto entry = reinterpret_cast<xex2_opt_header*>(cursor);
    ReadXexOptHeader(entry, data, info);
  }

  if (XFAILED(ReadXexHeaderSecurityInfo(file, info))) {
    XELOGE("Could not read xex security info");
    delete[] data;
    return X_STATUS_UNSUCCESSFUL;
  }
  if (XFAILED(ReadXexHeaderSectionInfo(file, info))) {
    XELOGE("Could not read xex section info");
    delete[] data;
    return X_STATUS_UNSUCCESSFUL;
  }

  delete[] data;
  return X_STATUS_SUCCESS;
}

X_STATUS ReadXexResources(File* file, XexInfo* info) {
  auto resources = info->resources;
  XELOGI("Reading %d XEX resources.", info->resources_count);

  for (size_t i = 0; i < info->resources_count; i++) {
    auto resource = &resources[i];

    uint32_t title_id = info->execution_info.title_id;
    uint32_t name =
        xe::string_util::from_string<uint32_t>(resource->name, true);
    XELOGI("Found resource: %s", resource->name);

    // Game resources are listed as the TitleID
    if (name == title_id) {
      uint32_t offset = resource->address - info->base_address;
      XELOGI("Found XBDF resource at %08x with size %08x", offset,
             resource->size);

      uint8_t* data;
      if (XFAILED(ReadXexImage(file, info, offset, resource->size, data))) {
        XELOGE("Could not read XBDF resource.");
        delete[] data;
        return X_STATUS_UNSUCCESSFUL;
      }

      auto xbdf_data = XdbfGameData(data, resource->size);

      if (!xbdf_data.is_valid()) {
        delete[] data;
        XELOGE("XBDF data is invalid.");
        return X_STATUS_UNSUCCESSFUL;
      }

      // Extract Game Title
      info->game_title = xbdf_data.title();

      // Extract Game Icon
      auto icon = xbdf_data.icon();
      info->icon_size = icon.size;
      info->icon = (uint8_t*)calloc(1, icon.size);
      memcpy(info->icon, icon.buffer, icon.size);

      // TODO: Extract Achievements

      delete[] data;
      XELOGI("Successfully read XBDF resource. Game title: %s, icon size: %08x",
             info->game_title.c_str(), info->icon_size);
      return X_STATUS_SUCCESS;
    }
  }

  return X_STATUS_SUCCESS;
}

X_STATUS XexScanner::ScanXex(File* file, GameInfo* out_info) {
  if (XFAILED(ReadXexHeader(file, &out_info->xex_info))) {
    XELOGE("ReadXexHeader failed");
    return X_STATUS_UNSUCCESSFUL;
  }
  if (XFAILED(ReadXexResources(file, &out_info->xex_info))) {
    XELOGE("ReadXexResources failed");
    return X_STATUS_UNSUCCESSFUL;
  }

  XELOGI("ScanXex was successful");
  return X_STATUS_SUCCESS;
}

}  // namespace app
}  // namespace xe