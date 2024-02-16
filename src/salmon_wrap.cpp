#include "salmon_wrap.h"

// Index reads from sequence data using Salmon
void salmon_index(std::string transIn, std::string transIndex, std::string threads,
                  bool dispOutput, std::string logFile) {

  std::string printOut;
  if (dispOutput) {
    printOut = " 2>&1 | tee -a " + logFile;
  }
  else {
    printOut = " >>" + logFile + " 2>&1";
  }
  int result;

  std::string salm_cmd = PATH_SALMON + " index" + " -t " + transIn +
                         " -i " + transIndex + " -p " + threads + printOut;
  result = system(salm_cmd.c_str());
  if (WIFSIGNALED(result)) {
    logOutput("Exited with signal " + std::to_string(WTERMSIG(result)), logFile);
    exit(1);
  }
}

bool runPaired(std::vector<std::pair<std::string, std::string>> sraRunsIn) {
  int numSingle = 0;
  int numPaired = 0;
  for (auto sraRun : sraRunsIn) {
    if (sraRun.second != "") {
      numPaired++;
    }
    else {
      numSingle++;
    }
  }
  if (numPaired >= numSingle) {
    return true;
  }
  else {
    return false;
  }
}

// Quantify reads from sequence data. Assumes all reads are either single or paired-end
void salmon_quant(std::string transIn, std::string transIndex, std::string transQuant,
                  std::vector<std::pair<std::string, std::string>> sraRunsIn,
                  std::string threads, bool dispOutput, std::string logFile) {
  std::string transFilePath(transIn);
  std::string indexFilePath(transIndex);
  std::string quantFilePath(transQuant);
  
  std::string sras1 = "";
  std::string sras2 = "";

  // Determine if more data exists for paired or single runs contituting transcript
  bool morePaired = false;
  morePaired = runPaired(sraRunsIn);
  
  // Construct largest possible list of either paired or single runs
  for (int i = 0; i < sraRunsIn.size(); i++) {
    if (morePaired) {
      if (sraRunsIn[i].second != "") {
        sras1 += (sraRunsIn[i].first + " ");
        sras2 += (sraRunsIn[i].second + " ");
      }
    }
    else {
      if (sraRunsIn[i].second == "") {
        sras1 += (sraRunsIn[i].first + " ");
      }
    }
  }

  std::string printOut;
  if (dispOutput) {
    printOut = " 2>&1 | tee -a " + logFile;
  }
  else {
    printOut = " >>" + logFile + " 2>&1";
  }
  int result;
  
  if (morePaired) {
    std::string salm_cmd = PATH_SALMON + " quant" + " -i " + indexFilePath + " --dumpEq" +
                           " --writeUnmappedNames " + " --libType" + " A" + " -p " + threads +
                           " -1 " + sras1 + " -2 " + sras2 + " -o " + quantFilePath + printOut;
    result = system(salm_cmd.c_str());
  }
  else {
    std::string salm_cmd = PATH_SALMON + " quant" + " -i " + indexFilePath + " --dumpEq" +
                           " --writeUnmappedNames " + " --libType" + " A" + " -p " + threads +
                           " -r " + sras1 + " -o " + quantFilePath +
                           printOut;
    result = system(salm_cmd.c_str());
  }
  if (WIFSIGNALED(result)) {
    system("setterm -cursor on");
    logOutput("Exited with signal " + std::to_string(WTERMSIG(result)), logFile);
    exit(1);
  }
  replaceChar(logFile, '\r', '\n');
}
