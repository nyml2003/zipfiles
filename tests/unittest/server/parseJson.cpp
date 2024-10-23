#include <json/json.h>
#include <iostream>
#include <sstream>
#include <string>

void testParseMultipleJsons(const std::string& jsonString) {
  std::istringstream stream(jsonString);
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errs;

  std::cout << "Opening stream: " << stream.tellg() << std::endl;

  while (!stream.eof()) {
    if (Json::parseFromStream(builder, stream, &root, &errs)) {
      std::cout << "Parsed JSON object: " << root << std::endl;
      std::cout << "Remaining stream: " << stream.tellg() << std::endl;
    } else if (!errs.empty()) {
      std::cout << "Failed to parse JSON object: " << errs << std::endl;
      std::cout << "Next stream: " << stream.get() << std::endl;
      errs.clear();  // Clear the error message for the next iteration

    //   exit(0);
    } else {
      break;  // No more JSON objects to parse
    }
  }
}

int main() {
  std::string multipleJsons = R"(
        {"name": "Alice", "age": 30}
        {"name": "Bob", "age": 25}
        {"name": "Charlie", "age": 35}
    )";

  testParseMultipleJsons(multipleJsons);

  return 0;
}
