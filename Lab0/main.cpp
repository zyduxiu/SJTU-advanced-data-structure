#include <iostream>
#include <memory>
#include "hftree.h"
#include "utils.h"

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cout << "[usage]: ./main [input_file_path]" << std::endl;
        return 0;
    }

    // Parse the input file and get all content
    std::string input_file(argv[1]);
    std::string text = utils::parseText(input_file);

    {
        std::map<std::string, std::string> codingTable;

        // Build a huffman tree according to different compression encoding methods
        std::unique_ptr<hfTree> huffmanTree_single = std::make_unique<hfTree>(text, hfTree::Option::SingleChar);

        // Compression encoding by SingleChar
        utils::output(utils::getFileName(input_file, ".sin.huffidx"), utils::codingTable2String(huffmanTree_single->getCodingTable()));
        utils::loadCodingTable(utils::getFileName(input_file, ".sin.huffidx"), codingTable);
        utils::output(utils::getFileName(input_file, ".sin.huffzip"), utils::compress(codingTable, text));
    }

    // Compression encoding by MultiChar
    {
        std::map<std::string, std::string> codingTable;

        std::unique_ptr<hfTree> huffmanTree_multiple = std::make_unique<hfTree>(text, hfTree::Option::MultiChar);

        utils::output(utils::getFileName(input_file, ".mul.huffidx"), utils::codingTable2String(huffmanTree_multiple->getCodingTable()));
        utils::loadCodingTable(utils::getFileName(input_file, ".mul.huffidx"), codingTable);
        utils::output(utils::getFileName(input_file, ".mul.huffzip"), utils::compress(codingTable, text));
    }

    return 0;
}