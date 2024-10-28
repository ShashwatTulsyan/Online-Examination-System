#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>

using namespace std;

// Question struct
struct Question {
    int id;
    string questionText;
    string answer; // Correct answer for MCQs
    bool isMCQ;    // true for MCQ, false for subjective
};

// Student struct
struct Student {
    string name;
    int rollNo;
    int mcqScore;
    int subjectiveScore;
    vector<string> subjectiveAnswers; // To hold answers to subjective questions
};

// Global variables
vector<Question> questionBank;  // Store questions
vector<Student> studentRecords;  // Store student records

// Function prototypes
void addQuestion();
void takeExam(Student &student);
void gradeSubjectiveAnswers();
void viewFinalScore(int rollNo);
void saveStudentRecords();
void loadStudentRecords();
void saveQuestions();
void loadQuestions();
void adminMode();
bool authenticateAdmin();
void studentMenu();
void viewFinalScoreMenu(); // New function for viewing final score

int main() {
    loadQuestions();      // Load questions from a file
    loadStudentRecords(); // Load previous student records if any

    int choice;
    do {
        cout << "1. Admin Mode\n2. Student Mode\n3. View Score\n4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                adminMode();
                return 0; // Close the program after exiting admin mode
            case 2:
                studentMenu();
                break;
            case 3:
                viewFinalScoreMenu(); // Call the function to view final score
                break;
            case 4:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 4);

    return 0;
}

