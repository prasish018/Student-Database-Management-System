#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

// Structure definitions
struct Student {
    int roll;
    char name[50];
    int age;
    char gender;
    char course[50];
    int semester;
    float gpa;
    char password[20];
    int attendance; // Number of days attended
};

struct Teacher {
    int id;
    char name[50];
    char department[50];
    char password[20];
};

struct Admin {
    char username[20];
    char password[20];
};

struct Assignment {
    int id;
    int teacherId;
    char title[100];
    char description[200];
    char startDate[20]; // Assignment start date
    char submissionDate[20]; // Assignment submission date
    int isDone[100]; // Array to track completion for each student
    int assignedStudents[100]; // Array to store rolls of assigned students
    int assignedCount; // Number of students the assignment is assigned to
};

// Global variables
struct Student students[100];
struct Teacher teachers[50];
struct Admin admin = {"admin", "hacker"};
struct Assignment assignments[100];
int studentCount = 0;

int teacherCount = 0;
int assignmentCount = 0;

// Function prototypes
void loadData();
void saveData();
void adminMenu();
void teacherMenu(int teacherId);
void studentMenu(int studentRoll);
void addStudent();
void viewStudents();
void editStudent();
void deleteStudent();
void addTeacher();
void viewTeachers();
void editTeacher();
void deleteTeacher();
void viewStudentDetails(int studentRoll);
void viewOwnDetails(int studentRoll);
void changeStudentPassword(int studentRoll);
void changeTeacherPassword(int teacherId);
void changeAdminPassword();
char *getPassword(char *password, int maxLength);
void signalHandler(int signal);
int isValidString(const char *str);
int isValidDate(int day, int month, int year);
void getDateInput(char *dateStr);

// Assignment related functions
void addAssignment(int teacherId);
void viewAssignmentsTeacher(int teacherId);
void viewAssignmentsStudent(int studentRoll);
void markAssignmentAsDone(int studentRoll);

// Attendance related functions
void recordStudentAttendance(int teacherId); // Modified to take teacherId

int main() {
    // Register signal handlers
    signal(SIGINT, signalHandler);  // Ctrl+C
    signal(SIGTERM, signalHandler); // Termination signal

    loadData();
    
    int choice;
    char username[20], password[20];
    int loggedIn = 0;
    int userId;
    int userRoll;
    char userType;
    
    while(1) {
        system("cls");
        printf("\n\t\tSTUDENT DATABASE MANAGEMENT SYSTEM\n");
        printf("\n\t\t1. Admin Login");
        printf("\n\t\t2. Teacher Login");
        printf("\n\t\t3. Student Login");
        printf("\n\t\t4. Exit");
        printf("\n\t\tEnter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                printf("\nEnter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                getPassword(password, sizeof(password));
                
                if(strcmp(username, admin.username) == 0 && strcmp(password, admin.password) == 0) {
                    adminMenu();
                } else {
                    printf("\nInvalid credentials! Press any key to continue...");
                    getch();
                }
                break;
                
            case 2:
                printf("\nEnter teacher ID: ");
                scanf("%d", &userId);
                printf("Enter password: ");
                getPassword(password, sizeof(password));
                
                loggedIn = 0;
                for(int i = 0; i < teacherCount; i++) {
                    if(teachers[i].id == userId && strcmp(teachers[i].password, password) == 0) {
                        loggedIn = 1;
                        break;
                    }
                }
                
                if(loggedIn) {
                    teacherMenu(userId);
                } else {
                    printf("\nInvalid credentials! Press any key to continue...");
                    getch();
                }
                break;
                
            case 3:
                printf("\nEnter student Roll: ");
                scanf("%d", &userRoll);
                printf("Enter password: ");
                getPassword(password, sizeof(password));
                
                loggedIn = 0;
                for(int i = 0; i < studentCount; i++) {
                    if(students[i].roll == userRoll && strcmp(students[i].password, password) == 0) {
                        loggedIn = 1;
                        break;
                    }
                }
                
                if(loggedIn) {
                    studentMenu(userRoll);
                } else {
                    printf("\nInvalid credentials! Press any key to continue...");
                    getch();
                }
                break;
                
            case 4:
                saveData();
                exit(0);
                
            default:
                printf("\nInvalid choice! Press any key to continue...");
                getch();
        }
    }
    
    return 0;
}

char *getPassword(char *password, int maxLength) {
    int i = 0;
    char ch;

    while (i < maxLength - 1) {
        ch = getch();
        if (ch == '\r') { // Enter key
            password[i] = '\0';
            printf("\n");
            return password;
        } else if (ch == '\b') { // Backspace
            if (i > 0) {
                i--;
                printf("\b \b"); // Erase the asterisk from the console
            }
        } else {
            password[i] = ch;
            i++;
            printf("*"); // Print an asterisk for each character
        }
    }

    password[maxLength - 1] = '\0';
    printf("\n");
    return password;
}

