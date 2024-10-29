#include "instance.h"

void Instance::read_input(const std::string input, const std::string mode)
{
    std::ifstream myFile(input);

    if (!myFile.is_open())
        std::cerr << "File input is missing !!";
    std::string line;
    std::vector<std::string> numbers;
    if (mode == "pardo")
    {
        num_nodes = 0;


        //skip header
        std::getline(myFile, line);
        //std::cout << line << '\n';

        while (std::getline(myFile, line))
        {
            num_nodes++;
            split(line, numbers, '\t');
            /*for (auto i : numbers)
            {
                std::cout << i << " ";
            }
            std::cout << '\n';
            std::cout << numbers.size() << '\n';*/
            x.push_back(stof(numbers[1]));
            y.push_back(stof(numbers[2]));
            release_time.push_back(stoi(numbers[3]));

        }

        // depot n+1
        x.push_back(x[0]);
        y.push_back(y[0]);
        release_time.push_back(release_time[0]);
        num_nodes++;

        std::cerr << x[0] << ' ' << y[0] << ' ' << release_time[0] << '\n';
        std::cout << "num_node = " << num_nodes << '\n';
    }

    if (mode == "solomon")
    {
        getline(myFile, line);
        split(line, numbers, ' ');
        num_nodes = stoi(numbers[1]);
        // std::cout << "num node = " << num_nodes << '\n';

         //skip header
        getline(myFile, line);
        getline(myFile, line);
        getline(myFile, line);
        getline(myFile, line);

        //read coordi and release date
        for (int i = 0; i < num_nodes; i++)
        {
            getline(myFile, line);
            //std::cout << line << '\n';
            split(line, numbers, '\t');
            x.push_back(stof(numbers[0]));
            y.push_back(stof(numbers[1]));
            release_time.push_back(stoi(numbers[6]));
        }

        //add depot n+1
        num_nodes++;
        x.push_back(x[0]);
        y.push_back(y[0]);
        release_time.push_back(release_time[0]);

        //std::cerr << x[11] << ' ' << y[11] << ' ' << release_time[11] << '\n';
        //std::cout << "num_node = " << num_nodes << '\n';
    }

}

Instance* Instance::singleton_ = nullptr;;
Instance* Instance::getInstance(const std::string input, const std::string param, const std::string mode)
{
    if (singleton_ == nullptr) {
        singleton_ = new Instance(input, param, mode);
    }
    return singleton_;
}

template<class Container>
void Instance::split(const std::string& str, Container& cont, char delim)
{
    cont.clear();
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        if (token != "")
            cont.push_back(token);
    }
}