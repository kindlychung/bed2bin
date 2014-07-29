#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

std::ifstream::pos_type filesize(std::string filename)
{
    std::ifstream in(filename, std::ifstream::in | std::ifstream::binary);
    in.seekg(0, std::ifstream::end);
    return in.tellg();
}

int nlines(std::string filename) {
    std::ifstream inFile(filename);
    return std::count(std::istreambuf_iterator<char>(inFile),
                      std::istreambuf_iterator<char>(), '\n');
}


int main(int argc, char* argv[])
{
    const unsigned char homo1bits = 3;
    const unsigned char homo2bits = 0;
    const unsigned char heterobits = 2;
    const unsigned char missingbits = 1;
    const short int homo1 = 0;
    const short int homo2 = 2;
    const short int hetero = 1;
    const short int missing = 9;

    std::string bedfile;
    std::string bimfile;
    std::string famfile;
    std::string outfile;
    bedfile = argv[1] + std::string(".bed");
    bimfile = argv[1] + std::string(".bim");
    famfile = argv[1] + std::string(".fam");
    outfile = argv[1] + std::string(".bin");

    // validation of number of genotypes
    int ndatapoints = ((int)filesize(bedfile) - 3) * 4;
    int nsnp = nlines(bimfile);
    int nobs = nlines(famfile);
    int nobsAppa = nobs - (nobs % 4) + 4; // apparent number of observations
    int estnda = nsnp * nobsAppa;
    if(estnda != ndatapoints) {
        throw "Number of genotypes are not in agreement with bim and fam files!";
    }

    std::vector<unsigned char> bytes;
    {
        std::ifstream in(bedfile, std::ios_base::binary);
        bytes.assign(std::istreambuf_iterator<char>(in >> std::noskipws),
                     std::istreambuf_iterator<char>());
    }

    // file format validation
    if(bytes[0] != 0x6c || bytes[1] != 0x1b) {
        throw "This a not a valid plink BED file!";
    }
    if(bytes[2] != 1) {
        throw "This file is not in SNP major!";
    }

    std::vector<short int> genotypes;
    {
        unsigned char genobits;
        for (int i=3; i < bytes.size(); ++i) {
            for (int j=0; j < 4; ++j) {
                genobits = (bytes[i] >> 2*j) & 3;
                switch (genobits) {
                case homo1bits:
                    genotypes.push_back(homo1);
                    break;
                case homo2bits:
                    genotypes.push_back(homo2);
                    break;
                case heterobits:
                    genotypes.push_back(hetero);
                    break;
                default:
                    genotypes.push_back(missing);
                    break;
                }
            }
        }
    }

    {
        std::ofstream fout(outfile, std::ios::out | std::ios::binary);
        for (int i = 0; i < nobs; i++) {
            for (int j = 0; j < nsnp; j++) {
                fout.write((char *) &genotypes[i + j * nobsAppa], sizeof(short int));
            }
        }
    }

    // for (int i = 0; i < genotypes.size(); i++) {
    //     if (i % 8 == 0) {
    //         std::cout << std::endl;
    //     }
    //     std::cout << genotypes[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < nobs; i++) {
    //     for (int j = 0; j < nsnp; j++) {
    //         std::cout << genotypes[i + j * nobsAppa] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    std::cout << std::endl;

    return 0;
}
