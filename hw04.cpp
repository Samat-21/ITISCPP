#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

namespace po = boost::program_options;
namespace fs = boost::filesystem;


void copy_file(std::string name_f, std::string name_d, std::string main_dir, std::string cur_dir) {
  fs::path src(cur_dir + "/" + name_f);
  std::string s = "../" + main_dir + "/" + name_d + "/" + name_f;
  try {
    fs::path dest(s);
    if (!fs::exists(dest.parent_path())) {
      fs::create_directory(dest.parent_path());
    }
    fs::copy(src, dest);
  } catch(const fs::filesystem_error& e) {
    std::cerr << e.what();
  }
}


void simple_ls(const fs::path& in_folder, std::map<std::string, std::string> rules, std::string main_dir, std::string cur_dir) {
  if (fs::exists(in_folder)) {
    if (fs::is_regular_file(in_folder)) {
      std::cout << "Regular file: " << in_folder.string()[0] << std::endl;
    } else if (fs::is_directory(in_folder)) {
      for (const auto& entry : fs::directory_iterator(in_folder)) {
        fs::file_status st = entry.status();
        fs::path p(entry);
        std::string f = p.filename().string();
        int tip_i = f.find(".");
        std::string tip;
        switch (st.type())
        {
          case fs::regular_file:
            if (tip_i > -1){
              tip = f.substr(tip_i+1);
              if(rules.find(tip) != rules.end()){
                //std::cout << cur_dir + f << "\n";
                copy_file(f, rules.find(tip)->second, main_dir, cur_dir);
              }
              else{
                copy_file(f, "Other", main_dir, cur_dir);
              }
            }
            else{
                copy_file(f, "Other", main_dir, cur_dir);
              }
            break;

          case fs::directory_file:
            simple_ls(entry, rules, main_dir, cur_dir + "/" + f);
            break;
          
          default:
            break;
        }
      }
    }
  } else {
    std::cout << in_folder << " does not exist!\n";
  }
}


std::map <std::string, std::string> read_rules(std::string name){
    std::map <std::string, std::string> rules;
    std::string line;
 
    std::ifstream in(name); // окрываем файл для чтения
    if (in.is_open())
    {
        while (getline(in, line))
        {
            std::string tip, papka;
            int flag = 0;
            for (size_t i = 0; i < line.length(); i++)
            {
              if (line[i] != '\n'){
                if (line[i] == ':')
                {
                  flag = 1;
                }
                else{
                  if (flag == 0){
                    tip+=line[i];
                  }
                  if (flag == 1){
                    papka+=line[i];
                  }
                }
                
              }
            }
            rules[tip] = papka;
        }
    }
    
    in.close(); 
    return rules;
}


    

int main(int argc, char** argv)
{
  std::map <std::string, std::string> rules = read_rules("../rules.txt");
  std::map <std::string, std::string> :: iterator it = rules.begin();
  std::vector<std::string> vec = {"Other"}; 
    for (int i = 0; it != rules.end(); it++, i++) {  // выводим их
      if (std::find(vec.begin(), vec.end(), it->second) == vec.end()){
        vec.push_back(it->second);
      }
    }
  fs::path in_folder = "../HW_test";
  try {
    simple_ls(in_folder, rules, "HW_test", "../HW_test");
  } catch (const fs::filesystem_error& exc) {
    std::cerr << exc.what() << std::endl;
  }  

  for (const auto& entry : fs::directory_iterator(in_folder)) {
        fs::file_status st = entry.status();
        fs::path p(entry);
        std::string f = p.filename().string();
        switch (st.type())
        {
          case fs::regular_file:
            fs::remove(entry);
            break;

          case fs::directory_file:
          
            if (std::find(vec.begin(), vec.end(), f) == vec.end()){
              fs::remove_all(entry);
            }
            break;
          
          default:
            break;
        }
      }
  return 0;
}