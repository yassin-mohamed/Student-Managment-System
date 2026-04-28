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

using namespace std;

struct Student
{
    string id;
    string name;
    string nationalId;
    char gender{};
    string dob; // date of birth
    string phone;
    string program;
    int level{};
};

struct Course
{
    string code;
    string title;
    int credits{};
};

struct GradeRecord
{
    string studentId;
    string courseCode;
    int midterm{};
    int finalExam{};
    int total{};
    string letter;   // A+ A A-
    double points{}; // 12.3
};

vector<Student> students; // [[ahmed, 03/03/2000, id, nid], [mahmoud, dob, nid]]
vector<Course> courses;   // [[CSE101, INTRODUCTION TO PROGRAMMING, 4], [PRO101, CRITICAL THINKING, 2]]
vector<GradeRecord> grades;
int nextStudentNumber = 1;
const string studentsFile = "students.txt";
const string coursesFile = "courses.txt";
const string gradesFile = "grades.txt";

// Removes leading and trailing whitespace from user input.
string trim(const string &text)
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
string toUpper(string text)
{
    // csE ==> CSE
    transform(text.begin(), text.end(), text.begin(), [](unsigned char ch)
              { return static_cast<char>(toupper(ch)); });
    return text;
}

// Converts a string to lowercase for case-insensitive comparisons.
string toLower(string text)
{
    // CSe ==> cse
    transform(text.begin(), text.end(), text.begin(), [](unsigned char ch)
              { return static_cast<char>(tolower(ch)); });
    return text;
}

// Splits a sentence by spaces into individual words.
vector<string> splitWords(const string &text)
{
    istringstream input(text);
    vector<string> words;
    string word;
    while (input >> word)
    {
        words.push_back(word);
    }
    return words;
}

// Splits one text line using a delimiter for file parsing.
vector<string> splitText(const string &text, char delimiter)
{
    vector<string> parts;
    stringstream input(text);
    string piece;

    while (getline(input, piece, delimiter))
    {
        parts.push_back(piece);
    }

    return parts;
}

// Checks whether a string contains only digits.
bool isDigitsOnly(const string &text)
{
    if (text.empty())
    {
        return false;
    }

    for (char ch : text)
    {
        if (!isdigit(static_cast<unsigned char>(ch)))
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
bool validName(const string &text, string &error)
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
            if (!isalpha(ch))
            {
                error = "Name must contain letters only";
                return false;
            }
        }
    }
    return true;
}

