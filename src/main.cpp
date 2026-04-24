#include <algorithm>     // for sorting and removing items
#include <cctype>        // for character checks
#include <fstream>       // for saving and loading files (bonus)
#include <iomanip>       // for formatted tables
#include <iostream>      // the normal library for console(input and output)
#include <limits>        // for input handling
#include <sstream>       // for splitting text
#include <string>        // for text variables
#include <unordered_map> // just for courses lookup
#include <vector>        // for vector using

struct Student
{
    std::string id;
    std::string name;
    std::string nationalId;
    char gender{};
    std::string dob; // date of birth
    std::string phone;
    std::string program;
    int level{};
};

struct Course
{
    std::string code;
    std::string title;
    int credits{};
};

struct GradeRecord
{
    std::string studentId;
    std::string courseCode;
    int midterm{};
    int finalExam{};
    int total{};
    std::string letter; // A+ A A-
    double points{};    // 12.3
};

std::vector<Student> students; // [[ahmed, 03/03/2000, id, nid], [mahmoud, dob, nid]]
std::vector<Course> courses;   // [[CSE101, INTRODUCTION TO PROGRAMMING, 4], [PRO101, CRITICAL THINKING, 2]]
std::vector<GradeRecord> grades;
int nextStudentNumber = 1;
const std::string studentsFile = "students.txt";
const std::string coursesFile = "courses.txt";
const std::string gradesFile = "grades.txt";

// Removes leading and trailing whitespace from user input.
std::string trim(const std::string &text)
{
    int start = 0; // ""
    int end = static_cast<int>(text.size()) - 1;
    // "          ahmed hussein           '
    while (start <= end && (text[start] == ' ' || text[start] == '\t' || text[start] == '\n' || text[start] == '\r'))
    {
        start++;
    }

    while (end >= start && (text[end] == ' ' || text[end] == '\t' || text[end] == '\n' || text[end] == '\r'))
    {
        end--;
    }

    if (start > end)
    {
        return "";
    }

    return text.substr(start, end - start + 1);
}

// Converts a string to uppercase for case-insensitive comparisons.
std::string toUpper(std::string text)
{
    // csE ==> CSE
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch)
                   { return static_cast<char>(std::toupper(ch)); });
    return text;
}

// Converts a string to lowercase for case-insensitive comparisons.
std::string toLower(std::string text)
{
    // CSe ==> cse
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch)
                   { return static_cast<char>(std::tolower(ch)); });
    return text;
}

// Splits a sentence by spaces into individual words.
std::vector<std::string> splitWords(const std::string &text)
{
    std::istringstream input(text);
    std::vector<std::string> words;
    std::string word;
    while (input >> word)
    {
        words.push_back(word);
    }
    return words;
}

// Splits one text line using a delimiter for file parsing.
std::vector<std::string> splitText(const std::string &text, char delimiter)
{
    std::vector<std::string> parts;
    std::stringstream input(text);
    std::string piece;

    while (std::getline(input, piece, delimiter))
    {
        parts.push_back(piece);
    }

    return parts;
}

// Checks whether a string contains only digits.
bool isDigitsOnly(const std::string &text)
{
    if (text.empty())
    {
        return false;
    }

    for (char ch : text)
    {
        if (!std::isdigit(static_cast<unsigned char>(ch)))
        {
            return false;
        }
    }

    return true;
}

// Checks whether a year follows leap-year rules.
bool isLeapYear(int year)
{
    if (year % 400 == 0)
        return true;
    if (year % 100 == 0)
        return false;
    return year % 4 == 0;
}

// Validates that a day-month-year combination is a real calendar date.
bool validDate(int day, int month, int year)
{
    if (year < 1900 || month < 1 || month > 12 || day < 1)
        return false;
    int daysInMonth = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        daysInMonth = 30;
    if (month == 2)
        daysInMonth = isLeapYear(year) ? 29 : 28;
    return day <= daysInMonth;
}

// Calculates age using a fixed 2026 reference year.
int currentAgeYears(int day, int month, int year)
{
    (void)day;
    (void)month;
    return 2026 - year;
}

// Validates that a student name has exactly two alphabetic words.
bool validName(const std::string &text, std::string &error)
{
    const auto words = splitWords(trim(text));
    if (words.size() != 2)
    {
        error = "Name must be exactly two words";
        return false;
    }
    for (const auto &word : words)
    {
        for (unsigned char ch : word)
        {
            if (!std::isalpha(ch))
            {
                error = "Name must contain letters only";
                return false;
            }
        }
    }
    return true;
}

