#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <map> 
using namespace std;

typedef map<string,float> StringFloatMap;
StringFloatMap vars;
map<string, float>::iterator vars_i;  

int precision = 2; // number of decimals

std::string exec(const char* cmd) {
	// Replace popen and pclose with _popen and _pclose for Windows.
	// https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po

    char buffer[128];
    std::string result = "";
    std::string myresult = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer; // we only need the first line of output from process
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

int main(int argc, char *argv[]){
	// open a file in read mode.
	ifstream infile; 
	infile.open("calculations"); 

	std::string line;
	std::string exp;
	std::string var;
	std::string cmdline;
	std::string result;
	std::ostringstream rounded;
	rounded.precision(precision);
	while (std::getline(infile, line)){
		if(line == "0=0"){
			std::cout << "EXIT" << endl;
			return 0;
		}else{
			exp = line.substr(line.find("=")+1);
			for(vars_i = vars.begin(); vars_i != vars.end(); vars_i++){
				//std::to_string(vars_i->second)
				rounded.str("");
				rounded.clear();
				rounded << std::fixed << vars_i->second;
				replaceAll(exp, vars_i->first, rounded.str());
//				std::cout << vars_i->first << vars_i->second << endl;
			}
			var = line.substr(0, line.find("="));
			cmdline = "echo 'scale="+std::to_string(precision)+"; "+exp+"' | bc";

			result = exec(cmdline.c_str());
			result = result.substr(0, result.find("\n")); // remove stuff after line break
			std::cout << var << "=" << exp << "=" << result << ";" << endl;

			vars[var] = ::atof(result.c_str());
		}

	}

	std::cout << "VARS" << endl;
//	std::cout << "x=" << vars["x"] << "SLUT";
	for(vars_i = vars.begin(); vars_i != vars.end(); vars_i++){
//		replaceAll(exp, "+", "*0+");
		std::cout << vars_i->first << "=" << vars_i->second << endl;
	}

	infile.close();
	return 0;
}


/*

#!/bin/bash
IFS=' ' # space is set as delimiter

declare -A VARS
VARS[t]=0
#VARS[y]=10


while [ : ]
do
  VARS[t]=$((VARS[t]+1))
  while IFS= read -r line
  do
    if [ "$line" == "0=0" ]; then
      echo "Received 0=0. Exiting";
      exit 0;
    else
      V=${line%=*}
      repline=${line#*=}

      for i in "${!VARS[@]}";
      do
#       echo "$i=${VARS[$i]}";
        repline=${repline//$i/${VARS[$i]}}
      done;
      E=${repline}

      RES=$(echo "$E" | bc)
      VARS[$V]=$((RES))
      echo "$line = $E = $RES"
    fi;
  done < calculations
  echo "-"
  sleep 1
done

*/
