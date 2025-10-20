#include <iostream>
#include <random>
#include <fstream>
#include <chrono>
#include <omp.h>

std::ofstream writeFile(const std::string& fileName, unsigned long long int NoOfPermutations, unsigned int random, double error, double elapsed);
std::ifstream readFile(const std::string& fileName);
std::ofstream deleteData(const std::string& fileName);
bool isFileEmpty(const std::string& fileName);

int password{ 10212 };

int numberOfRepetition(int number, int count)
{
	std::cout << "\nNumber of " << number << "'s appeared are " << count;
	return 0;
}

long long int chance(int number, int repeated, unsigned long long int permutations)
{
	std::cout << "\n" << number << " had " << (repeated * 100.0) / permutations << "% chance of appearing.";
	return 0;
}

double modulus(double number)
{
	return number >= 0 ? number : -number;
}


void ignoreLine()
{
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// returns true if extraction failed, false otherwise
bool clearFailedExtraction()
{
	// Check for failed extraction
	if (!std::cin) // If the previous extraction failed
	{
		if (std::cin.eof()) // If the stream was closed
		{
			std::exit(0); // Shut down the program now
		}

		// Let's handle the failure
		std::cin.clear(); // Put us back in 'normal' operation mode
		ignoreLine();     // And remove the bad input

		return true;
	}

	return false;
}

int main()
{
	while (true)
	{
		double error{};
		unsigned int seed{};
		unsigned long long int iterations{};
		bool wannaSee{};
		double mean{};

		while (true) //cheking for input and any error in them
		{

			std::cout << "\nEnter any random positive integer: ";
			std::cin >> seed;
			if (clearFailedExtraction())
			{
				continue;
			}

			std::cout << "Enter the number of iterations: ";
			std::cin >> iterations;

			if (clearFailedExtraction())
			{
				continue;
			}

			std::cout << "Do you want to see all the iterations (1 for yes, 0 for no): ";
			std::cin >> wannaSee;

			if (clearFailedExtraction())
			{
				continue;
			}

			if (iterations == password && seed == password)
			{
				std::cout << "\nDeleting all data!";
				deleteData("data.txt");

				if (isFileEmpty("data.txt"))
					std::cout << "\nThe file was successfully deleted!";
				else
					std::cout << "\nUnable to delete the file!!";

				continue;
			}


			break;
		}


		long long int dieNumber[6]{ 0, 0, 0, 0, 0, 0 };

		auto start = std::chrono::high_resolution_clock::now();//start point of the clock to see how long it took

#pragma omp parallel
		{
			std::mt19937 mt_local(seed + omp_get_thread_num());//random number
			std::uniform_int_distribution<int> die(1, 6);
			long long int local_dieNumber[6]{ 0, 0, 0, 0, 0, 0 };

#pragma omp for
			for (long long int i = 0; i < iterations; ++i)//outputing all the iterations and adding them to local_dieNumber
			{

				int value = die(mt_local);
				local_dieNumber[value - 1]++;

				if (wannaSee)
				{
					std::cout << value << '\t';

					if (i % 10 == 0)
					{
						std::cout << "\n";
					}
				}
			}

#pragma omp critical
			{
				for (long long int i = 0; i < 6; ++i) // adding local_dieNumber to dieNumber
					dieNumber[i] += local_dieNumber[i];
			}
		}

		std::cout << '\n' << "For " << iterations << " iterations:";

		long long int total{};
		for (int i = 0; i < 6; i++) //outputing the data of number to times a number was repeated
		{
			numberOfRepetition(i + 1, dieNumber[i]);
			total += dieNumber[i];
		}

		for (int i = 0; i < 6; i++)//outputing the chance of the number appearing
		{
			chance(i + 1, dieNumber[i], iterations);
		}

		mean = total / 6.0;

		error = (
			(modulus(dieNumber[0] - mean) +
				modulus(dieNumber[1] - mean) +
				modulus(dieNumber[2] - mean) +
				modulus(dieNumber[3] - mean) +
				modulus(dieNumber[4] - mean) +
				modulus(dieNumber[5] - mean)) / 6.0) * 100.0 / mean;

		std::cout << "\nPercentage mean average error in probability is: " << error << "%";
		std::cout << "\nthe given die is " << 100 - error << "% as accurate as a perfect dice";

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;

		std::cout << "\nTime taken to complete the given iterations: " << elapsed.count() << " seconds";

		writeFile("data.txt", iterations, seed, error, elapsed.count());
	}

	return 0;
}