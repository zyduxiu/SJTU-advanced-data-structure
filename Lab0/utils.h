
#define HUFFMAN_UTILS_H
#ifndef UTILS_H
#define UTILS_H
#include<iostream>
#include <map>
#include <fstream>
#include <vector>
#include "hftree.h"
#include <algorithm>
#include <string>
namespace utils {
    std::string getFileName(const std::string &fileName, const std::string &suffix) {
        size_t final_pos = fileName.find_last_of("/\\");
        size_t suffix_pos = fileName.find_last_of('.');
        return final_pos == std::string::npos ? "./output/" + fileName.substr(0, suffix_pos) + suffix : "./output/" +
                                                                                                        fileName.substr(
                                                                                                                final_pos +
                                                                                                                1,
                                                                                                                suffix_pos -
                                                                                                                final_pos -
                                                                                                                1) +
                                                                                                        suffix;
    }

    std::string parseText(const std::string &input) {
        std::string content;//定义文件名字符数组
        std::string tmpw;
        std::ifstream in_file;
        in_file.open(input,std::ios::in|std::ios::binary);     //使ouffile关联文件
        while (getline(in_file, tmpw)) {
            content=content+tmpw;
            content=content+'\n';
        }
        in_file.close();
        return content;
    }

    void output(const std::string &output, const std::string &data) {
        std::ofstream os; //创建一个文件输出流对象
        os.open(output,std::ios::out|std::ios::binary);//将对象与文件关联
        os << data; //将输入的内容放入txt文件中
        os.close();
        return;
        // TODO: Your code here
    }

    std::string codingTable2String(const std::map<std::string, std::string> &codingTable) {
        std::string result = "";
        std::vector<std::string> strArray;
        int i, j = 0;
        for (auto it: codingTable) {
            strArray.push_back(it.first);
        }
        std::sort(strArray.begin(), strArray.end());
        std::vector<std::string>::iterator st;
        for (int i = 0; i < strArray.size(); i++) {
            result += strArray[i];
            result += " ";
            for (auto it: codingTable) {
                if (it.first.compare(strArray[i]) == 0) {
                    result += it.second;
                }
            }
            result += "\n";
        }
        // TODO: Your code here
        return result;
    }

    void loadCodingTable(const std::string &input, std::map<std::string, std::string> &codingTable) {
        std::string content;
        std::ifstream in_file;
        in_file.open(input);
        while (getline(in_file, content)) {
            if (content.compare("") != 0&&content.compare("") != 1) {
                std::string st1;
                int pos1 = 0;
                std::string st2;
                for (int i = 0; i < content.size(); i++) {
                    if (content[i] == ' ') {
                        if (content[i + 1] == ' ') {
                            if (content[i + 2] == ' ') {
                                pos1 = i + 2;
                                break;
                            } else {
                                pos1 = i + 1;
                                break;
                            }
                        } else {
                            if (content[i + 2] == ' ') {
                                pos1 = i + 2;
                                break;
                            }
                            pos1 = i;
                            break;
                        }
                    }
                }
                st1 = content.substr(0, pos1);
                st2 = content.substr(pos1 + 1, content.size() - pos1);
                codingTable.insert(std::pair<std::string, std::string>(st1, st2));
            }
            else{
                std::string st1=content+"\n";
                getline(in_file,content);
                std::string st2;
                int pos1;
                for (int i = 0; i < content.size(); i++) {
                    if (content[i] == ' ') {
                        if (content[i + 1] == ' ') {
                            if (content[i + 2] == ' ') {
                                pos1 = i + 2;
                                break;
                            } else {
                                pos1 = i + 1;
                                break;
                            }
                        } else {
                            if (content[i + 2] == ' ') {
                                pos1 = i + 2;
                                break;
                            }
                            pos1 = i;
                            break;
                        }
                    }
                }
                st2 = content.substr(pos1 + 1, content.size() - pos1);
                codingTable.insert(std::pair<std::string, std::string>(st1, st2));
            }
        }
        // TODO: Your code here
    }

    std::string compress(const std::map<std::string, std::string> &codingTable, const std::string &text) {
        std::string result;
        for (int i = 0; i < text.size() - 1; i++) {
            bool whetherdouble = false;
            std::string tmp = text.substr(i, 2);
            for (auto it: codingTable) {
                if (tmp.compare(it.first) == 0 && it.first.size() == 2) {
                    result += it.second;
                    whetherdouble = true;
                    break;
                }
            }
            if (whetherdouble == true) {
                i = i + 1;
            } else {
                for (auto it: codingTable) {
                    if (it.first.size()==1&&text[i]==it.first[0]) {
                        result += it.second;
                        break;
                    }
                }

            }
            // TODO: Your code here

        }
        int rest=8-result.size()%8;
        for(int i=0;i<rest;i++){
            result+="0";
        }
        std::string hex_result="";
        for(int i=0;i<result.size();i+=4){
            std::string s=result.substr(i,4);
            int r = 0, j, t = 1;
            for(j = s.size() - 1; j >= 0; j--){
                r = r + (s[j] - '0') * t;
                t = t * 2;
            }

            char c;	// 存储1位的16进制字符
            if(r < 10){
                c = r + '0';
            }else{
                c = r + 'A' - 10;
            }

            hex_result=hex_result+c;
        }
        return hex_result;
    };
}
#endif //HUFFMAN_UTILS_H
