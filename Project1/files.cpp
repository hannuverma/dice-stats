#include <iostream>
#include <fstream>
#include <string>

// Function to create and return an ofstream (write mode)
std::ofstream writeFile(const std::string& fileName, unsigned long long int NoOfPermutaions, unsigned int random, double error, double elapsed)
{
	std::ofstream outFile(fileName, std::ios::app); // Open the file for writing
	if (!outFile.is_open())
	{
		std::cerr << "Error opening file for writing: " << fileName << std::endl;
	}

	else
	{
		outFile << "iterations: " << NoOfPermutaions << " random number chosen: " << random << " percentage mean average error: " << error << " time taken: " << elapsed << '\n';
	}

	outFile.close();

	return outFile; // Return the ofstream object (by value)
}

// Function to create and return an ifstream (read mode)
std::ifstream readFile(const std::string& fileName)
{
	int lineNumber{ 1 };
	std::string myText;

	std::ifstream inFile(fileName); // Open the file for reading
	if (!inFile.is_open())
	{
		std::cerr << "Error opening file for reading: " << fileName << std::endl;
	}

	while (std::getline(inFile, myText)) {
		// Output the text from the file
		std::cout << lineNumber++ << ": " << myText << '\n';
	}

	inFile.close();

	return inFile; // Return the ifstream object (by value)
}

std::ofstream deleteData(const std::string& fileName)
{
	std::ofstream file(fileName);

	file.close();
	return file;
}

bool isFileEmpty(const std::string& fileName)
{
	std::ifstream file(fileName);
	return file.peek() == std::ifstream::traits_type::eof();
}