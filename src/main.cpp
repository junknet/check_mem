#include <algorithm>
#include <android/log.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define adebug(...) __android_log_print(ANDROID_LOG_DEBUG, "dbi", __VA_ARGS__)

std::vector<int> buildBadCharTable(const std::string &pattern) {
  std::vector<int> badCharTable(256, -1);
  int patternLength = static_cast<int>(pattern.length());

  for (int i = 0; i < patternLength; ++i) {
    badCharTable[static_cast<int>(pattern[i])] = i;
  }

  return badCharTable;
}

std::vector<int> searchBoyerMoore(const char *text, int textLength,
                                  const std::string &pattern) {
  std::vector<int> occurrences;
  int patternLength = static_cast<int>(pattern.length());
  std::vector<int> badCharTable = buildBadCharTable(pattern);

  int s = 0; // s is the shift of the pattern with respect to the text
  while (s <= textLength - patternLength) {
    int j = patternLength - 1;

    while (j >= 0 && pattern[j] == text[s + j]) {
      --j;
    }

    if (j < 0) {
      // Pattern found at index s
      occurrences.push_back(s);
      char *ptr = (char *)(text + s);

      // printf("%p : %s\n", ptr, ptr - 1);
      adebug("%p : %s\n", ptr, ptr - 1);
      s += (s + patternLength < textLength)
               ? patternLength - badCharTable[text[s + patternLength]]
               : 1;
    } else {
      s += std::max(1, j - badCharTable[text[s + j]]);
    }
  }

  return occurrences;
}

extern "C" void find_module(char *pattern) {

  std::ifstream maps_file("/proc/self/maps");
  if (!maps_file.is_open()) {
    std::cerr << "Failed to open /proc/self/maps file." << std::endl;
  }

  // 逐行读取文件内容
  std::string line;
  while (std::getline(maps_file, line)) {
    // 使用字符串流分割每行内容
    std::istringstream iss(line);
    std::string address_range, permissions;

    // 读取地址范围和权限信息
    iss >> address_range >> permissions;

    if (permissions != "rw-p") {
      continue;
    }

    // 分割地址范围字符串
    size_t dash_pos = address_range.find('-');
    std::string start_addr_str = address_range.substr(0, dash_pos);
    std::string end_addr_str = address_range.substr(dash_pos + 1);

    // 将地址字符串转换为整数
    uint64_t start_addr = std::stoull(start_addr_str, nullptr, 16);
    uint64_t end_addr = std::stoull(end_addr_str, nullptr, 16);

    searchBoyerMoore((char *)start_addr, end_addr - start_addr, pattern);

    // 打印解析结果
    // std::cout << "Start Address: " << std::hex << start_addr
    //           << ", End Address: " << std::hex << end_addr
    //           << ", Permissions: " << permissions << std::endl;
  }
  // 关闭文件
  maps_file.close();
  adebug("search ok");
}
