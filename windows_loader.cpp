#include <cstdio>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>

static const char* defStartString = "/* End of boilerplate cgo prologue.  */";
static const char* defEndString = "#ifdef __cplusplus\n}\n#endif";

typedef struct {
  std::string name, returnType;
  std::vector<std::string> argTypes;
} FunctionDefinition;

static std::string rewriteFunctionDefinitions(
  std::vector<FunctionDefinition>& defs
);
static std::vector<FunctionDefinition> getFunctionDefinitions(
  const std::string& contents,
  size_t defStartIndex,
  size_t defEndIndex
);
static void writeFile(const std::string& path, const std::string& contents);
static std::string readFile(const std::string& path);
static std::vector<std::string> split(
  const std::string& str, 
  const std::string& delim, 
  int maxSplits = 0
);
static std::string join(
  const std::vector<std::string>& elems, 
  const std::string& delim
);
static bool startsWith(const std::string& s, const std::string& pattern);
static bool endsWith(const std::string& s, const std::string& pattern);
static std::string trim(const std::string& s);

std::string rewriteFunctionDefinitions(
  std::vector<FunctionDefinition>& defs
) {
  std::vector<std::string> typedefs = {}, funcDecls = {}, funcDefs = {};
  for (const auto& def: defs) {
    std::string protoDef = "typedef " + def.returnType + " (*" 
      + def.name + "_proto)(" + join(def.argTypes, ", ") + ");";
    typedefs.push_back(protoDef);
    std::string funcDecl = "inline " + def.name + "_proto " + def.name + ";";
    funcDecls.push_back(funcDecl); 
    std::string funcDef = 
      "  " + def.name + " = (" + def.name 
      + "_proto)GetProcAddress(goapi, \"" + def.name + "\");";
    std::string assertCode = 
      "  if (" + def.name + " == NULL)\n"
      "    messageAbort(\"Failed to load symbol '" + def.name + "'" 
      " from Go library (\" __goLibPath \")\");"; 
    funcDef += "\n" + assertCode;
    funcDefs.push_back(funcDef);
  }
  std::string code =
    "/* Windows loader */\n\n"
    "#include <Windows.h>\n\n"
    "#define __mbErrorCaption \"[hyprnova]: Fatal Error\"\n"
    "#define __goLibPath \"goapi.dll\"\n\n"
    "inline HMODULE goapi = (HMODULE)NULL; // Library handle \n\n"
    "/* Function pointer definitions */\n\n"
    + join(typedefs, "\n") +
    "\n\n/* Function declarations */\n\n"
    + join(funcDecls, "\n") +
    "\n\n/* Loader functions */\n\n"
    "inline void messageAbort(const char* msg) {\n"
    "  MessageBoxA(NULL, msg, __mbErrorCaption, MB_ICONERROR);\n"
    "  exit(1);\n"
    "}\n\n"
    "inline void freeGoLibrary() {\n"
    "  if (goapi != NULL)\n"
    "    FreeLibrary(goapi);\n"
    "}\n\n"
    "inline void loadGoLibrary() {\n"
    "  goapi = LoadLibraryA(__goLibPath);\n" 
    "  if (goapi == NULL)\n"
    "    messageAbort(\"Failed to load Go library (\" __goLibPath \")\");\n";
  code += join(funcDefs, "\n");
  code += "\n}\n\n";
  code += "#endif\n";
  return code;
}

std::vector<FunctionDefinition> getFunctionDefinitions(
  const std::string& contents,
  size_t defStartIndex,
  size_t defEndIndex
) {
  std::string defSubString = contents.substr(defStartIndex, defEndIndex - defStartIndex);
  std::vector<std::string> lines = split(contents, "\n");
  std::vector<FunctionDefinition> defs = {};
  for (auto& line: lines) {
    if (line.empty())
      continue;
    if (!startsWith(line, "extern __declspec(dllexport) ") || !endsWith(line, ");")) 
      continue;
    line.erase(0, 29);
    line = trim(line);
    FunctionDefinition def;
    size_t firstSpaceIndex = line.find(" ") ;
    size_t firstBracketIndex = line.find("(") + 1;
    size_t secondBracketIndex = line.find(")");
    def.returnType = line.substr(0, firstSpaceIndex);
    def.name = line.substr(firstSpaceIndex + 1, firstBracketIndex - firstSpaceIndex - 2);
    def.argTypes = {};
    std::vector<std::string> args = split(line.substr(firstBracketIndex, secondBracketIndex - firstBracketIndex), ",");
    for (auto& arg: args) {
      arg = trim(arg);
      std::string argType = arg.substr(0, arg.find(" "));
      def.argTypes.push_back(argType);
    }
    defs.push_back(def);
  }
  return defs;
}

