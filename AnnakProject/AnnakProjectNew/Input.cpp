#include "Input.h"
#include "Utility.h"



std::string Input::next_line()
{
    std::string line = "";
    std::getline(std::cin, line);
    return line;
}

#include <fstream> // Include for file operations

void Input::parse_and_store()
{
    // Clear the file content at the beginning
    const std::string filePath = "world.txt";
    std::ofstream clearFile(filePath, std::ios::trunc);
    clearFile.close(); // Close the file to finish clearing its contents

    // Open the file in append mode to add lines as they are read
    std::ofstream outputFile(filePath, std::ios::app);

    // read first command
    auto line = next_line();
    line = next_line(); // This line solves the issue on the platform
    std::vector<std::shared_ptr<Command>>* command_list = &start;
    bool parsing_world = false;
    bool asserts_reached = false;
    while (!line.empty()) {
        strip(line);

        // Write the processed line to the file
        outputFile << line << std::endl;

        if (line.rfind("+", 0) == 0) { //order name
            // command block reached (e.g.: World, Infrastructure, etc...)
            std::string name = line.substr(1);
            parsing_world = false;
            if (name == Command::WORLD) {
                parsing_world = true;
            }
            else if (name == Command::START) {
                command_list = &start;
            }
            else if (name == Command::INPUT) {
                command_list = &steps;
            }
            else if (name == Command::ASSERTS) {
                asserts_reached = true;
            }
            else {
                throw std::runtime_error("Unknown Input Command found: " + name);
            }
        }
        else { //order input
            if (asserts_reached) {
                asserts.push_back(line);
            }
            else if (parsing_world) {
                auto row = split(line);
                world->data.push_back(row);
            }
            else {
                auto command = parse_command(line);
                command_list->push_back(command);
            }
        }
        // read next input line
        line = next_line();
    }

    outputFile.close(); // Close the file when done writing
}


std::shared_ptr<Command> Input::parse_command(std::string line)
{
    auto strings = split(line);
    std::shared_ptr<Command> command(new Command(strings[0]));
    command->arguments.insert(
        command->arguments.end(), strings.begin() + 1, strings.end()
    );
    return command;
}


std::ostream& operator<<(std::ostream& os, const Input& in) {
    os << "\nInput:\n" << "World Command: " << *(in.world) << "\n";
    os << "Start Commands:\n";
    for (const auto& cmd : in.start) {
        os << "  " << *cmd << "\n";
    }
    os << "Step Commands:\n";
    for (const auto& cmd : in.steps) {
        os << "  " << *cmd << "\n";
    }
    os << "Asserts:\n";
    for (const auto& assert : in.asserts) {
        os << "  " << assert << "\n";
    }
    return os;
}