// Validates the national ID format and leading-digit constraint.
bool validNationalId(const std::string &text, std::string &error)
{
    const std::string value = trim(text);
    if (value.size() != 14 || !isDigitsOnly(value))
    {
        error = "National ID must be exactly 14 digits";
        return false;
    }
    if (value.front() == '0')
    {
        error = "National ID must not start with zero";
        return false;
    }
    return true;
}

// Validates and normalizes gender input to M or F.
bool validGender(const std::string &text, char &gender, std::string &error)
{
    std::string value = toUpper(trim(text));
    if (value == "M")
    {
        gender = 'M';
        return true;
    }
    if (value == "F")
    {
        gender = 'F';
        return true;
    }
    error = "Gender must be M or F";
    return false;
}

// Validates birth date format, calendar correctness, and minimum age.
bool validDob(const std::string &text, std::string &error)
{
    const std::string value = trim(text);
    const std::vector<std::string> parts = splitText(value, '/');
    if (parts.size() != 3 || parts[0].size() != 2 || parts[1].size() != 2 || parts[2].size() != 4)
    {
        error = "Date of Birth must be DD/MM/YYYY";
        return false;
    }

    if (!isDigitsOnly(parts[0]) || !isDigitsOnly(parts[1]) || !isDigitsOnly(parts[2]))
    {
        error = "Date of Birth must be DD/MM/YYYY";
        return false;
    }

    const int day = std::stoi(parts[0]);
    const int month = std::stoi(parts[1]);
    const int year = std::stoi(parts[2]);
    if (!validDate(day, month, year))
    {
        error = "Date of Birth is not valid";
        return false;
    }
    if (currentAgeYears(day, month, year) < 17)
    {
        error = "Student must be at least 17 years old";
        return false;
    }
    return true;
}

// Validates phone number format and required prefix.
bool validPhone(const std::string &text, std::string &error)
{
    const std::string value = trim(text);
    if (value.size() != 11 || value[0] != '0' || value[1] != '1' || !isDigitsOnly(value))
    {
        error = "Phone number must be 11 digits and start with 01";
        return false;
    }
    return true;
}

// Validates program code and returns normalized uppercase value.
bool validProgram(const std::string &text, std::string &program, std::string &error)
{
    const std::string value = toUpper(trim(text));
    if (value == "CSE" || value == "CCE" || value == "MCT")
    {
        program = value;
        return true;
    }
    error = "Program must be CSE, CCE, or MCT";
    return false;
}

// Validates academic level range.
bool validLevel(int level, std::string &error)
{
    if (level < 1 || level > 4)
    {
        error = "Academic level must be between 1 and 4";
        return false;
    }
    return true;
}

// Validates student ID format (26Pxxxx).
bool validStudentIdFormat(const std::string &text, std::string &error)
{
    const std::string value = toUpper(trim(text));
    if (value.size() != 7 || value.substr(0, 3) != "26P" || !isDigitsOnly(value.substr(3)))
    {
        error = "Invalid Student ID format";
        return false;
    }
    return true;
}

// Validates course code format and returns normalized uppercase value.
bool validCourseCode(const std::string &text, std::string &code, std::string &error)
{
    const std::string value = toUpper(trim(text));
    if (value.size() != 6 || !std::isalpha(static_cast<unsigned char>(value[0])) || !std::isalpha(static_cast<unsigned char>(value[1])) || !std::isalpha(static_cast<unsigned char>(value[2])) || !isDigitsOnly(value.substr(3)))
    {
        error = "Course code must be 3 letters and 3 digits";
        return false;
    }
    code = value;
    return true;
}

// Validates that course title is not empty.
bool validCourseTitle(const std::string &text, std::string &error)
{
    if (trim(text).empty())
    {
        error = "Course title cannot be empty";
        return false;
    }
    return true;
}

// Validates credit hour range.
bool validCredits(int credits, std::string &error)
{
    if (credits < 1 || credits > 4)
    {
        error = "Credit hours must be between 1 and 4";
        return false;
    }
    return true;
}

// Validates midterm grade range.
bool validMidterm(int grade, std::string &error)
{
    if (grade < 0 || grade > 40)
    {
        error = "Midterm grade must be between 0 and 40";
        return false;
    }
    return true;
}

// Validates final exam grade range.
bool validFinal(int grade, std::string &error)
{
    if (grade < 0 || grade > 60)
    {
        error = "Final grade must be between 0 and 60";
        return false;
    }
    return true;
}