void loadData() {
    FILE *file;
    
    // Load students
    file = fopen("students.dat", "rb");
    if(file != NULL) {
        if (fread(&studentCount, sizeof(int), 1, file) != 1) {
            if (feof(file)) {
                studentCount = 0;
            } else {
                perror("Error reading student count from file");
                fclose(file);
                return;
            }
        }
        if (studentCount > 0) {
            if (fread(students, sizeof(struct Student), studentCount, file) != studentCount) {
                perror("Error reading students from file");
                studentCount = 0;
            }
        }
        fclose(file);
    } else {
        perror("Error opening students.dat for reading");
    }
    
    // Load teachers
    file = fopen("teachers.dat", "rb");
    if(file != NULL) {
        if (fread(&teacherCount, sizeof(int), 1, file) != 1) {
            if (feof(file)) {
                teacherCount = 0;
            } else {
                perror("Error reading teacher count from file");
                fclose(file);
                return;
            }
        }
        if (teacherCount > 0) {
            if (fread(teachers, sizeof(struct Teacher), teacherCount, file) != teacherCount) {
                perror("Error reading teachers from file");
                teacherCount = 0;
            }
        }
        fclose(file);
    } else {
        perror("Error opening teachers.dat for reading");
    }

     // Load assignments
    file = fopen("assignments.dat", "rb");
    if (file != NULL) {
        if (fread(&assignmentCount, sizeof(int), 1, file) != 1) {
            if (feof(file)) {
                assignmentCount = 0;
            } else {
                perror("Error reading assignment count from file");
                fclose(file);
                return;
            }
        }
        if (assignmentCount > 0) {
            if (fread(assignments, sizeof(struct Assignment), assignmentCount, file) != assignmentCount) {
                perror("Error reading assignments from file");
                assignmentCount = 0;
            }
        }
        fclose(file);
    } else {
        perror("Error opening assignments.dat for reading");
    }
}

void saveData() {
    FILE *file;
    
    // Save students
    file = fopen("students.dat", "wb");
    if(file != NULL) {
        if (fwrite(&studentCount, sizeof(int), 1, file) != 1) {
            perror("Error writing student count to file");
        } else if (studentCount > 0) {
            if (fwrite(students, sizeof(struct Student), studentCount, file) != studentCount) {
                perror("Error writing students to file");
            }
        }
        if (fclose(file) == EOF) {
            perror("Error closing students.dat");
        }
    } else {
        perror("Error opening students.dat for writing");
    }
    
    // Save teachers
    file = fopen("teachers.dat", "wb");
    if(file != NULL) {
        if (fwrite(&teacherCount, sizeof(int), 1, file) != 1) {
            perror("Error writing teacher count to file");
        } else if (teacherCount > 0) {
            if (fwrite(teachers, sizeof(struct Teacher), teacherCount, file) != teacherCount) {
                perror("Error writing teachers to file");
            }
        }
        if (fclose(file) == EOF) {
            perror("Error closing teachers.dat");
        }
    } else {
        perror("Error opening teachers.dat for writing");
    }

    // Save assignments
    file = fopen("assignments.dat", "wb");
    if (file != NULL) {
        if (fwrite(&assignmentCount, sizeof(int), 1, file) != 1) {
            perror("Error writing assignment count to file");
        } else if (assignmentCount > 0) {
            if (fwrite(assignments, sizeof(struct Assignment), assignmentCount, file) != assignmentCount) {
                perror("Error writing assignments to file");
            }
        }
        if (fclose(file) == EOF) {
            perror("Error closing assignments.dat");
        }
    } else {
        perror("Error opening assignments.dat for writing");
    }
}

void signalHandler(int signal) {
    printf("\nInterrupt signal (%d) received. Saving data...\n", signal);
    saveData();
    printf("Data saved. Exiting gracefully.\n");
    exit(signal);
}

void adminMenu() {
    int choice;
    
    while(1) {
        system("cls");
        printf("\n\t\tADMIN DASHBOARD\n");
        printf("\n\t\t1. Add Student");
        printf("\n\t\t2. View All Students");
        printf("\n\t\t3. Edit Student");
        printf("\n\t\t4. Delete Student");
        printf("\n\t\t5. Add Teacher");
        printf("\n\t\t6. View All Teachers");
        printf("\n\t\t7. Edit Teacher");
        printf("\n\t\t8. Delete Teacher");
        printf("\n\t\t9. Change Password");
        printf("\n\t\t10. Back to Main Menu");
        printf("\n\t\tEnter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                addStudent();
                break;
            case 2:
                viewStudents();
                break;
            case 3:
                editStudent();
                break;
            case 4:
                deleteStudent();
                break;
            case 5:
                addTeacher();
                break;
            case 6:
                viewTeachers();
                break;
            case 7:
                editTeacher();
                break;
            case 8:
                deleteTeacher();
                break;
            case 9:
                changeAdminPassword();
                break;
            case 10:
                return;
            default:
                printf("\nInvalid choice! Press any key to continue...");
                getch();
        }
    }
}