// Admin mode for managing questions and grading
void adminMode() {
    if (!authenticateAdmin()) {
        cout << "Authentication failed. Exiting Admin Mode.\n";
        return;
    }

    int choice;
    do {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Question\n2. Grade Subjective Answers\n3. Exit Admin Mode\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addQuestion();
                break;
            case 2:
                gradeSubjectiveAnswers();
                break;
            case 3:
                cout << "Exiting Admin Mode.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 3);
}

// Function to add a question to the question bank
void addQuestion() {
    Question q;
    cout << "Enter Question ID: ";
    cin >> q.id;
    cin.ignore();  // Clear the newline character from the input buffer
    cout << "Enter Question Text: ";
    getline(cin, q.questionText);

    char type;
    cout << "Is this an MCQ or Subjective (m/s)? ";
    cin >> type;
    q.isMCQ = (type == 'm');

    if (q.isMCQ) {
        cout << "Enter Correct Answer: ";
        cin >> q.answer;
    } else {
        cout << "No answer required for subjective questions.\n";
    }

    questionBank.push_back(q);
    cout << "Question added successfully.\n";
    saveQuestions(); // Save questions after adding
}

// Function to save questions to a file
void saveQuestions() {
    ofstream file("questions.txt");
    if (file.is_open()) {
        for (const auto &q : questionBank) {
            file << q.id << "\n";
            file << q.questionText << "\n";
            file << (q.isMCQ ? "m" : "s") << "\n"; // m for MCQ, s for Subjective
            if (q.isMCQ) {
                file << q.answer << "\n"; // Save answer for MCQ
            }
        }
        file.close();
        cout << "Questions saved successfully.\n";
    } else {
        cout << "Unable to open file for saving questions.\n";
    }
}

// Function to load questions from a file
void loadQuestions() {
    ifstream file("questions.txt");
    if (file.is_open()) {
        Question q;
        string type;
        while (file >> q.id) {
            file.ignore(); // Ignore newline after reading ID
            getline(file, q.questionText);
            getline(file, type);
            q.isMCQ = (type == "m");

            if (q.isMCQ) {
                getline(file, q.answer); // Read answer for MCQ
            }
            questionBank.push_back(q);
        }
        file.close();
        cout << "Questions loaded successfully.\n";
    } else {
        cout << "No previous questions found.\n";
    }
}

// Function to authenticate admin
bool authenticateAdmin() {
    string password;
    cout << "Enter admin password: ";
    cin >> password;

    // Simple password check (replace with your password)
    return password == "admin123";  // Example password
}


// Function to grade subjective answers manually
void gradeSubjectiveAnswers() {
    for (auto &student : studentRecords) {
        int subjectiveScore = 0;
        cout << "\nGrading subjective answers for student " << student.name << " (Roll No: " << student.rollNo << ")\n";

        // Check if the student has any subjective answers
        if (student.subjectiveAnswers.empty()) {
            cout << "No subjective answers to grade for this student.\n";
            continue;  // Skip to the next student
        }

        // Display each subjective answer for grading
        for (size_t i = 0; i < student.subjectiveAnswers.size(); ++i) {
            cout << "Subjective Answer " << (i + 1) << ": " << student.subjectiveAnswers[i] << endl;

            int score = -1;  // Initialize score with invalid value
            while (score < 0 || score > 10) { // Validate score input
                cout << "Enter score for this answer (0-10): ";
                cin >> score;

                if (cin.fail() || score < 0 || score > 10) {
                    cin.clear(); // Clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                    cout << "Invalid score. Please enter a score between 0 and 10.\n";
                } else {
                    subjectiveScore += score; // Add valid score to total
                }
            }
        }

        // Store the total subjective score for the student
        student.subjectiveScore = subjectiveScore;
        cout << "Total Subjective Score for " << student.name << " (Roll No: " << student.rollNo << "): " << subjectiveScore << endl;
    }

    saveStudentRecords(); // Save student records after grading
}



// Function for students to take the exam
void takeExam(Student &student) {
    int mcqScore = 0;
    int totalQuestions = questionBank.size();

    if (totalQuestions == 0) {
        cout << "No questions available in the question bank.\n";
        return;
    }

    for (auto &q : questionBank) {
        string studentAnswer;
        cout << "\nQuestion ID " << q.id << ": " << q.questionText << endl;

        // For MCQs
        if (q.isMCQ) {
            cout << "(MCQ) Your answer: ";
            cin >> studentAnswer;  // Read the answer for MCQ
            cin.ignore();          // Clear the newline character from the input buffer

            // Check answer
            if (studentAnswer == q.answer) {
                cout << "Correct!\n";
                mcqScore += 10;  // Assuming each MCQ is worth 10 points
            } else {
                cout << "Incorrect. The correct answer is " << q.answer << endl;
            }
        } else {  // Subjective
            cout << "(Subjective) Your answer: ";
            cin.ignore(); // Ensure the buffer is cleared before getline
            getline(cin, studentAnswer);  // Read full line for subjective answer

            // Store the subjective answer in the student record
            student.subjectiveAnswers.push_back(studentAnswer);
            cout << "Answer recorded for grading.\n";
        }
    }

    // Store the score and record the student
    student.mcqScore = mcqScore;
    student.subjectiveScore = 0; // Initialize subjective score
    studentRecords.push_back(student);  // Add student to records
    saveStudentRecords();
    cout << "\nExam complete! Your MCQ score: " << mcqScore << " out of " << totalQuestions * 10 << endl;
}

// Function to view final score for a student by roll number
void viewFinalScore(int rollNo) {
    bool found = false;
    for (const auto &student : studentRecords) {
        if (student.rollNo == rollNo) {
            cout << "\nFinal Score for " << student.name << " (Roll No: " << student.rollNo << "):\n";
            cout << "MCQ Score: " << student.mcqScore << "\n";
            cout << "Subjective Score: " << student.subjectiveScore << "\n";
            cout << "Total Score: " << (student.mcqScore + student.subjectiveScore) << "\n";
            cout << "Subjective Answers: \n";
            for (const auto &answer : student.subjectiveAnswers) {
                cout << "- " << answer << endl;  // Display subjective answers
            }
            found = true;
            break; // Exit loop once found
        }
    }
    if (!found) {
        cout << "No record found for Roll No: " << rollNo << endl;
    }
}

// Function to save student records to a file
void saveStudentRecords() {
    ofstream file("student_records.txt");
    if (file.is_open()) {
        for (const auto &student : studentRecords) {
            file << student.name << "\n";
            file << student.rollNo << "\n";
            file << student.mcqScore << "\n";
            file << student.subjectiveScore << "\n";
            for (const auto &answer : student.subjectiveAnswers) {
                file << answer << "\n";  // Save each subjective answer
            }
            file << "END\n"; // Indicate end of answers for this student
        }
        file.close();
        cout << "Student records saved successfully.\n";
    } else {
        cout << "Unable to open file for saving student records.\n";
    }
}

// Function to load student records from a file
void loadStudentRecords() {
    ifstream file("student_records.txt");
    if (file.is_open()) {
        Student s;
        string line;
        while (getline(file, s.name)) {
            file >> s.rollNo;
            file >> s.mcqScore;
            file >> s.subjectiveScore;
            file.ignore(); // Ignore newline character

            // Load subjective answers
            while (getline(file, line) && line != "END") {
                s.subjectiveAnswers.push_back(line);
            }
            studentRecords.push_back(s);
        }
        file.close();
        cout << "Student records loaded successfully.\n";
    } else {
        cout << "No previous student records found.\n";
    }
}

// Function to display the student menu and take the exam
void studentMenu() {
    Student student;
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, student.name);
    cout << "Enter your Roll No: ";
    cin >> student.rollNo;

    takeExam(student);
}

// Function for students to view their final score
void viewFinalScoreMenu() {
    int rollNo;
    cout << "Enter your Roll No to view your final score: ";
    cin >> rollNo;
    viewFinalScore(rollNo);
}