// Converts numeric total grade into a letter grade.
std::string calculateLetterGrade(int total)
{
    if (total >= 85)
        return "A";
    if (total >= 75)
        return "B";
    if (total >= 65)
        return "C";
    if (total >= 50)
        return "D";
    return "F";
}

// Converts letter grade into grade points used in GPA.
double letterToPoints(const std::string &letter)
{
    if (letter == "A")
        return 4.0;
    if (letter == "B")
        return 3.0;
    if (letter == "C")
        return 2.0;
    if (letter == "D")
        return 1.0;
    return 0.0;
}

// Recomputes the next generated student number from loaded records.
void updateNextStudentNumber()
{
    int highestNumber = 0;
    for (const Student &student : students)
    {
        if (student.id.size() == 7 && student.id.rfind("26P", 0) == 0)
        {
            const std::string suffix = student.id.substr(3);
            if (suffix.size() == 4 && isDigitsOnly(suffix))
            {
                highestNumber = std::max(highestNumber, std::stoi(suffix));
            }
        }
    }

    nextStudentNumber = highestNumber + 1;
    if (nextStudentNumber < 1)
    {
        nextStudentNumber = 1;
    }
}

// Writes all student records to students.txt.
void saveStudents()
{
    std::ofstream outFile(studentsFile);
    for (const Student &student : students)
    {
        outFile << student.id << '|' << student.name << '|' << student.nationalId << '|' << student.gender << '|' << student.dob << '|' << student.phone << '|' << student.program << '|' << student.level << '\n';
    }
}

// Writes all course records to courses.txt.
void saveCourses()
{
    std::ofstream outFile(coursesFile);
    for (const Course &course : courses)
    {
        outFile << course.code << '|' << course.title << '|' << course.credits << '\n';
    }
}

// Writes all grade records to grades.txt.
void saveGrades()
{
    std::ofstream outFile(gradesFile);
    for (const GradeRecord &record : grades)
    {
        outFile << record.studentId << '|' << record.courseCode << '|' << record.midterm << '|' << record.finalExam << '|' << record.total << '|' << record.letter << '|' << record.points << '\n';
    }
}

// Saves students, courses, and grades files together.
void saveAllData()
{
    saveStudents();
    saveCourses();
    saveGrades();
}

// Loads students from file and rebuilds the next student ID counter.
void loadStudents()
{
    std::ifstream inFile(studentsFile);
    if (!inFile.is_open())
    {
        return;
    }

    students.clear();
    std::string line;
    while (std::getline(inFile, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        const std::vector<std::string> parts = splitText(line, '|');
        if (parts.size() != 8)
        {
            continue;
        }

        try
        {
            Student student;
            student.id = parts[0];
            student.name = parts[1];
            student.nationalId = parts[2];
            student.gender = parts[3].empty() ? 'M' : parts[3][0];
            student.dob = parts[4];
            student.phone = parts[5];
            student.program = parts[6];
            student.level = std::stoi(parts[7]);
            students.push_back(student);
        }
        catch (...)
        {
            continue;
        }
    }

    updateNextStudentNumber();
}

// Loads courses from file.
void loadCourses()
{
    std::ifstream inFile(coursesFile);
    if (!inFile.is_open())
    {
        return;
    }

    courses.clear();
    std::string line;
    while (std::getline(inFile, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        const std::vector<std::string> parts = splitText(line, '|');
        if (parts.size() != 3)
        {
            continue;
        }

        try
        {
            Course course;
            course.code = parts[0];
            course.title = parts[1];
            course.credits = std::stoi(parts[2]);
            courses.push_back(course);
        }
        catch (...)
        {
            continue;
        }
    }
}

// Loads grades from file.
void loadGrades()
{
    std::ifstream inFile(gradesFile);
    if (!inFile.is_open())
    {
        return;
    }

    grades.clear();
    std::string line;
    while (std::getline(inFile, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        const std::vector<std::string> parts = splitText(line, '|');
        if (parts.size() != 7)
        {
            continue;
        }

        try
        {
            GradeRecord record;
            record.studentId = parts[0];
            record.courseCode = parts[1];
            record.midterm = std::stoi(parts[2]);
            record.finalExam = std::stoi(parts[3]);
            record.total = std::stoi(parts[4]);
            record.letter = parts[5];
            record.points = std::stod(parts[6]);
            grades.push_back(record);
        }
        catch (...)
        {
            continue;
        }
    }
}

// Loads all persisted data files at startup.
void loadAllData()
{
    loadStudents();
    loadCourses();
    loadGrades();
}

// Finds a student index by student ID.
int findStudentById(const std::string &id)
{
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (students[i].id == id)
            return static_cast<int>(i);
    }
    return -1;
}

// Finds a student index by national ID.
int findStudentByNationalId(const std::string &nationalId)
{
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (students[i].nationalId == nationalId)
            return static_cast<int>(i);
    }
    return -1;
}

// Finds all students whose names match the search term.
std::vector<int> findStudentsByName(const std::string &name)
{
    std::vector<int> matches;
    const std::string lookup = toLower(trim(name));
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (toLower(trim(students[i].name)) == lookup)
        {
            matches.push_back(static_cast<int>(i));
        }
    }
    return matches;
}