void teacherMenu(int teacherId) {
    int choice;
    int teacherIndex = -1;
    
    // Find teacher index
    for(int i = 0; i < teacherCount; i++) {
        if(teachers[i].id == teacherId) {
            teacherIndex = i;
            break;
        }
    }
    
    if(teacherIndex == -1) {
        printf("\nTeacher not found! Press any key to continue...");
        getch();
        return;
    }
    
    while(1) {
        system("cls");
        printf("\n\t\tTEACHER DASHBOARD\n");
        printf("\n\t\tLogged in as: %s", teachers[teacherIndex].name);
        printf("\n\t\t1. View All Students");
        printf("\n\t\t2. Search Student");
        printf("\n\t\t3. Change Password");
        printf("\n\t\t4. Add Assignment");
        printf("\n\t\t5. View Assignments");
        printf("\n\t\t6. Record Student Attendance"); 
        printf("\n\t\t7. Back to Main Menu");
        printf("\n\t\tEnter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                viewStudents();
                break;
            case 2:
                {
                    int studentRoll;
                    printf("\nEnter student Roll to view details: ");
                    scanf("%d", &studentRoll);
                    viewStudentDetails(studentRoll);
                }
                break;
            case 3:
                changeTeacherPassword(teacherId);
                break;
            case 4:
                addAssignment(teacherId);
                break;
            case 5:
                viewAssignmentsTeacher(teacherId);
                break;
            case 6:
                recordStudentAttendance(teacherId); //Call record student attendence
                break;
            case 7:
                return;
            default:
                printf("\nInvalid choice! Press any key to continue...");
                getch();
        }
    }
}

void studentMenu(int studentRoll) {
    int choice;
    int studentIndex = -1;
    
    // Find student index
    for(int i = 0; i < studentCount; i++) {
        if(students[i].roll == studentRoll) {
            studentIndex = i;
            break;
        }
    }
    
    if(studentIndex == -1) {
        printf("\nStudent not found! Press any key to continue...");
        getch();
        return;
    }
    
    while(1) {
        system("cls");
        printf("\n\t\tSTUDENT DASHBOARD\n");
        printf("\n\t\tLogged in as: %s", students[studentIndex].name);
        printf("\n\t\t1. View My Details");
        printf("\n\t\t2. Change Password");
        printf("\n\t\t3. View Assignments");
        printf("\n\t\t4. Mark Assignment as Done");
        printf("\n\t\t5. Back to Main Menu");
        printf("\n\t\tEnter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                viewOwnDetails(studentRoll);
                break;
            case 2:
                changeStudentPassword(studentRoll);
                break;
            case 3:
                viewAssignmentsStudent(studentRoll);
                break;
            case 4:
                 markAssignmentAsDone(studentRoll);
                 break;
            case 5:
                return;
            default:
                printf("\nInvalid choice! Press any key to continue...");
                getch();
        }
    }
}

int isValidString(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0; // Empty string is invalid
    }
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != ' ') {
            return 0; // Non-alphabetic character found
        }
    }
    return 1; // String is valid
}

// Function to validate date
int isValidDate(int day, int month, int year) {
    if (year < 1000 || year > 9999 || month < 1 || month > 12 || day < 1) {
        return 0;
    }
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) {
        daysInMonth[2] = 29; // Leap year
    }
    return day <= daysInMonth[month];
}