// Validates the national ID format and leading-digit constraint.
bool validNationalId(const string &text, string &error)
{
    const string value = trim(text);
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
bool validGender(const string &text, char &gender, string &error)
{
    string value = toUpper(trim(text));
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
bool validDob(const string &text, string &error)
{
    const string value = trim(text);
    const vector<string> parts = splitText(value, '/');
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

    const int day = stoi(parts[0]);
    const int month = stoi(parts[1]);
    const int year = stoi(parts[2]);
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
bool validPhone(const string &text, string &error)
{
    const string value = trim(text);
    if (value.size() != 11 || value[0] != '0' || value[1] != '1' || !isDigitsOnly(value))
    {
        error = "Phone number must be 11 digits and start with 01";
        return false;
    }
    return true;
}

// Validates program code and returns normalized uppercase value.
bool validProgram(const string &text, string &program, string &error)
{
    const string value = toUpper(trim(text));
    if (value == "CSE" || value == "CCE" || value == "MCT")
    {
        program = value;
        return true;
    }
    error = "Program must be CSE, CCE, or MCT";
    return false;
}

// Validates academic level range.
bool validLevel(int level, string &error)
{
    if (level < 1 || level > 4)
    {
        error = "Academic level must be between 1 and 4";
        return false;
    }
    return true;
}

// Validates student ID format (26Pxxxx).
bool validStudentIdFormat(const string &text, string &error)
{
    const string value = toUpper(trim(text));
    if (value.size() != 7 || value.substr(0, 3) != "26P" || !isDigitsOnly(value.substr(3)))
    {
        error = "Invalid Student ID format";
        return false;
    }
    return true;
}

// Validates course code format and returns normalized uppercase value.
bool validCourseCode(const string &text, string &code, string &error)
{
    const string value = toUpper(trim(text));
    if (value.size() != 6 || !isalpha(static_cast<unsigned char>(value[0])) || !isalpha(static_cast<unsigned char>(value[1])) || !isalpha(static_cast<unsigned char>(value[2])) || !isDigitsOnly(value.substr(3)))
    {
        error = "Course code must be 3 letters and 3 digits";
        return false;
    }
    code = value;
    return true;
}

// Validates that course title is not empty.
bool validCourseTitle(const string &text, string &error)
{
    if (trim(text).empty())
    {
        error = "Course title cannot be empty";
        return false;
    }
    return true;
}

// Validates credit hour range.
bool validCredits(int credits, string &error)
{
    if (credits < 1 || credits > 4)
    {
        error = "Credit hours must be between 1 and 4";
        return false;
    }
    return true;
}

// Validates midterm grade range.
bool validMidterm(int grade, string &error)
{
    if (grade < 0 || grade > 40)
    {
        error = "Midterm grade must be between 0 and 40";
        return false;
    }
    return true;
}

// Validates final exam grade range.
bool validFinal(int grade, string &error)
{
    if (grade < 0 || grade > 60)
    {
        error = "Final grade must be between 0 and 60";
        return false;
    }
    return true;
}

// Converts numeric total grade into a letter grade.
string calculateLetterGrade(int total)
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
double letterToPoints(const string &letter)
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
            const string suffix = student.id.substr(3);
            if (suffix.size() == 4 && isDigitsOnly(suffix))
            {
                highestNumber = max(highestNumber, stoi(suffix));
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
    ofstream outFile(studentsFile);
    for (const Student &student : students)
    {
        outFile << student.id << '|' << student.name << '|' << student.nationalId << '|' << student.gender << '|' << student.dob << '|' << student.phone << '|' << student.program << '|' << student.level << '\n';
    }
}

// Writes all course records to courses.txt.
void saveCourses()
{
    ofstream outFile(coursesFile);
    for (const Course &course : courses)
    {
        outFile << course.code << '|' << course.title << '|' << course.credits << '\n';
    }
}

// Writes all grade records to grades.txt.
void saveGrades()
{
    ofstream outFile(gradesFile);
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
    ifstream inFile(studentsFile);
    if (!inFile.is_open())
    {
        return;
    }

    students.clear();
    string line;
    while (getline(inFile, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        const vector<string> parts = splitText(line, '|');
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
            student.level = stoi(parts[7]);
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
    ifstream inFile(coursesFile);
    if (!inFile.is_open())
    {
        return;
    }

    courses.clear();
    string line;
    while (getline(inFile, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        const vector<string> parts = splitText(line, '|');
        if (parts.size() != 3)
        {
            continue;
        }

        try
        {
            Course course;
            course.code = parts[0];
            course.title = parts[1];
            course.credits = stoi(parts[2]);
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
    ifstream inFile(gradesFile);
    if (!inFile.is_open())
    {
        return;
    }

    grades.clear();
    string line;
    while (getline(inFile, line))
    {
        if (trim(line).empty())
        {
            continue;
        }

        const vector<string> parts = splitText(line, '|');
        if (parts.size() != 7)
        {
            continue;
        }

        try
        {
            GradeRecord record;
            record.studentId = parts[0];
            record.courseCode = parts[1];
            record.midterm = stoi(parts[2]);
            record.finalExam = stoi(parts[3]);
            record.total = stoi(parts[4]);
            record.letter = parts[5];
            record.points = stod(parts[6]);
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
int findStudentById(const string &id)
{
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (students[i].id == id)
            return static_cast<int>(i);
    }
    return -1;
}

// Finds a student index by national ID.
int findStudentByNationalId(const string &nationalId)
{
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (students[i].nationalId == nationalId)
            return static_cast<int>(i);
    }
    return -1;
}

// Finds all students whose names match the search term.
vector<int> findStudentsByName(const string &name)
{
    vector<int> matches;
    const string lookup = toLower(trim(name));
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
int findCourseByCode(const string &code)
{
    for (size_t i = 0; i < courses.size(); ++i)
    {
        if (courses[i].code == code)
            return static_cast<int>(i);
    }
    return -1;
}

// Generates the next student ID in 26Pxxxx format.
string generateStudentId()
{
    ostringstream out;
    out << "26P" << setw(4) << setfill('0') << nextStudentNumber++;
    return out.str();
}

// Reads and validates an integer from console input.
int readInt(const string &prompt)
{
    while (true)
    {
        string text;
        cout << prompt;
        getline(cin, text);
        text = trim(text);
        if (text.empty())
        {
            cout << "ERROR: Value cannot be empty\n";
            continue;
        }
        stringstream ss(text);
        int value = 0;
        char extra = '\0';
        if ((ss >> value) && !(ss >> extra))
        {
            return value;
        }
        cout << "ERROR: Please enter a valid number\n";
    }
}

// Reads one text line from console input.
string readText(const string &prompt)
{
    string text;
    cout << prompt;
    getline(cin, text);
    return trim(text);
}

// Prints a horizontal separator line for table output.
void printLine(int width)
{
    for (int i = 0; i < width; ++i)
    {
        cout << "-";
    }
    cout << "\n";
}

// Prints one full student card with summary information.
void showStudentCard(const Student &student, double gpa)
{
    cout << "---------------------------------------------\n";
    cout << "Student ID: " << student.id << "\n";
    cout << "Name: " << student.name << "\n";
    cout << "National ID: " << student.nationalId << "\n";
    cout << "Gender: " << student.gender << " | DOB: " << student.dob << "\n";
    cout << "Phone: " << student.phone << "\n";
    cout << "Program: " << student.program << " | Level: " << student.level;
    cout << " | GPA: " << fixed << setprecision(2) << gpa << "\n";
    cout << "---------------------------------------------\n";
}

// Calculates a student's GPA using credit-weighted grade points.
double calculateGpa(const string &studentId)
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
vector<const GradeRecord *> getStudentGrades(const string &studentId)
{
    vector<const GradeRecord *> found;
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
    string error;

    while (true)
    {
        student.name = readText("Enter Student Name: ");
        if (validName(student.name, error))
            break;
        cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        student.nationalId = readText("Enter National ID: ");
        if (!validNationalId(student.nationalId, error))
        {
            cout << "ERROR: " << error << "\n";
            continue;
        }
        if (findStudentByNationalId(student.nationalId) != -1)
        {
            cout << "ERROR: A student with this National ID already exists\n";
            continue;
        }
        break;
    }

    while (true)
    {
        string genderText = readText("Enter Gender (M/F): ");
        char gender = 'M';
        if (validGender(genderText, gender, error))
        {
            student.gender = gender;
            break;
        }
        cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        student.dob = readText("Enter Date of Birth (DD/MM/YYYY): ");
        if (validDob(student.dob, error))
            break;
        cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        student.phone = readText("Enter Phone Number: ");
        if (validPhone(student.phone, error))
            break;
        cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        string programText = readText("Enter Program (CSE/CCE/MCT): ");
        string program;
        if (validProgram(programText, program, error))
        {
            student.program = program;
            break;
        }
        cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        int level = readInt("Enter Academic Level (1-4): ");
        if (validLevel(level, error))
        {
            student.level = level;
            break;
        }
        cout << "ERROR: " << error << "\n";
    }

    student.id = generateStudentId();
    students.push_back(student);
    saveStudents();
    cout << "Student added successfully!\n";
    cout << "Student ID: " << student.id << "\n";
}

// Searches for students by ID, name, or national ID.
void searchStudent()
{
    while (true)
    {
        cout << "\n=== Search Student ===\n";
        cout << "1. Search by Student ID\n";
        cout << "2. Search by Name\n";
        cout << "3. Search by National ID\n";
        cout << "4. Back\n";

        int choice = readInt("Enter your choice: ");
        if (choice == 4)
            return;

        string error;
        vector<int> matches;

        if (choice == 1)
        {
            string id = toUpper(readText("Enter Student ID: "));
            if (!validStudentIdFormat(id, error))
            {
                cout << "Invalid Search Value: " << error << "\n";
                continue;
            }
            int index = findStudentById(id);
            if (index != -1)
                matches.push_back(index);
        }
        else if (choice == 2)
        {
            string name = readText("Enter student name to search: ");
            if (!validName(name, error))
            {
                cout << "Invalid Search Value: " << error << "\n";
                continue;
            }
            matches = findStudentsByName(name);
        }
        else if (choice == 3)
        {
            string nationalId = readText("Enter National ID: ");
            if (!validNationalId(nationalId, error))
            {
                cout << "Invalid Search Value: " << error << "\n";
                continue;
            }
            int index = findStudentByNationalId(nationalId);
            if (index != -1)
                matches.push_back(index);
        }

        if (matches.empty())
        {
            cout << "Student Not Found: No student found with the given criteria\n";
            continue;
        }

        cout << "Found " << matches.size() << " student(s):\n\n";
        for (int index : matches)
        {
            showStudentCard(students[index], calculateGpa(students[index].id));
            cout << "\n";
        }
    }
}

// Updates selected editable fields for an existing student.
void updateStudent()
{
    string error;
    string id = toUpper(readText("Enter Student ID to update: "));
    if (!validStudentIdFormat(id, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findStudentById(id);
    if (index == -1)
    {
        cout << "Student Not Found: No student exists with ID: " << id << "\n";
        return;
    }

    Student &student = students[index];
    cout << "Student found: " << student.name << "\n";
    cout << "1. Phone Number\n2. Program\n3. Academic Level\n4. Cancel\n";

    int choice = readInt("Enter your choice: ");
    if (choice == 4)
        return;

    if (choice == 1)
    {
        while (true)
        {
            string phone = readText("Enter new Phone Number: ");
            if (validPhone(phone, error))
            {
                student.phone = phone;
                break;
            }
            cout << "Invalid New Value: " << error << "\n";
        }
    }
    else if (choice == 2)
    {
        while (true)
        {
            string programText = readText("Enter new Program (CSE/CCE/MCT): ");
            string program;
            if (validProgram(programText, program, error))
            {
                student.program = program;
                break;
            }
            cout << "Invalid New Value: " << error << "\n";
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
            cout << "Invalid New Value: " << error << "\n";
        }
    }

    cout << "Operation Successful: Student record updated successfully\n";
}

// Deletes a student and removes linked grade records.
void deleteStudent()
{
    string error;
    string id = toUpper(readText("Enter Student ID to delete: "));
    if (!validStudentIdFormat(id, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findStudentById(id);
    if (index == -1)
    {
        cout << "ID Not Found: No student exists with ID: " << id << "\n";
        return;
    }

    cout << "Student found: " << students[index].name << "\n";
    string confirm = toUpper(trim(readText("Are you sure you want to delete this student? (Y/N): ")));
    if (confirm != "Y")
    {
        cout << "Operation Cancelled\n";
        return;
    }

    students.erase(students.begin() + index);
    grades.erase(remove_if(grades.begin(), grades.end(), [&](const GradeRecord &record)
                           { return record.studentId == id; }),
                 grades.end());
    saveAllData();

    cout << "Operation Successful: Student deleted successfully\n";
}

// Lists all students with selectable sort options.
void listStudents()
{
    saveStudents();
    if (students.empty())
    {
        cout << "No students available\n";
        return;
    }

    cout << "\n=== List All Students ===\n";
    cout << "1. Student ID\n2. Name (A-Z)\n3. GPA (Highest First)\n4. Back\n";
    int choice = readInt("Enter your choice: ");
    if (choice == 4)
        return;

    vector<int> order;
    for (size_t i = 0; i < students.size(); ++i)
    {
        order.push_back(static_cast<int>(i));
    }

    if (choice == 1)
    {
        sort(order.begin(), order.end(), [](int a, int b)
             { return students[a].id < students[b].id; });
    }
    else if (choice == 2)
    {
        sort(order.begin(), order.end(), [](int a, int b)
             { return toLower(trim(students[a].name)) < toLower(trim(students[b].name)); });
    }
    else if (choice == 3)
    {
        sort(order.begin(), order.end(), [](int a, int b)
             {
            double gpaA = calculateGpa(students[a].id);
            double gpaB = calculateGpa(students[b].id);
            if (gpaA == gpaB) return students[a].id < students[b].id;
            return gpaA > gpaB; });
    }

    cout << "Total Students: " << students.size() << "\n\n";
    cout << left << setw(10) << "ID" << setw(22) << "Name" << setw(10) << "Program" << setw(8) << "Level" << setw(8) << "GPA" << "\n";
    printLine(58);
    for (int index : order)
    {
        cout << left << setw(10) << students[index].id
             << setw(22) << students[index].name
             << setw(10) << students[index].program
             << setw(8) << students[index].level
             << setw(8) << fixed << setprecision(2) << calculateGpa(students[index].id) << "\n";
    }
}

// Adds a new course after validation and uniqueness checks.
void addCourse()
{
    Course course;
    string error;

    while (true)
    {
        string codeText = readText("Enter Course Code: ");
        string code;
        if (!validCourseCode(codeText, code, error))
        {
            cout << "ERROR: " << error << "\n";
            continue;
        }
        if (findCourseByCode(code) != -1)
        {
            cout << "ERROR: Course Code must be UNIQUE\n";
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
        cout << "ERROR: " << error << "\n";
    }

    while (true)
    {
        int credits = readInt("Enter Credit Hours (1-4): ");
        if (validCredits(credits, error))
        {
            course.credits = credits;
            break;
        }
        cout << "ERROR: " << error << "\n";
    }

    courses.push_back(course);
    saveCourses();
    cout << "Course added successfully!\n";
}

// Displays all courses in code order.
void viewCourses()
{
    if (courses.empty())
    {
        cout << "No courses available\n";
        return;
    }

    vector<Course> sortedCourses = courses;
    sort(sortedCourses.begin(), sortedCourses.end(), [](const Course &a, const Course &b)
         { return a.code < b.code; });

    cout << "\n=== All Courses ===\n";
    cout << left << setw(12) << "Code" << setw(35) << "Title" << setw(8) << "Credit" << "\n";
    printLine(55);
    for (const Course &course : sortedCourses)
    {
        cout << left << setw(12) << course.code << setw(35) << course.title << setw(8) << course.credits << "\n";
    }
}

// Updates either title or credit hours for a selected course.
void updateCourse()
{
    string error;
    string code;
    if (!validCourseCode(readText("Enter Course Code to update: "), code, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findCourseByCode(code);
    if (index == -1)
    {
        cout << "Course Not Found\n";
        return;
    }

    Course &course = courses[index];
    cout << "Course found: " << course.title << "\n";
    cout << "1. Update Course Title\n2. Update Credit Hours\n3. Cancel\n";
    int choice = readInt("Enter your choice: ");
    if (choice == 3)
        return;

    if (choice == 1)
    {
        while (true)
        {
            string title = readText("Enter new Course Title: ");
            if (validCourseTitle(title, error))
            {
                course.title = title;
                break;
            }
            cout << "ERROR: " << error << "\n";
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
            cout << "ERROR: " << error << "\n";
        }
    }

    cout << "Course updated successfully!\n";
}

// Deletes a course and removes linked grade records.
void deleteCourse()
{
    string error;
    string code;
    if (!validCourseCode(readText("Enter Course Code to delete: "), code, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }

    int index = findCourseByCode(code);
    if (index == -1)
    {
        cout << "Course Not Found\n";
        return;
    }

    string confirm = toUpper(trim(readText("Are you sure you want to delete this course? (Y/N): ")));
    if (confirm != "Y")
    {
        cout << "Operation cancelled\n";
        return;
    }

    courses.erase(courses.begin() + index);
    grades.erase(remove_if(grades.begin(), grades.end(), [&](const GradeRecord &record)
                           { return record.courseCode == code; }),
                 grades.end());
    saveAllData();

    cout << "Course deleted successfully!\n";
}

// Adds or updates one student's grade entry for a course.
void enterGrades()
{
    saveCourses();
    string error;
    string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }
    if (findStudentById(studentId) == -1)
    {
        cout << "ERROR: Student ID must exist in the system\n";
        return;
    }

    string courseCode;
    if (!validCourseCode(readText("Enter Course Code: "), courseCode, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }
    if (findCourseByCode(courseCode) == -1)
    {
        cout << "ERROR: Course Code must exist in the system\n";
        return;
    }

    int midterm = 0;
    while (true)
    {
        midterm = readInt("Enter Midterm Grade (0-40): ");
        if (validMidterm(midterm, error))
            break;
        cout << "ERROR: " << error << "\n";
    }

    int finalExam = 0;
    while (true)
    {
        finalExam = readInt("Enter Final Grade (0-60): ");
        if (validFinal(finalExam, error))
            break;
        cout << "ERROR: " << error << "\n";
    }

    const int total = midterm + finalExam;
    const string letter = calculateLetterGrade(total);
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
            cout << "Grade updated successfully!\n";
            cout << "Total: " << total << "/100\n";
            cout << "Letter Grade: " << letter << "\n";
            saveGrades();
            return;
        }
    }

    grades.push_back({studentId, courseCode, midterm, finalExam, total, letter, points});
    cout << "Grade recorded successfully!\n";
    cout << "Total: " << total << "/100\n";
    cout << "Letter Grade: " << letter << "\n";
    saveGrades();
}

// Displays all stored grades for one student.
void viewGrades()
{
    string error;
    string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }

    int studentIndex = findStudentById(studentId);
    if (studentIndex == -1)
    {
        cout << "ERROR: Student not found\n";
        return;
    }

    auto records = getStudentGrades(studentId);
    if (records.empty())
    {
        cout << "No grades recorded for this student yet\n";
        return;
    }

    cout << "\nGrades for " << students[studentIndex].name << " (" << studentId << ")\n";
    cout << left << setw(12) << "Course" << setw(30) << "Title" << setw(10) << "Midterm" << setw(8) << "Final" << setw(8) << "Total" << setw(8) << "Grade" << "\n";
    printLine(76);

    for (const GradeRecord *record : records)
    {
        string title = "N/A";
        int courseIndex = findCourseByCode(record->courseCode);
        if (courseIndex != -1)
        {
            title = courses[courseIndex].title;
        }
        cout << left << setw(12) << record->courseCode << setw(30) << title << setw(10) << record->midterm << setw(8) << record->finalExam << setw(8) << record->total << setw(8) << record->letter << "\n";
    }
}

// Shows GPA for one student.
void calculateGpaMenu()
{
    string error;
    string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }
    int studentIndex = findStudentById(studentId);
    if (studentIndex == -1)
    {
        cout << "ERROR: Student not found\n";
        return;
    }

    cout << fixed << setprecision(2);
    cout << "Cumulative GPA for " << students[studentIndex].name << " is: " << calculateGpa(studentId) << "\n";
}

// Prints a transcript-style view for one student.
void transcriptMenu()
{
    string error;
    string studentId = toUpper(readText("Enter Student ID: "));
    if (!validStudentIdFormat(studentId, error))
    {
        cout << "ERROR: " << error << "\n";
        return;
    }

    int studentIndex = findStudentById(studentId);
    if (studentIndex == -1)
    {
        cout << "ERROR: Student not found\n";
        return;
    }

    vector<const GradeRecord *> records = getStudentGrades(studentId);
    sort(records.begin(), records.end(), [](const GradeRecord *a, const GradeRecord *b)
         { return a->courseCode < b->courseCode; });

    cout << "\n=============================================\n";
    cout << "             STUDENT TRANSCRIPT\n";
    cout << "=============================================\n";
    cout << "Student ID: " << students[studentIndex].id << "\n";
    cout << "Name: " << students[studentIndex].name << "\n";
    cout << "Program: " << students[studentIndex].program << " | Level: " << students[studentIndex].level << "\n\n";

    cout << left << setw(12) << "Course" << setw(30) << "Title" << setw(8) << "Cred" << setw(8) << "Grade" << setw(8) << "Pts" << "\n";
    printLine(66);

    int totalCredits = 0;
    if (records.empty())
    {
        cout << "No grade records available\n";
    }
    else
    {
        for (const GradeRecord *record : records)
        {
            string title = "N/A";
            int credits = 0;
            int courseIndex = findCourseByCode(record->courseCode);
            if (courseIndex != -1)
            {
                title = courses[courseIndex].title;
                credits = courses[courseIndex].credits;
            }
            totalCredits += credits;
            cout << left << setw(12) << record->courseCode << setw(30) << title << setw(8) << credits << setw(8) << record->letter << setw(8) << fixed << setprecision(1) << record->points << "\n";
        }
    }

    cout << "\nTotal Credit Hours: " << totalCredits << "\n";
    cout << fixed << setprecision(2);
    cout << "Cumulative GPA: " << calculateGpa(studentId) << "\n";
}

// Runs the Student Management submenu loop.
void studentMenu()
{
    while (true)
    {
        cout << "\n=== Student Management ===\n";
        cout << "1. Add New Student\n2. Search Student\n3. Update Student\n4. Delete Student\n5. List All Students\n6. Back to Main Menu\n";
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
        cout << "\n=== Course Management ===\n";
        cout << "1. Add New Course\n2. View All Courses\n3. Update Course\n4. Delete Course\n5. Back to Main Menu\n";
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
        cout << "\n=== Grades Management ===\n";
        cout << "1. Enter Student Grades\n2. View Student Grades\n3. Calculate GPA\n4. Generate Transcript\n5. Back to Main Menu\n";
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
        cout << "\n======================================\n";
        cout << "       STUDENT INFORMATION SYSTEM\n";
        cout << "======================================\n";
        cout << "1. Student Management\n2. Course Management\n3. Grades Management\n4. Exit\n";

        int choice = readInt("Enter your choice: ");
        if (choice == 4)
        {
            cout << "Exiting system...\n";
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