// Finds a course index by course code.
int findCourseByCode(const std::string &code)
{
    for (size_t i = 0; i < courses.size(); ++i)
    {
        if (courses[i].code == code)
            return static_cast<int>(i);
    }
    return -1;
}

// Generates the next student ID in 26Pxxxx format.
std::string generateStudentId()
{
    std::ostringstream out;
    out << "26P" << std::setw(4) << std::setfill('0') << nextStudentNumber++;
    return out.str();
}

// Reads and validates an integer from console input.
int readInt(const std::string &prompt)
{
    while (true)
    {
        std::string text;
        std::cout << prompt;
        std::getline(std::cin, text);
        text = trim(text);
        if (text.empty())
        {
            std::cout << "ERROR: Value cannot be empty\n";
            continue;
        }
        std::stringstream ss(text);
        int value = 0;
        char extra = '\0';
        if ((ss >> value) && !(ss >> extra))
        {
            return value;
        }
        std::cout << "ERROR: Please enter a valid number\n";
    }
}

// Reads one text line from console input.
std::string readText(const std::string &prompt)
{
    std::string text;
    std::cout << prompt;
    std::getline(std::cin, text);
    return trim(text);
}

// Prints a horizontal separator line for table output.
void printLine(int width)
{
    for (int i = 0; i < width; ++i)
    {
        std::cout << "-";
    }
    std::cout << "\n";
}

// Prints one full student card with summary information.
void showStudentCard(const Student &student, double gpa)
{
    std::cout << "---------------------------------------------\n";
    std::cout << "Student ID: " << student.id << "\n";
    std::cout << "Name: " << student.name << "\n";
    std::cout << "National ID: " << student.nationalId << "\n";
    std::cout << "Gender: " << student.gender << " | DOB: " << student.dob << "\n";
    std::cout << "Phone: " << student.phone << "\n";
    std::cout << "Program: " << student.program << " | Level: " << student.level;
    std::cout << " | GPA: " << std::fixed << std::setprecision(2) << gpa << "\n";
    std::cout << "---------------------------------------------\n";
}

// Calculates a student's GPA using credit-weighted grade points.
double calculateGpa(const std::string &studentId)
{
    double totalPoints = 0.0;
    int totalCredits = 0;

    for (const GradeRecord &record : grades)
    {
        if (record.studentId != studentId)
            continue;

        int courseIndex = findCourseByCode(record.courseCode);
        if (courseIndex == -1)
            continue;

        const int credits = courses[courseIndex].credits;
        totalCredits += credits;
        totalPoints += record.points * credits;
    }

    if (totalCredits == 0)
        return 0.0;
    return totalPoints / totalCredits;
}

// Collects all grade rows that belong to one student.
std::vector<const GradeRecord *> getStudentGrades(const std::string &studentId)
{
    std::vector<const GradeRecord *> found;
    for (const auto &record : grades)
    {
        if (record.studentId == studentId)
        {
            found.push_back(&record);
        }
    }
    return found;
}

// Adds a new student after validating all required fields.
void addStudent()
{
    Student student;
    std::string error;

    while (true)
    {
        student.name = readText("Enter Student Name: ");
        if (validName(student.name, error))
            break;
        std::cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        student.nationalId = readText("Enter National ID: ");
        if (!validNationalId(student.nationalId, error))
        {
            std::cout << "ERROR: " << error << "\n";
            continue;
        }
        if (findStudentByNationalId(student.nationalId) != -1)
        {
            std::cout << "ERROR: A student with this National ID already exists\n";
            continue;
        }
        break;
    }

    while (true)
    {
        std::string genderText = readText("Enter Gender (M/F): ");
        char gender = 'M';
        if (validGender(genderText, gender, error))
        {
            student.gender = gender;
            break;
        }
        std::cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        student.dob = readText("Enter Date of Birth (DD/MM/YYYY): ");
        if (validDob(student.dob, error))
            break;
        std::cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        student.phone = readText("Enter Phone Number: ");
        if (validPhone(student.phone, error))
            break;
        std::cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        std::string programText = readText("Enter Program (CSE/CCE/MCT): ");
        std::string program;
        if (validProgram(programText, program, error))
        {
            student.program = program;
            break;
        }
        std::cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        int level = readInt("Enter Academic Level (1-4): ");
        if (validLevel(level, error))
        {
            student.level = level;
            break;
        }
        std::cout << "ERROR: " << error << "\n";
    }

    student.id = generateStudentId();
    students.push_back(student);
    saveStudents();
    std::cout << "Student added successfully!\n";
    std::cout << "Student ID: " << student.id << "\n";
}