// Function to get date input with validation
void getDateInput(char *dateStr) {
    int day, month, year;
    do {
        printf("Enter date (DD-MM-YYYY): ");
        if (scanf("%d-%d-%d", &day, &month, &year) != 3) {
            printf("Invalid date format. Please use DD-MM-YYYY.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (!isValidDate(day, month, year)) {
            printf("Invalid date. Please enter a valid date.\n");
        } else {
            sprintf(dateStr, "%02d-%02d-%04d", day, month, year);
            break;
        }
    } while (1);
}


void addStudent() {
    system("cls");
    printf("\n\t\tADD NEW STUDENT\n");

    if(studentCount >= 100) {
        printf("\nDatabase full! Cannot add more students.");
        getch();
        return;
    }

    struct Student newStudent;
    int duplicate;

    do {
        printf("\nEnter student Roll: ");
        scanf("%d", &newStudent.roll);
        duplicate = 0;
        if (newStudent.roll <= 0) {
            printf("Invalid roll! Must be greater than 0.\n");
            duplicate = 1;
            continue;
        }
        for (int i = 0; i < studentCount; i++) {
            if (students[i].roll == newStudent.roll) {
                printf("Roll already exists! Enter a new roll number.\n");
                duplicate = 1;
                break;
            }
        }
    } while (duplicate);

  getchar(); 


do {
    printf("Enter student name : ");
    fgets(newStudent.name, sizeof(newStudent.name), stdin);
    newStudent.name[strcspn(newStudent.name, "\n")] = '\0'; // Remove newline
     if (!isValidString(newStudent.name)) {
            printf("Invalid name! Only alphabetic characters and spaces are allowed.\n");
        }
    else if (strlen(newStudent.name) < 3) {
        printf("Name too short! Please try again.\n");
    }
} while (!isValidString(newStudent.name) || strlen(newStudent.name) < 3);

    do {
        printf("Enter age: ");
        scanf("%d", &newStudent.age);
        if (newStudent.age <= 0 || newStudent.age > 120) {
            printf("Invalid age! Please enter a valid age between 1 and 120.\n");
        }
    } while (newStudent.age <= 0 || newStudent.age > 120);

    do {
        printf("Enter gender (M/F): ");
        scanf(" %c", &newStudent.gender);
        newStudent.gender = toupper(newStudent.gender);
        if (newStudent.gender != 'M' && newStudent.gender != 'F') {
            printf("Invalid gender! Please enter M or F.\n");
        }
    } while (newStudent.gender != 'M' && newStudent.gender != 'F');

    
    do {
        printf("Enter course: ");
        fgets(newStudent.course, sizeof(newStudent.course), stdin);
        newStudent.course[strcspn(newStudent.course, "\n")] = '\0';
        if (!isValidString(newStudent.course)) {
            printf("Invalid course! Only alphabetic characters and spaces are allowed.\n");
        }
    } while (!isValidString(newStudent.course));

    do {
        printf("Enter semester : ");
        scanf("%d", &newStudent.semester);
        if (newStudent.semester < 1 || newStudent.semester > 8) {
            printf("Invalid semester! Enter a value between 1 and 8.\n");
        }
    } while (newStudent.semester < 1 || newStudent.semester > 8);

    do {
        printf("Enter GPA : ");
        scanf("%f", &newStudent.gpa);
        if (newStudent.gpa < 0.0 || newStudent.gpa > 4.0) {
            printf("Invalid GPA! Please enter a value between 0.0 and 4.0.\n");
        }
    } while (newStudent.gpa < 0.0 || newStudent.gpa > 4.0);

    do {
        printf("Set password : ");
        char tempPassword[20];
        getPassword(tempPassword, sizeof(tempPassword));
        if (strlen(tempPassword) < 3) {
            printf("Password too short! Try again.\n");
        } else {
            strcpy(newStudent.password, tempPassword);
            break;
        }
    } while (1);

    newStudent.attendance = 0; // Initialize attendance to 0

    students[studentCount++] = newStudent;

    printf("\nStudent added successfully! Press any key to continue...");
    getch();
}


void viewStudents() {
    system("cls");
    printf("\n\t\tALL STUDENTS\n");
    
    if(studentCount == 0) {
        printf("\nNo students found in database.");
    } else {
        printf("\n%-10s %-20s %-5s %-8s %-20s %-5s %-5s %-10s", "Roll", "Name", "Age", "Gender", "Course", "Sem", "GPA", "Attendance");
        printf("\n--------------------------------------------------------------------------------------------------");
        
        for(int i = 0; i < studentCount; i++) {
            printf("\n%-10d %-20s %-5d %-8c %-20s %-5d %-5.2f %-10d", 
                   students[i].roll, students[i].name, students[i].age,
                   students[i].gender, students[i].course, students[i].semester, students[i].gpa, students[i].attendance);
        }
    }
    
    printf("\n\nPress any key to continue..."); 
    getch();
}
void editStudent() {
    system("cls");
    printf("\n\t\tEDIT STUDENT\n");
    
    if (studentCount == 0) {
        printf("\nNo students found in database.");
        getch();
        return;
    }

    int roll, found = 0, index;
    printf("\nEnter student Roll to edit: ");
    scanf("%d", &roll);
    getchar(); // Clear newline from input buffer

    for (int i = 0; i < studentCount; i++) {
        if (students[i].roll == roll) {
            found = 1;
            index = i;
            break;
        }
    }

    if (!found) {
        printf("\nStudent not found!");
        getch();
        return;
    }

    printf("\nCurrent details:");
    printf("\nRoll: %d", students[index].roll);
    printf("\nName: %s", students[index].name);
    printf("\nAge: %d", students[index].age);
    printf("\nGender: %c", students[index].gender);
    printf("\nCourse: %s", students[index].course);
    printf("\nSemester: %d", students[index].semester);
    printf("\nGPA: %.2f", students[index].gpa);

    printf("\n\nEnter new details:");

    char temp[50];

    // Name
     do {
        printf("\nEnter name (leave blank to keep current): ");
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
         if (strlen(temp) > 0) {
            if (!isValidString(temp)) {
                printf("Invalid name! Only alphabetic characters and spaces are allowed.\n");
            }
            else if (strlen(temp) < 3) {
                printf("Name too short! Please enter a name with at least 3 characters.\n");
            } else {
                strcpy(students[index].name, temp);
                break;
            }
        } else {
            break;
        }
    } while (!isValidString(temp) || strlen(temp) < 3);

    // Age
    int tempAge;
    printf("Enter age (0 to keep current): ");
    scanf("%d", &tempAge);
    getchar();
    if (tempAge > 0) {
         if (tempAge < 1 || tempAge > 120) {
             printf("Invalid age! Please enter a valid age between 1 and 120.\n");
         } else {
              students[index].age = tempAge;
         }

    }

    // Gender
    printf("Enter gender (M/F, leave blank to keep current): ");
    char line[10];
    fgets(line, sizeof(line), stdin);
    line[strcspn(line, "\n")] = '\0';
    if (strlen(line) == 1 && (toupper(line[0]) == 'M' || toupper(line[0]) == 'F')) {
        students[index].gender = toupper(line[0]);
    }

    // Course
    do {
        printf("Enter course (leave blank to keep current): ");
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
        if (strlen(temp) > 0) {
            if (!isValidString(temp)) {
                printf("Invalid course! Only alphabetic characters and spaces are allowed.\n");
            } else {
                strcpy(students[index].course, temp);
                break;
            }
        } else {
            break;
        }
    } while (!isValidString(temp));

    // Semester
    int tempSemester;
    printf("Enter semester (0 to keep current): ");
    scanf("%d", &tempSemester);
    getchar();
    if (tempSemester > 0) {
         if (tempSemester < 1 || tempSemester > 8) {
             printf("Invalid semester! Please enter a valid value between 1 and 8.\n");
         } else {
              students[index].semester = tempSemester;
         }

    }

    // GPA
    float tempGpa;
    printf("Enter GPA (0 to keep current): ");
    scanf("%f", &tempGpa);
    getchar();
    if (tempGpa > 0.0f  && tempGpa<=4.0f) {
        students[index].gpa = tempGpa;
    }

    printf("\nStudent details updated successfully! Press any key to continue...");
    getch();
}


void deleteStudent() {
    system("cls");
    printf("\n\t\tDELETE STUDENT\n");
    
    if(studentCount == 0) {
        printf("\nNo students found in database.");
        getch();
        return;
    }
    
    int roll, found = 0, index;
    printf("\nEnter student Roll to delete: ");
    scanf("%d", &roll);
    
    for(int i = 0; i < studentCount; i++) {
        if(students[i].roll == roll) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nStudent not found!");
        getch();
        return;
    }
    
    printf("\nAre you sure you want to delete this student? (Y/N): ");
    char confirm;
    scanf(" %c", &confirm);
    
    if(toupper(confirm) == 'Y') {
        for(int i = index; i < studentCount - 1; i++) {
            students[i] = students[i + 1];
        }
        studentCount--;
        printf("\nStudent deleted successfully!");
    } else {
        printf("\nDeletion cancelled!");
    }
    
    printf("\nPress any key to continue...");
    getch();
}

void addTeacher() {
    system("cls");
    printf("\n\t\tADD NEW TEACHER\n");
    
    if(teacherCount >= 50) {
        printf("\nDatabase full! Cannot add more teachers.");
        getch();
        return;
    }
    
    struct Teacher newTeacher;
    
    printf("\nEnter teacher ID: ");
    scanf("%d", &newTeacher.id);
    
    // Check if ID already exists
    for(int i = 0; i < teacherCount; i++) {
        if(teachers[i].id == newTeacher.id) {
            printf("\nTeacher with this ID already exists!");
            getch();
            return;
        }
    }
   getchar(); // Clear input buffer after scanf

    // Teacher name input with validation
     do {
        printf("Enter teacher name : ");
        fgets(newTeacher.name, sizeof(newTeacher.name), stdin);
        newTeacher.name[strcspn(newTeacher.name, "\n")] = '\0'; // Remove newline
        if (!isValidString(newTeacher.name)) {
            printf("Invalid name! Only alphabetic characters and spaces are allowed.\n");
        }
        else if (strlen(newTeacher.name) < 3) {
            printf("Name too short! Please try again.\n");
        }
    } while (!isValidString(newTeacher.name) || strlen(newTeacher.name) < 3);
    
     do {
        printf("Enter department: ");
        fgets(newTeacher.department, sizeof(newTeacher.department), stdin);
        newTeacher.department[strcspn(newTeacher.department, "\n")] = '\0';
        if (!isValidString(newTeacher.department)) {
            printf("Invalid department! Only alphabetic characters and spaces are allowed.\n");
        }
    } while (!isValidString(newTeacher.department));
    do {
        printf("Set password : ");
        char tempPassword[20];
        getPassword(tempPassword, sizeof(tempPassword));
        if (strlen(tempPassword) < 3) {
            printf("Password too short! Please try again.\n");
        } else {
            strcpy(newTeacher.password, tempPassword);
            break;
        }
    } while (1);

    
    teachers[teacherCount++] = newTeacher;
    
    printf("\nTeacher added successfully! Press any key to continue...");
    getch();
}

void viewTeachers() {
    system("cls");
    printf("\n\t\tALL TEACHERS\n");
    
    if(teacherCount == 0) {
        printf("\nNo teachers found in database.");
    } else {
        printf("\n%-10s %-20s %-20s", "ID", "Name", "Department");
        printf("\n------------------------------------------------------------------");
        
        for(int i = 0; i < teacherCount; i++) {
            printf("\n%-10d %-20s %-20s", 
                   teachers[i].id, teachers[i].name, teachers[i].department);
        }
    }
    
    printf("\n\nPress any key to continue...");
    getch();
}
void editTeacher() {
    system("cls");
    printf("\n\t\tEDIT TEACHER\n");
    
    if(teacherCount == 0) {
        printf("\nNo teachers found in database.");
        getch();
        return;
    }
    
    int id, found = 0, index;
    printf("\nEnter teacher ID to edit: ");
    scanf("%d", &id);
    getchar();  // Consume leftover newline after scanf
    
    for(int i = 0; i < teacherCount; i++) {
        if(teachers[i].id == id) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nTeacher not found!");
        getch();
        return;
    }
    
    printf("\nCurrent details:");
    printf("\nID: %d", teachers[index].id);
    printf("\nName: %s", teachers[index].name);
    printf("\nDepartment: %s", teachers[index].department);
    
    char temp[50];
    printf("\n\nEnter new details:");
    
     do {
        printf("\nEnter name (leave blank to keep current): ");
        fgets (temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
         if (strlen(temp) > 0) {
            if (!isValidString(temp)) {
                printf("Invalid name! Only alphabetic characters and spaces are allowed.\n");
            }
            else if (strlen(temp) < 3) {
                printf("Name too short! Please enter a name with at least 3 characters.\n");
            } else {
                strcpy(teachers[index].name, temp);
                break;
            }
        } else {
            break;
        }
    } while (!isValidString(temp) || strlen(temp) < 3);
    
     do {
        printf("Enter department (leave blank to keep current): ");
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';
        if (strlen(temp) > 0) {
            if (!isValidString(temp)) {
                printf("Invalid department! Only alphabetic characters and spaces are allowed.\n");
            } else {
                strcpy(teachers[index].department, temp);
                break;
            }
        } else {
            break;
        }
    } while (!isValidString(temp));
    
    printf("\nTeacher details updated successfully! Press any key to continue...");
    getch();
}



void deleteTeacher() {
    system("cls");
    printf("\n\t\tDELETE TEACHER\n");
    
    if(teacherCount == 0) {
        printf("\nNo teachers found in database.");
        getch();
        return;
    }
    
    int id, found = 0, index;
    printf("\nEnter teacher ID to delete: ");
    scanf("%d", &id);
    
    for(int i = 0; i < teacherCount; i++) {
        if(teachers[i].id == id) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nTeacher not found!");
        getch();
        return;
    }
    
    printf("\nAre you sure you want to delete this teacher? (Y/N): ");
    char confirm;
    scanf(" %c", &confirm);
    
    if(toupper(confirm) == 'Y') {
        for(int i = index; i < teacherCount - 1; i++) {
            teachers[i] = teachers[i + 1];
        }
        teacherCount--;
        printf("\nTeacher deleted successfully!");
        } else {
        printf("\nDeletion cancelled!");
    }
    
    printf("\nPress any key to continue...");
    getch();
}

void viewStudentDetails(int studentRoll) {
    system("cls");
    printf("\n\t\tSTUDENT DETAILS\n");
    
    int found = 0, index;
    
    for(int i = 0; i < studentCount; i++) {
        if(students[i].roll == studentRoll) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nStudent not found!");
        getch();
        return;
    }
    
    printf("\nRoll: %d", students[index].roll);
    printf("\nName: %s", students[index].name);
    printf("\nAge: %d", students[index].age);
    printf("\nGender: %c", students[index].gender);
    printf("\nCourse: %s", students[index].course);
    printf("\nSemester: %d", students[index].semester);
    printf("\nGPA: %.2f", students[index].gpa);
    
    printf("\n\nPress any key to continue...");
    getch();
}

void viewOwnDetails(int studentRoll) {
    system("cls");
    printf("\n\t\tMY DETAILS\n");
    
    int found = 0, index;
    
    for(int i = 0; i < studentCount; i++) {
        if(students[i].roll == studentRoll) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nStudent not found!");
        getch();
        return;
    }
    
    printf("\nRoll: %d", students[index].roll);
    printf("\nName: %s", students[index].name);
    printf("\nAge: %d", students[index].age);
    printf("\nGender: %c", students[index].gender);
    printf("\nCourse: %s", students[index].course);
    printf("\nSemester: %d", students[index].semester);
    printf("\nGPA: %.2f", students[index].gpa);
    printf("\nAttendance: %d days", students[index].attendance); // Display attendance

    printf("\n\nPress any key to continue...");
    getch();
}

void changeStudentPassword(int studentRoll) {
    system("cls");
    printf("\n\t\tCHANGE PASSWORD\n");
    
    int found = 0, index;
    
    for(int i = 0; i < studentCount; i++) {
        if(students[i].roll == studentRoll) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nStudent not found!");
        getch();
        return;
    }
    
    char current[20], newPass[20], confirm[20];
    
    printf("\nEnter current password: ");
    getPassword(current, sizeof(current));
    
    if(strcmp(current, students[index].password) != 0) {
        printf("\nIncorrect current password!");
        getch();
        return;
    }
    
    char tempPassword[20];
    do {
        printf("Enter new password: ");
        getPassword(tempPassword, sizeof(tempPassword));
         if (strlen(tempPassword) < 3) {
            printf("Password too short! Please try again.\n");
        }
        else {
            strcpy(newPass, tempPassword);
            break;
        }
    } while (strlen(tempPassword) < 3);

    do {
        printf("Confirm new password: ");
        getPassword(confirm, sizeof(confirm));
        if (strcmp(newPass, confirm) != 0) {
            printf("\nPasswords don't match!");
        } else {
            break;
        }
    } while (strcmp(newPass, confirm) != 0);
    
    strcpy(students[index].password, newPass);
    printf("\nPassword changed successfully! Press any key to continue...");
    getch();
}

void changeTeacherPassword(int teacherId) {
    system("cls");
    printf("\n\t\tCHANGE PASSWORD\n");
    
    int found = 0, index;
    
    for(int i = 0; i < teacherCount; i++) {
        if(teachers[i].id == teacherId) {
            found = 1;
            index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nTeacher not found!");
        getch();
        return;
    }
    
    char current[20], newPass[20], confirm[20];
    
    printf("\nEnter current password: ");
    getPassword(current, sizeof(current));
    
    if(strcmp(current, teachers[index].password) != 0) {
        printf("\nIncorrect current password!");
        getch();
        return;
    }
    
    char tempPassword[20];
     do {
        printf("Enter new password: ");
        getPassword(tempPassword, sizeof(tempPassword));
         if (strlen(tempPassword) < 3) {
            printf("Password too short! Please try again.\n");
        }
        else {
            strcpy(newPass, tempPassword);
            break;
        }
    } while (strlen(tempPassword) < 3);

    do {
        printf("Confirm new password: ");
        getPassword(confirm, sizeof(confirm));
        if (strcmp(newPass, confirm) != 0) {
            printf("\nPasswords don't match!");
        } else {
            break;
        }
    } while (strcmp(newPass, confirm) != 0);
    
    strcpy(teachers[index].password, newPass);
    printf("\nPassword changed successfully! Press any key to continue...");
    getch();
}

void changeAdminPassword() {
    system("cls");
    printf("\n\t\tCHANGE ADMIN PASSWORD\n");

    char current[20], newPass[20], confirm[20];

    printf("\nEnter current password: ");
    getPassword(current, sizeof(current));

    if (strcmp(current, admin.password) != 0) {
        printf("\nIncorrect current password!");
        getch();
        return;
    }
     char tempPassword[20];
    do {
        printf("Enter new password: ");
        getPassword(tempPassword, sizeof(tempPassword));
         if (strlen(tempPassword) < 3) {
            printf("Password too short! Please try again.\n");
        }
        else {
            strcpy(newPass, tempPassword);
            break;
        }
    } while (strlen(tempPassword) < 3);

    do {
        printf("Confirm new password: ");
        getPassword(confirm, sizeof(confirm));
        if (strcmp(newPass, confirm) != 0) {
            printf("\nPasswords don't match!");
        } else {
            break;
        }
    } while (strcmp(newPass, confirm) != 0);

    strcpy(admin.password, newPass);
    printf("\nAdmin password changed successfully! Press any key to continue...");
    getch();
}
void addAssignment(int teacherId) {
    system("cls");
    printf("\n\t\tADD NEW ASSIGNMENT\n");

    if (assignmentCount >= 100) {
        printf("\nAssignment database is full!");
        getch();
        return;
    }

    struct Assignment newAssignment;

    newAssignment.id = assignmentCount + 1; // Simple auto-increment
    newAssignment.teacherId = teacherId;
    newAssignment.assignedCount = 0; // Initialize assigned student count

    printf("Enter assignment title: ");
    scanf(" %[^\n]", newAssignment.title);
    getchar();

    printf("Enter assignment description: ");
    scanf(" %[^\n]", newAssignment.description);
    getchar();

    printf("Enter assignment start date:\n");
    getDateInput(newAssignment.startDate);

    printf("Enter assignment submission date:\n");
    getDateInput(newAssignment.submissionDate);

    // Assign to all students
    for (int i = 0; i < studentCount; i++) {
        newAssignment.assignedStudents[i] = students[i].roll;
        newAssignment.isDone[i] = 0; // Initialize as not done for each student
        newAssignment.assignedCount++;
    }

    assignments[assignmentCount++] = newAssignment;

    printf("\nAssignment added successfully and assigned to all students! Press any key to continue...");
    getch();
}


void viewAssignmentsTeacher(int teacherId) {
    system("cls");
    printf("\n\t\tTEACHER ASSIGNMENTS\n");

    int found = 0;

    printf("\n%-5s %-10s %-30s %-40s %-12s %-12s", "ID", "Teacher ID", "Title", "Description", "Start Date", "Submission Date");
    printf("\n------------------------------------------------------------------------------------------------------------------");

    for (int i = 0; i < assignmentCount; i++) {
        if (assignments[i].teacherId == teacherId) {
            printf("\n%-5d %-10d %-30s %-40s %-12s %-12s",
                   assignments[i].id, assignments[i].teacherId, assignments[i].title,
                   assignments[i].description, assignments[i].startDate, assignments[i].submissionDate);

            // List assigned students and their completion status
            printf("\n  Assigned to Students:");
            for (int j = 0; j < assignments[i].assignedCount; j++) {
                printf("\n    Roll: %-5d  Done: %s",
                       assignments[i].assignedStudents[j],
                       assignments[i].isDone[j] ? "Yes" : "No");
            }
            found = 1;
        }
    }

    if (!found) {
        printf("\nNo assignments found for this teacher.");
    }

    printf("\n\nPress any key to continue...");
    getch();
}


void viewAssignmentsStudent(int studentRoll) {
    system("cls");
    printf("\n\t\tSTUDENT ASSIGNMENTS\n");

    int found = 0;

    printf("\n%-5s %-10s %-30s %-40s %-12s %-12s %-8s", "ID", "Teacher ID", "Title", "Description", "Start Date", "Submission Date", "Done?");
    printf("\n------------------------------------------------------------------------------------------------------------------");

    for (int i = 0; i < assignmentCount; i++) {
        // Check if the assignment is assigned to this student
        for (int j = 0; j < assignments[i].assignedCount; j++) {
            if (assignments[i].assignedStudents[j] == studentRoll) {
                printf("\n%-5d %-10d %-30s %-40s %-12s %-12s %-8s",
                       assignments[i].id, assignments[i].teacherId, assignments[i].title,
                       assignments[i].description, assignments[i].startDate, assignments[i].submissionDate,
                       assignments[i].isDone[j] ? "Yes" : "No");
                found = 1;
                break; // Assignment found, no need to check other students for this assignment
            }
        }
    }

    if (!found) {
        printf("\nNo assignments found for this student.");
    }

    printf("\n\nPress any key to continue...");
    getch();
}


void markAssignmentAsDone(int studentRoll) {
    system("cls");
    printf("\n\t\tMARK ASSIGNMENT AS DONE\n");

    int assignmentId, found = 0;

    printf("Enter the Assignment ID to mark as done: ");
    scanf("%d", &assignmentId);

    for (int i = 0; i < assignmentCount; i++) {
        // Find the assignment
        if (assignments[i].id == assignmentId) {
            // Check if the assignment is assigned to this student
            for (int j = 0; j < assignments[i].assignedCount; j++) {
                if (assignments[i].assignedStudents[j] == studentRoll) {
                    assignments[i].isDone[j] = 1; // Mark as done
                    found = 1;
                    printf("\nAssignment marked as done!");
                    break; // No need to check other students for this assignment
                }
            }
            break; // Assignment found, no need to check other assignments
        }
    }

    if (!found) {
        printf("\nAssignment not found or not assigned to this student!");
    }

    printf("\n\nPress any key to continue...");
    getch();
}

void recordStudentAttendance(int teacherId) {
    system("cls");
    printf("\n\t\tRECORD STUDENT ATTENDANCE\n");

    int studentRoll, found = 0, index;

    printf("Enter student Roll to mark attendance: ");
    scanf("%d", &studentRoll);

    for (int i = 0; i < studentCount; i++) {
        if (students[i].roll == studentRoll) {
            found = 1;
            index = i;
            break;
        }
    }

    if (!found) {
        printf("\nStudent not found!");
        getch();
        return;
    }

    // Get current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Today is: %d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    printf("\nMark student %s (Roll %d) as present? (Y/N): ", students[index].name, studentRoll);
    char confirm;
    scanf(" %c", &confirm);

    if (toupper(confirm) == 'Y') {
        students[index].attendance++;
        printf("\nAttendance recorded for today!");
    } else {
        printf("\nAttendance not recorded.");
    }

    printf("\n\nPress any key to continue...");
    getch();
}

