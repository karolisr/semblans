#include "fastqc_wrap.h"

void run_fastqc(std::vector<SRA> sras, std::string threads) {
  std::string outDir(sras[0].get_fastqc_dir().first.parent_path().parent_path().native().c_str());
  std::string outFile;
  std::string inFile1;
  std::string inFile2;
  std::string fastqcFlags = " --extract -t " + threads + " -o ";
  for (auto sra : sras) {
    std::cout << "\nRunning quality analysis for:\n" << std::endl;
    summarize_sing_sra(sra);
    outFile = outDir + "/" + sra.make_file_str();
    system(("mkdir " + outFile).c_str());
    if (sra.is_paired()) {
      inFile1 = sra.get_sra_path_raw().first.c_str();
      inFile2 = sra.get_sra_path_raw().second.c_str();
      system((PATH_FASTQC + fastqcFlags + outFile + " " +
              inFile1 + " " + inFile2).c_str());
    }
    else {
      inFile1 = sra.get_sra_path_raw().first.c_str();
      system((PATH_FASTQC + fastqcFlags + outFile + " " + inFile1).c_str());
    }
  } 
}