// Searches for students by ID, name, or national ID.
void searchStudent()
{
    while (true)
    {
        std::cout << "\n=== Search Student ===\n";
        std::cout << "1. Search by Student ID\n";
        std::cout << "2. Search by Name\n";
        std::cout << "3. Search by National ID\n";
        std::cout << "4. Back\n";

        int choice = readInt("Enter your choice: ");
        if (choice == 4)
            return;

        std::string error;
        std::vector<int> matches;

        if (choice == 1)
        {
            std::string id = toUpper(readText("Enter Student ID: "));
            if (!validStudentIdFormat(id, error))
            {
                std::cout << "Invalid Search Value: " << error << "\n";
                continue;
            }
            int index = findStudentById(id);
            if (index != -1)
                matches.push_back(index);
        }
        else if (choice == 2)
        {
            std::string name = readText("Enter student name to search: ");
            if (!validName(name, error))
            {
                std::cout << "Invalid Search Value: " << error << "\n";
                continue;
            }
            matches = findStudentsByName(name);
        }
        else if (choice == 3)
        {
            std::string nationalId = readText("Enter National ID: ");
            if (!validNationalId(nationalId, error))
            {
                std::cout << "Invalid Search Value: " << error << "\n";
                continue;
            }
            int index = findStudentByNationalId(nationalId);
            if (index != -1)
                matches.push_back(index);
        }

        if (matches.empty())
        {
            std::cout << "Student Not Found: No student found with the given criteria\n";
            continue;
        }

        std::cout << "Found " << matches.size() << " student(s):\n\n";
        for (int index : matches)
        {
            showStudentCard(students[index], calculateGpa(students[index].id));
            std::cout << "\n";
        }
    }
}

