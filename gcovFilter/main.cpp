// Copyright 2014 The CRAVE developers. All rights reserved.//

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <utility>

std::vector<std::string> readFile(std::string filename) {
  std::string line;
  std::vector<std::string> fileData;
  std::ifstream myfile(filename.c_str());
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      fileData.push_back(line);
    }
    myfile.close();
  }
  return fileData;
}

int findNextFileName(unsigned int start, const std::vector<std::string> &data) {
  for (unsigned int i = start; i < data.size(); i++) {
    if (data.at(i).find("File") != data.at(i).npos) {
      return i;
    }
  }
  return -1;
}

int findNextBlankLine(unsigned int start,
                      const std::vector<std::string> &data) {
  for (unsigned int i = start; i < data.size(); i++) {
    if (data.at(i).empty()) {
      return i;
    }
  }
  return -1;
}

bool isCraveSrc(std::string s) {
  return (s.find("/crave/src/") != s.npos) || (s.find("crave/tests") != s.npos);
}

std::vector<std::string> collectBranches(const std::vector<std::string> &data) {
  std::vector<std::string> branches;
  for (unsigned i = 0; i < data.size(); i++) {
    std::string line = data[i];
    if (line.find("Branches executed:") != line.npos) {
      line.erase(line.begin(),
                 line.begin() + std::string("Branches executed:").size());
      branches.push_back(line);
    }
  }
  return branches;
}

float toFloat(std::string s) {
  std::stringstream ss;
  ss << s;
  float f;
  ss >> f;
  return f;
}

float toUint(std::string s) {
  std::stringstream ss;
  ss << s;
  unsigned int f;
  ss >> f;
  return f;
}

std::vector<std::pair<float, unsigned int> > convertBranches(
    std::vector<std::string> branches) {
  std::vector<std::pair<float, unsigned int> > pairs;
  for (unsigned i = 0; i < branches.size(); i++) {
    std::string s = branches[i];
    pairs.push_back(std::make_pair(toFloat(s.substr(0, s.find("% of"))),
                                   toUint(s.substr(s.find("% of") + 5))));
  }
  return pairs;
}

void filterOther(std::vector<std::string> *fileData) {
  int i = findNextFileName(0, *fileData);
  do {
    i = findNextFileName(i, *fileData);
    if (i != -1 && !isCraveSrc(fileData->at(i))) {
      int blankLine = findNextBlankLine(i, *fileData);
      if (blankLine != -1) {
        fileData->erase(fileData->begin() + i,
                        fileData->begin() + blankLine + 1);
      } else {
        fileData->erase(fileData->begin() + i, fileData->end());
      }
    } else if (i != -1) {
      i++;
    }
  } while (i != -1);
}

void printTotalCoverage(
    const std::vector<std::pair<float, unsigned int> > &branches) {
  unsigned int totalBranches = 0;
  float percent = 0;
  for (unsigned i = 0; i < branches.size(); i++) {
    std::pair<float, unsigned int> p = branches[i];
    percent += p.second * p.first;
    totalBranches += p.second;
  }
  if (totalBranches != 0) {
    percent /= totalBranches;
  }
  std::cout << "TOTAL: " << percent << "% of " << totalBranches << std::endl;
}

int main(int argc, char **argv) {
  if (argc != 2) {  // argc should be 2 for correct execution
    // We print argv[0] assuming it is the program name
    std::cout << "usage: " << argv[0] << " <filename>" << std::endl;
    return -1;
  } else {
    std::vector<std::string> fileData;
    fileData = readFile(argv[1]);
    filterOther(&fileData);
    std::vector<std::string> branches = collectBranches(fileData);
    for (unsigned i = 0; i < fileData.size(); i++) {
      std::cout << fileData[i] << std::endl;
    }
    std::vector<std::pair<float, unsigned int> > fubranches =
        convertBranches(branches);
    printTotalCoverage(fubranches);
  }
  return 0;
}