void writeFile(const std::string& path, const std::string& contents) {
  std::ofstream f(path, std::ios::trunc);
  if (!f.is_open()) {
    throw std::runtime_error("");
  }
  f.write(contents.c_str(), contents.size());
  f.close();
}

std::string readFile(const std::string& path) {
  std::ifstream f(path);
  if (!f.is_open()) {
    throw std::runtime_error("");
  }
  f.seekg(0, std::ios::end);
  size_t size = f.tellg();
  f.seekg(0);
  std::string s;
  s.resize(size);
  f.read(s.data(), size);
  return s;
} 

std::vector<std::string> split(
  const std::string& str, 
  const std::string& delim, 
  int maxSplits
) {
  std::size_t last = 0, next = 0; 
  std::vector<std::string> splits = {};
  while (
    (next = str.find(delim, last)) != std::string::npos && 
    (maxSplits == 0 || splits.size() < maxSplits)
  ) {   
    splits.push_back(str.substr(last, next - last));
    last = next + 1; 
  } 
  splits.push_back(str.substr(last));
  return splits;
}

std::string join(
  const std::vector<std::string>& elems, 
  const std::string& delim
) {
  std::string joined = "";
  if (elems.empty()) 
    return joined;
  int size = elems.size();
  for (int i = 0; i < size; ++i) {
    joined += elems[i];  
    if (i != size - 1) 
      joined += delim;
  }
  return joined;
}

bool startsWith(const std::string& s, const std::string& pattern) {
  if (pattern.length() > s.length())
    return false;
  for (int i = 0; i < pattern.length(); ++i) {
    if (s[i] != pattern[i])
      return false;
  }
  return true;
}

bool endsWith(const std::string& s, const std::string& pattern) {
  int sLen = s.length(), patternLen = pattern.length();
  if (patternLen > sLen)
    return false;
  for (int i = 0; i < patternLen; ++i) {
    if (s[sLen - patternLen + i] != pattern[i])
      return false;
  }
  return true;
}

std::string trim(const std::string& s) {
  int startIndex, endIndex;
  for (startIndex = 0; startIndex < s.length(); ++startIndex) 
    if (!isspace(s[startIndex]))
      break; 
  for (endIndex = s.length() - 1; endIndex > 0; endIndex--) 
    if (!isspace(s[endIndex])) {
      endIndex++;
      break;
    }
  return s.substr(startIndex, endIndex);
}

int main(const int argc, const char** argv) {
#ifndef _WIN32
  printf("this script program is only used on Windows to generate a symbol loader\n");
  return -1;
#else
  if (argc < 2) {
    printf("usage: %s <header-file>\n", argv[0]);
    return -1;
  }
  std::string path = argv[1];
  std::string contents;
  try {
    contents = readFile(path);
  } catch (std::runtime_error& e) {
    printf("error: failed to open file %s\n", path.c_str());
    return -1;
  }
  try {
    size_t defStartIndex = contents.find(defStartString) + strlen(defStartString);
    size_t defEndIndex = contents.find(defEndString);
    if (defStartIndex == std::string::npos || defEndIndex == std::string::npos)   
      throw std::runtime_error("");
    auto defs = getFunctionDefinitions(contents, defStartIndex, defEndIndex);
    std::string code = rewriteFunctionDefinitions(defs);
    contents = contents.substr(0, defStartIndex - strlen(defStartString) - 10) 
      + code + contents.substr(defEndIndex + strlen(defEndString));
  } catch (std::runtime_error& e) {
    printf("error: failed to parse file\n");
    return -1;
  }
  try {
    writeFile(path, contents);
  } catch (std::runtime_error& e) {
    printf("error: failed to write to file %s\n", path.c_str());
    return -1;
  }
  return 0;
#endif
}