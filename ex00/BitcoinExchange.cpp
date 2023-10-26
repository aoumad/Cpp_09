/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aoumad <aoumad@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 15:47:10 by aoumad            #+#    #+#             */
/*   Updated: 2023/03/22 15:47:12 by aoumad           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange(char *input_filename)
{
    try
    {
        fill_the_map(); 
        Input_checker(input_filename);
        // print the _rtn_rates vector
        for (size_t i = 0; i < this->_rtn_rates.size(); i++)
            std::cout << _rtn_rates[i] << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    // find_closest_date();
}

BitcoinExchange::~BitcoinExchange()
{
}

// int     BitcoinExchange::is_valid_value(int i)
// {
//     int count = count(_value[i].begin(), _value[i].end(), '.');

//     if (count > 1 || _value[i].find_first_not_of("f0123456789.") != std::string::npos)
//     {
//         std::cer << "Error: the value " << _value[i] << " is incorrect!" << std::endl;
//      
//     }

//     _value[i].erase(_value[i].begin()) // in this way i am skipping the space after the vertical bar `|`
//     // then we will check if there is minus operator or not
//     if (_value[i][0] == '-')
//     {
//         std::cerr << "Error: not a positive number." << std::endl;
//      
//     }

//     double nbr = std::stod(_value[i]);
//     if (nbr > INT_MAX)
//     {
//         std::cer << "Error: too large a number." << std::endl;
//      
//     }

//     _value[i] = nbr;
//   
// }

int BitcoinExchange::is_valid_date(std::string date)
{
    int day, month, year;
    char first_del, second_del;
    bool leap_year;

    std::istringstream ss(date);
    ss >> year >> first_del >> month >> second_del >> day;
    // the ss.fail() check tells us if an input was read
    if (ss.fail() || count(date.begin(), date.end(), '-') != 2 || first_del != '-' || second_del != '-'
        || day < 1 || day > 31 || month < 1 || month > 12 || year < 0)
    {
        this->_rtn_rates.push_back("Error: bad input => " + date);
        return (false);
    }

    leap_year = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
    /*
    This part of the code checks whether the given year is a leap year. 
    Leap years have an extra day (February 29), and they occur every four years, with some exceptions. 
    The conditions (year % 4 == 0) and (year % 100 != 0) check if the year is divisible by 4 but not divisible by 100. 
    Additionally, (year % 400 == 0) checks if the year is divisible by 400.
    If any of these conditions are true, leap_year is set to true, indicating that it's a leap year.
    */
    if ((month == 2 && (leap_year ? day > 29 : day > 28)) ||
    ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30))
    {
        this->_rtn_rates.push_back("Error: bad input => " + date);
        return (false);
    }

    /*
    In a standard (non-leap) year, February has 28 days. This is because the calendar is designed with 365 days, 
    and February typically has 28 days to keep the total year length close to 365 days.

    In a leap year, an extra day is added to the calendar, making it 366 days in total.
    This extra day is added to the end of February, resulting in February having 29 days instead of the usual 28.
    */
   return (true);
}

void    BitcoinExchange::find_accurate_date() // Binary search approach
{
    std::string input_date = this->_date.back();
    double minDiff = 1e10; // A large number that serves as an initial maximum
    std::string closest_date = "";

    std::map<std::string, double>::iterator it = this->_exchange_rates.begin();
    std::map<std::string, double>::iterator end = this->_exchange_rates.end();

    while (it != end)
    {
        const std::string& data_date = it->first;
        // Calculate the absolute difference between the input date and data date
        double diff = calculate_date_diff(input_date, data_date);

        if (diff < minDiff && diff >= 0)
        {
            minDiff = diff;
            closest_date = data_date;
        }

        if (input_date == data_date)
        {
            // an exact match was found, thus we need to exit
            closest_date = data_date;
            break;
        }
        else if (input_date < data_date)
        {
            end = it;
            // in this case it seems that the input date doesn't exist in my DB
            //so i should exit and just get the last value i got so far
        }
        else
            it++;
    }
    this->_closest_date = closest_date;
    std::map<std::string, double>::iterator ite = _exchange_rates.find(closest_date);
    if (it != _exchange_rates.end())
        this->_rtn_rate = _value.back() * ite->second;

    char buffer[256]; // A buffer to hold the formatted string

    sprintf(buffer, "%s => %.1f => %.2f", this->_date.back().c_str(), this->_value.back(), this->_rtn_rate);
    std::string rtn_string(buffer);
    this->_rtn_rates.push_back(rtn_string);
}


double  BitcoinExchange::calculate_date_diff(std::string date1, std::string date2)
{
    int year1, month1, day1;
    int year2, month2, day2;

    sscanf(date1.c_str(), "%d-%d-%d", &year1, &month1, &day1); // int sscanf( const char* buffer, const char* format, ... );
    sscanf(date2.c_str(), "%d-%d-%d", &year2, &month2, &day2);

    // Calculating the diff
    int days1 = (year1 * 365 + month1 * 30 + day1);
    int days2 = (year2 * 365 + month2 * 30 + day2);

    return (static_cast<double>((days1 - days2)));
}