// Updates selected editable fields for an existing student.
void updateStudent()
{
    std::string error;
    std::string id = toUpper(readText("Enter Student ID to update: "));
    if (!validStudentIdFormat(id, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findStudentById(id);
    if (index == -1)
    {
        std::cout << "Student Not Found: No student exists with ID: " << id << "\n";
        return;
    }

    Student &student = students[index];
    std::cout << "Student found: " << student.name << "\n";
    std::cout << "1. Phone Number\n2. Program\n3. Academic Level\n4. Cancel\n";

    int choice = readInt("Enter your choice: ");
    if (choice == 4)
        return;

    if (choice == 1)
    {
        while (true)
        {
            std::string phone = readText("Enter new Phone Number: ");
            if (validPhone(phone, error))
            {
                student.phone = phone;
                break;
            }
            std::cout << "Invalid New Value: " << error << "\n";
        }
    }
    else if (choice == 2)
    {
        while (true)
        {
            std::string programText = readText("Enter new Program (CSE/CCE/MCT): ");
            std::string program;
            if (validProgram(programText, program, error))
            {
                student.program = program;
                break;
            }
            std::cout << "Invalid New Value: " << error << "\n";
        }
    }
    else if (choice == 3)
    {
        while (true)
        {
            int level = readInt("Enter new Academic Level (1-4): ");
            if (validLevel(level, error))
            {
                student.level = level;
                break;
            }
            std::cout << "Invalid New Value: " << error << "\n";
        }
    }

    std::cout << "Operation Successful: Student record updated successfully\n";
}

// Deletes a student and removes linked grade records.
void deleteStudent()
{
    std::string error;
    std::string id = toUpper(readText("Enter Student ID to delete: "));
    if (!validStudentIdFormat(id, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findStudentById(id);
    if (index == -1)
    {
        std::cout << "ID Not Found: No student exists with ID: " << id << "\n";
        return;
    }

    std::cout << "Student found: " << students[index].name << "\n";
    std::string confirm = toUpper(trim(readText("Are you sure you want to delete this student? (Y/N): ")));
    if (confirm != "Y")
    {
        std::cout << "Operation Cancelled\n";
        return;
    }

    students.erase(students.begin() + index);
    grades.erase(std::remove_if(grades.begin(), grades.end(), [&](const GradeRecord &record)
                                { return record.studentId == id; }),
                 grades.end());
    saveAllData();

    std::cout << "Operation Successful: Student deleted successfully\n";
}

// Lists all students with selectable sort options.
void listStudents()
{
    saveStudents();
    if (students.empty())
    {
        std::cout << "No students available\n";
        return;
    }

    std::cout << "\n=== List All Students ===\n";
    std::cout << "1. Student ID\n2. Name (A-Z)\n3. GPA (Highest First)\n4. Back\n";
    int choice = readInt("Enter your choice: ");
    if (choice == 4)
        return;

    std::vector<int> order;
    for (size_t i = 0; i < students.size(); ++i)
    {
        order.push_back(static_cast<int>(i));
    }

    if (choice == 1)
    {
        std::sort(order.begin(), order.end(), [](int a, int b)
                  { return students[a].id < students[b].id; });
    }
    else if (choice == 2)
    {
        std::sort(order.begin(), order.end(), [](int a, int b)
                  { return toLower(trim(students[a].name)) < toLower(trim(students[b].name)); });
    }
    else if (choice == 3)
    {
        std::sort(order.begin(), order.end(), [](int a, int b)
                  {
            double gpaA = calculateGpa(students[a].id);
            double gpaB = calculateGpa(students[b].id);
            if (gpaA == gpaB) return students[a].id < students[b].id;
            return gpaA > gpaB; });
    }

    std::cout << "Total Students: " << students.size() << "\n\n";
    std::cout << std::left << std::setw(10) << "ID" << std::setw(22) << "Name" << std::setw(10) << "Program" << std::setw(8) << "Level" << std::setw(8) << "GPA" << "\n";
    printLine(58);
    for (int index : order)
    {
        std::cout << std::left << std::setw(10) << students[index].id
                  << std::setw(22) << students[index].name
                  << std::setw(10) << students[index].program
                  << std::setw(8) << students[index].level
                  << std::setw(8) << std::fixed << std::setprecision(2) << calculateGpa(students[index].id) << "\n";
    }
}

// Adds a new course after validation and uniqueness checks.
void addCourse()
{
    Course course;
    std::string error;

    while (true)
    {
        std::string codeText = readText("Enter Course Code: ");
        std::string code;
        if (!validCourseCode(codeText, code, error))
        {
            std::cout << "ERROR: " << error << "\n";
            continue;
        }
        if (findCourseByCode(code) != -1)
        {
            std::cout << "ERROR: Course Code must be UNIQUE\n";
            continue;
        }
        course.code = code;
        break;
    }

    while (true)
    {
        course.title = readText("Enter Course Title: ");
        if (validCourseTitle(course.title, error))
            break;
        std::cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        int credits = readInt("Enter Credit Hours (1-4): ");
        if (validCredits(credits, error))
        {
            course.credits = credits;
            break;
        }
        std::cout << "ERROR: " << error << "\n";
    }

    courses.push_back(course);
    saveCourses();
    std::cout << "Course added successfully!\n";
}

// Displays all courses in code order.
void viewCourses()
{
    if (courses.empty())
    {
        std::cout << "No courses available\n";
        return;
    }

    std::vector<Course> sortedCourses = courses;
    std::sort(sortedCourses.begin(), sortedCourses.end(), [](const Course &a, const Course &b)
              { return a.code < b.code; });

    std::cout << "\n=== All Courses ===\n";
    std::cout << std::left << std::setw(12) << "Code" << std::setw(35) << "Title" << std::setw(8) << "Credit" << "\n";
    printLine(55);
    for (const Course &course : sortedCourses)
    {
        std::cout << std::left << std::setw(12) << course.code << std::setw(35) << course.title << std::setw(8) << course.credits << "\n";
    }
}

// Updates either title or credit hours for a selected course.
void updateCourse()
{
    std::string error;
    std::string code;
    if (!validCourseCode(readText("Enter Course Code to update: "), code, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findCourseByCode(code);
    if (index == -1)
    {
        std::cout << "Course Not Found\n";
        return;
    }

    Course &course = courses[index];
    std::cout << "Course found: " << course.title << "\n";
    std::cout << "1. Update Course Title\n2. Update Credit Hours\n3. Cancel\n";
    int choice = readInt("Enter your choice: ");
    if (choice == 3)
        return;

    if (choice == 1)
    {
        while (true)
        {
            std::string title = readText("Enter new Course Title: ");
            if (validCourseTitle(title, error))
            {
                course.title = title;
                break;
            }
            std::cout << "ERROR: " << error << "\n";
        }
    }
    else if (choice == 2)
    {
        while (true)
        {
            int credits = readInt("Enter new Credit Hours (1-4): ");
            if (validCredits(credits, error))
            {
                course.credits = credits;
                break;
            }
            std::cout << "ERROR: " << error << "\n";
        }
    }

    std::cout << "Course updated successfully!\n";
}

// Deletes a course and removes linked grade records.
void deleteCourse()
{
    std::string error;
    std::string code;
    if (!validCourseCode(readText("Enter Course Code to delete: "), code, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findCourseByCode(code);
    if (index == -1)
    {
        std::cout << "Course Not Found\n";
        return;
    }

    std::string confirm = toUpper(trim(readText("Are you sure you want to delete this course? (Y/N): ")));
    if (confirm != "Y")
    {
        std::cout << "Operation cancelled\n";
        return;
    }

    courses.erase(courses.begin() + index);
    grades.erase(std::remove_if(grades.begin(), grades.end(), [&](const GradeRecord &record)
                                { return record.courseCode == code; }),
                 grades.end());
    saveAllData();

    std::cout << "Course deleted successfully!\n";
}

// Adds or updates one student's grade entry for a course.
void enterGrades()
{
    saveCourses();
    std::string error;
    std::string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }
    if (findStudentById(studentId) == -1)
    {
        std::cout << "ERROR: Student ID must exist in the system\n";
        return;
    }

    std::string courseCode;
    if (!validCourseCode(readText("Enter Course Code: "), courseCode, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }
    if (findCourseByCode(courseCode) == -1)
    {
        std::cout << "ERROR: Course Code must exist in the system\n";
        return;
    }

    int midterm = 0;
    while (true)
    {
        midterm = readInt("Enter Midterm Grade (0-40): ");
        if (validMidterm(midterm, error))
            break;
        std::cout << "ERROR: " << error << "\n";
    }

    int finalExam = 0;
    while (true)
    {
        finalExam = readInt("Enter Final Grade (0-60): ");
        if (validFinal(finalExam, error))
            break;
        std::cout << "ERROR: " << error << "\n";
    }

    const int total = midterm + finalExam;
    const std::string letter = calculateLetterGrade(total);
    const double points = letterToPoints(letter);

    for (auto &record : grades)
    {
        if (record.studentId == studentId && record.courseCode == courseCode)
        {
            record.midterm = midterm;
            record.finalExam = finalExam;
            record.total = total;
            record.letter = letter;
            record.points = points;
            std::cout << "Grade updated successfully!\n";
            std::cout << "Total: " << total << "/100\n";
            std::cout << "Letter Grade: " << letter << "\n";
            saveGrades();
            return;
        }
    }

    grades.push_back({studentId, courseCode, midterm, finalExam, total, letter, points});
    std::cout << "Grade recorded successfully!\n";
    std::cout << "Total: " << total << "/100\n";
    std::cout << "Letter Grade: " << letter << "\n";
    saveGrades();
}

// Displays all stored grades for one student.
void viewGrades()
{
    std::string error;
    std::string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }

    int studentIndex = findStudentById(studentId);
    if (studentIndex == -1)
    {
        std::cout << "ERROR: Student not found\n";
        return;
    }

    auto records = getStudentGrades(studentId);
    if (records.empty())
    {
        std::cout << "No grades recorded for this student yet\n";
        return;
    }

    std::cout << "\nGrades for " << students[studentIndex].name << " (" << studentId << ")\n";
    std::cout << std::left << std::setw(12) << "Course" << std::setw(30) << "Title" << std::setw(10) << "Midterm" << std::setw(8) << "Final" << std::setw(8) << "Total" << std::setw(8) << "Grade" << "\n";
    printLine(76);

    for (const GradeRecord *record : records)
    {
        std::string title = "N/A";
        int courseIndex = findCourseByCode(record->courseCode);
        if (courseIndex != -1)
        {
            title = courses[courseIndex].title;
        }
        std::cout << std::left << std::setw(12) << record->courseCode << std::setw(30) << title << std::setw(10) << record->midterm << std::setw(8) << record->finalExam << std::setw(8) << record->total << std::setw(8) << record->letter << "\n";
    }
}

// Shows GPA for one student.
void calculateGpaMenu()
{
    std::string error;
    std::string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }
    int studentIndex = findStudentById(studentId);
    if (studentIndex == -1)
    {
        std::cout << "ERROR: Student not found\n";
        return;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Cumulative GPA for " << students[studentIndex].name << " is: " << calculateGpa(studentId) << "\n";
}

// Prints a transcript-style view for one student.
void transcriptMenu()
{
    std::string error;
    std::string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        std::cout << "ERROR: " << error << "\n";
        return;
    }

    int studentIndex = findStudentById(studentId);
    if (studentIndex == -1)
    {
        std::cout << "ERROR: Student not found\n";
        return;
    }

    std::vector<const GradeRecord *> records = getStudentGrades(studentId);
    std::sort(records.begin(), records.end(), [](const GradeRecord *a, const GradeRecord *b)
              { return a->courseCode < b->courseCode; });

    std::cout << "\n=============================================\n";
    std::cout << "             STUDENT TRANSCRIPT\n";
    std::cout << "=============================================\n";
    std::cout << "Student ID: " << students[studentIndex].id << "\n";
    std::cout << "Name: " << students[studentIndex].name << "\n";
    std::cout << "Program: " << students[studentIndex].program << " | Level: " << students[studentIndex].level << "\n\n";

    std::cout << std::left << std::setw(12) << "Course" << std::setw(30) << "Title" << std::setw(8) << "Cred" << std::setw(8) << "Grade" << std::setw(8) << "Pts" << "\n";
    printLine(66);

    int totalCredits = 0;
    if (records.empty())
    {
        std::cout << "No grade records available\n";
    }
    else
    {
        for (const GradeRecord *record : records)
        {
            std::string title = "N/A";
            int credits = 0;
            int courseIndex = findCourseByCode(record->courseCode);
            if (courseIndex != -1)
            {
                title = courses[courseIndex].title;
                credits = courses[courseIndex].credits;
            }
            totalCredits += credits;
            std::cout << std::left << std::setw(12) << record->courseCode << std::setw(30) << title << std::setw(8) << credits << std::setw(8) << record->letter << std::setw(8) << std::fixed << std::setprecision(1) << record->points << "\n";
        }
    }

    std::cout << "\nTotal Credit Hours: " << totalCredits << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Cumulative GPA: " << calculateGpa(studentId) << "\n";
}

// Runs the Student Management submenu loop.
void studentMenu()
{
    while (true)
    {
        std::cout << "\n=== Student Management ===\n";
        std::cout << "1. Add New Student\n2. Search Student\n3. Update Student\n4. Delete Student\n5. List All Students\n6. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ");
        if (choice == 6)
            return;
        if (choice == 1)
            addStudent();
        else if (choice == 2)
            searchStudent();
        else if (choice == 3)
            updateStudent();
        else if (choice == 4)
            deleteStudent();
        else if (choice == 5)
            listStudents();
    }
}

// Runs the Course Management submenu loop.
void courseMenu()
{
    while (true)
    {
        std::cout << "\n=== Course Management ===\n";
        std::cout << "1. Add New Course\n2. View All Courses\n3. Update Course\n4. Delete Course\n5. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ");
        if (choice == 5)
            return;
        if (choice == 1)
            addCourse();
        else if (choice == 2)
            viewCourses();
        else if (choice == 3)
            updateCourse();
        else if (choice == 4)
            deleteCourse();
    }
}

// Runs the Grades Management submenu loop.
void gradesMenu()
{
    while (true)
    {
        std::cout << "\n=== Grades Management ===\n";
        std::cout << "1. Enter Student Grades\n2. View Student Grades\n3. Calculate GPA\n4. Generate Transcript\n5. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ");
        if (choice == 5)
            return;
        if (choice == 1)
            enterGrades();
        else if (choice == 2)
            viewGrades();
        else if (choice == 3)
            calculateGpaMenu();
        else if (choice == 4)
            transcriptMenu();
    }
}

// Runs the main menu loop and orchestrates startup/shutdown persistence.
int main()
{
    // Simple console loop for the whole system.
    loadAllData();
    while (true)
    {
        std::cout << "\n======================================\n";
        std::cout << "       STUDENT INFORMATION SYSTEM\n";
        std::cout << "======================================\n";
        std::cout << "1. Student Management\n2. Course Management\n3. Grades Management\n4. Exit\n";

        int choice = readInt("Enter your choice: ");
        if (choice == 4)
        {
            std::cout << "Exiting system...\n";
            saveAllData();
            break;
        }
        if (choice == 1)
            studentMenu();
        else if (choice == 2)
            courseMenu();
        else if (choice == 3)
            gradesMenu();
    }

    return 0;
}
