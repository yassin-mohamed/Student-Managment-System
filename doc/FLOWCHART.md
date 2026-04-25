# Student Information System Pseudocode Documentation

This document describes the runtime flow of the project in pseudocode form.

## 1) Notes

- Data files are loaded once at startup.
- Data is saved after add, update, and delete operations.
- Deleting a student or course also removes linked grade records.
- GPA is credit-weighted using grade points.

## 2) Pseudocode

### 4.1 Main Program

```text
BEGIN PROGRAM
    CALL LoadAllData
    LOOP forever (until user chooses Exit)
        DISPLAY Main Menu
        READ choice
        IF choice = 1 THEN
            CALL StudentMenu
        ELSE IF choice = 2 THEN
            CALL CourseMenu
        ELSE IF choice = 3 THEN
            CALL GradesMenu
        ELSE IF choice = 4 THEN
            CALL SaveAllData
            EXIT LOOP
        END IF
    END LOOP
END PROGRAM
```

### 4.2 Student Management

```text
PROCEDURE StudentMenu
    LOOP until user chooses Back
        DISPLAY Student Menu
        READ choice

        CASE choice OF
            1: CALL AddStudent
            2: CALL SearchStudent
            3: CALL UpdateStudent
            4: CALL DeleteStudent
            5: CALL ListStudents
            6: RETURN
        END CASE
    END LOOP
END PROCEDURE

PROCEDURE AddStudent
    REPEAT read name until ValidName
    REPEAT read nationalId until ValidNationalId AND nationalId is unique
    REPEAT read gender until ValidGender
    REPEAT read dob until ValidDob
    REPEAT read phone until ValidPhone
    REPEAT read program until ValidProgram
    REPEAT read level until ValidLevel

    student.id <- GenerateStudentId
    APPEND student to students
    CALL SaveStudents
    DISPLAY success message
END PROCEDURE

PROCEDURE SearchStudent
    LOOP until Back
        DISPLAY search options (ID / Name / National ID / Back)
        READ searchChoice

        IF searchChoice is Back THEN
            RETURN
        END IF

        FIND matching students based on selected criterion
        IF none found THEN
            DISPLAY not found message
        ELSE
            DISPLAY all matched student cards
        END IF
    END LOOP
END PROCEDURE

PROCEDURE UpdateStudent
    READ studentId
    VALIDATE studentId format
    FIND student by ID
    IF not found THEN
        DISPLAY error and RETURN
    END IF

    DISPLAY editable fields (phone, program, level)
    READ fieldChoice
    UPDATE selected field with validation loop
    CALL SaveStudents
    DISPLAY success message
END PROCEDURE

PROCEDURE DeleteStudent
    READ studentId
    VALIDATE studentId format
    FIND student by ID
    IF not found THEN
        DISPLAY error and RETURN
    END IF

    ASK confirmation
    IF confirmation is not Y THEN
        RETURN
    END IF

    REMOVE student from students
    REMOVE related grade rows from grades
    CALL SaveAllData
    DISPLAY success message
END PROCEDURE

PROCEDURE ListStudents
    IF students is empty THEN
        DISPLAY empty message
        RETURN
    END IF

    DISPLAY sort options (ID / Name / GPA / Back)
    READ sortChoice
    IF sortChoice is Back THEN RETURN

    CREATE index list for all students
    SORT index list based on selected option
    DISPLAY table using sorted index list
END PROCEDURE
```

### 4.3 Course Management

```text
PROCEDURE CourseMenu
    LOOP until user chooses Back
        DISPLAY Course Menu
        READ choice

        CASE choice OF
            1: CALL AddCourse
            2: CALL ViewCourses
            3: CALL UpdateCourse
            4: CALL DeleteCourse
            5: RETURN
        END CASE
    END LOOP
END PROCEDURE

PROCEDURE AddCourse
    REPEAT read course code until ValidCourseCode AND unique
    REPEAT read title until ValidCourseTitle
    REPEAT read credits until ValidCredits

    APPEND course to courses
    CALL SaveCourses
END PROCEDURE

PROCEDURE ViewCourses
    IF courses is empty THEN
        DISPLAY empty message
        RETURN
    END IF

    COPY courses to temp list
    SORT temp list by course code
    DISPLAY temp list
END PROCEDURE

PROCEDURE UpdateCourse
    READ course code
    VALIDATE code
    FIND course by code
    IF not found THEN
        DISPLAY error and RETURN
    END IF

    READ update option (title or credits)
    UPDATE selected field with validation loop
    CALL SaveCourses
END PROCEDURE

PROCEDURE DeleteCourse
    READ course code
    VALIDATE code
    FIND course by code
    IF not found THEN
        DISPLAY error and RETURN
    END IF

    ASK confirmation
    IF confirmation is not Y THEN
        RETURN
    END IF

    REMOVE course from courses
    REMOVE related grade rows from grades
    CALL SaveAllData
END PROCEDURE
```

### 4.4 Grades Management

```text
PROCEDURE GradesMenu
    LOOP until user chooses Back
        DISPLAY Grades Menu
        READ choice

        CASE choice OF
            1: CALL EnterGrades
            2: CALL ViewGrades
            3: CALL CalculateGpaMenu
            4: CALL TranscriptMenu
            5: RETURN
        END CASE
    END LOOP
END PROCEDURE

PROCEDURE EnterGrades
    READ studentId and validate format
    CHECK student exists

    READ courseCode and validate format
    CHECK course exists

    REPEAT read midterm until ValidMidterm
    REPEAT read final until ValidFinal

    total <- midterm + final
    letter <- CalculateLetterGrade(total)
    points <- LetterToPoints(letter)

    IF (studentId, courseCode) already exists in grades THEN
        UPDATE existing grade row
    ELSE
        APPEND new grade row
    END IF

    CALL SaveGrades
    DISPLAY result
END PROCEDURE

PROCEDURE ViewGrades
    READ studentId and validate
    FIND student
    GET all grade rows for that student
    DISPLAY grade table
END PROCEDURE

PROCEDURE CalculateGpaMenu
    READ studentId and validate
    FIND student
    gpa <- CalculateGpa(studentId)
    DISPLAY gpa
END PROCEDURE

PROCEDURE TranscriptMenu
    READ studentId and validate
    FIND student
    GET and sort student grade rows by course code
    DISPLAY transcript table, total credits, and cumulative GPA
END PROCEDURE
```

### 4.5 Persistence and Core Helpers

```text
PROCEDURE LoadAllData
    CALL LoadStudents
    CALL LoadCourses
    CALL LoadGrades
END PROCEDURE

PROCEDURE SaveAllData
    CALL SaveStudents
    CALL SaveCourses
    CALL SaveGrades
END PROCEDURE

FUNCTION CalculateGpa(studentId)
    totalPoints <- 0
    totalCredits <- 0

    FOR each grade row of studentId
        FIND matching course credits
        totalPoints <- totalPoints + (gradePoints * credits)
        totalCredits <- totalCredits + credits
    END FOR

    IF totalCredits = 0 THEN
        RETURN 0
    ELSE
        RETURN totalPoints / totalCredits
    END IF
END FUNCTION
```