// an updated and better function of Input_checker
void    BitcoinExchange::Input_checker(char *file)
{
    std::ifstream fs(file);
    std::string date;
    std::string value;
    // int     index;

    // check the existence of the file
    if (!fs.is_open())
    {
        std::cerr << "Error: Failed to open file!!" << std::endl;
        exit(1);
    }

    // parsing the first line
    std::string line;
    if (!std::getline(fs, line) || line != "date | value")
    {
        std::cerr << "Error: the first line doesn't match the line in the example!!" << std::endl;
        fs.close();
        exit(1);
    }

    if (fs.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "Error: the input doesn't contain any data!" << std::endl;
        fs.close();
        exit(1);
    }

    // index = 0;
    while (std::getline(fs, line))
    {
        if (is_valid_line(line) == true)
            generate_report();
    }
}

int BitcoinExchange::is_valid_line(std::string line)
{
    size_t index = line.find('|');
    std::string value;
    if (index == std::string::npos || line.at(11) != '|' || index != line.rfind('|'))
    {
        this->_rtn_rates.push_back("Error: bad input => " + line);
        return (false);
    }

    _date.push_back(line.substr(0, index));
    value = line.substr(index + 1);

    // if (date[i].empty() || value.empty())
    if (_date.back().empty() || value.empty())
    {
        this->_rtn_rates.push_back("Error: bad input => " + line);
        return (false);
    }
    if (is_valid_date(_date.back()) == false || is_valid_value(value) == false)
        return (false);
    // std::cout << "date: " << _date.back() << " | value: " << _value.back() << std::endl;
    // _exchange_rates.insert(std::pair<std::string, double>(_date.back(), _value.back()));
    // // print the map
    // std::cout << "The map is: " << std::endl;
    // std::map<std::string, double>::iterator it = _exchange_rates.begin();
    // std::map<std::string, double>::iterator end = _exchange_rates.end();
    // while (it != end)
    // {
    //     std::cout << it->first << " => " << it->second << '\n';
    //     it++;
    // }
    // std::cout << std::endl;
    return (true);
}

int BitcoinExchange::is_valid_value(std::string value)
{
    bool cnt = std::find(value.begin(), value.end(), '.') != value.end();
    if (value.find_first_not_of("f0123456789.") == std::string::npos)
    {
        this->_rtn_rates.push_back("Error: bad input => " + value);
        return (false);
    }

    if (cnt == true)
    {
        // check in case there are more than one `.` in the value string
        std::string::iterator it = std::find(value.begin(), value.end(), '.');
        while (it != value.end())
        {
            it = std::find(it + 1, value.end(), '.');
            if (it != value.end())
            {
                this->_rtn_rates.push_back("Error: bad input => " + value + " is incorrect!");
                return (false);
            }
        }
    }
    value.erase(value.begin()); // in this way i am skipping the space after the vertical bar `|`
    // then we will check if there is minus operator or not
    if (value[0] == '-')
    {
        this->_rtn_rates.push_back("Error: not a positive number.");
        return (false);
    }

    double nbr = std::stod(value);
    if (nbr > INT_MAX)
    {
        this->_rtn_rates.push_back("Error: too large a number.");
        return (false);
    }
    _value.push_back(nbr);
    return (true);
}

void    BitcoinExchange::fill_the_map()
{
    // i need to fill this map with the data i have in the data.csv file
    // open data.csv file
    std::ifstream fs("data.csv");

    // check the existence of the file
    if (!fs.is_open())
    {
        std::cerr << "Error: Failed to open data.csv file!!" << std::endl;
        exit(1);
    }

    // check the first line syntax and s
    std::string line;
    if (!std::getline(fs, line) || line != "date,exchange_rate")
    {
        std::cerr << "Error: the first line doesn't match the line in the example!!" << std::endl;
        fs.close();
        exit(1);
    }

    // now insert the other data that looks like this "2017-01-01,1000.00" into the map 
    while (std::getline(fs, line))
    {
        size_t index = line.find(',');
        this->_exchange_rates.insert(std::pair<std::string, double>(line.substr(0, index), std::stod(line.substr(index + 1))));
    }

    // print the map
    // std::cout << "The map is: " << std::endl;
    // std::map<std::string, double>::iterator it = _exchange_rates.begin();
    // std::map<std::string, double>::iterator end = _exchange_rates.end();
    // while (it != end)
    // {
    //     std::cout << it->first << " => " << it->second << '\n';
    //     it++;
    // }
    // std::cout << std::endl;
}

void    BitcoinExchange::generate_report()
{
    // loop through the _date vector and call find_accurate_date function
    // then calculate the return rate
    // then print the report

    // for (size_t i = 0; i < _date.size(); i++)
        find_accurate_date();
}