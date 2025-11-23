#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>
#include <algorithm>
#include <fstream>

using namespace std;

unordered_map<string, int> cheatingScores;

class Student {
public:
    string id;
    string name;
    string faceData;
    string aadhar;
    bool isAuthenticated;
    bool isTakingExam;

    Student() : isAuthenticated(false), isTakingExam(false) {}
    Student(string _id, string _name, string _faceData, string _aadhar = "")
        : id(_id), name(_name), faceData(_faceData), aadhar(_aadhar),
          isAuthenticated(false), isTakingExam(false) {}
};

struct ExamNode {
    string examId;
    string subject;
    time_t examTime;
    int duration;
    ExamNode* left;
    ExamNode* right;

    ExamNode(string id, string sub, time_t time, int dur)
        : examId(id), subject(sub), examTime(time), duration(dur), left(nullptr), right(nullptr) {}
};

class SmartExamSystem {
private:

    unordered_map<string, Student> students;

    ExamNode* examRoot;

    struct LogNode {
        string studentId;
        string activity;
        time_t timestamp;
        LogNode* next;

        LogNode(string id, string act, time_t time)
            : studentId(id), activity(act), timestamp(time), next(nullptr) {}
    };
    LogNode* logHead;

    struct MonitoringData {
        double audioLevel;
        int movementCount;
        bool isTalking;
        bool isUsingPhone;
        time_t lastChecked;

        MonitoringData() : audioLevel(0.0), movementCount(0), isTalking(false), isUsingPhone(false), lastChecked(time(nullptr)) {}
    };
    unordered_map<string, MonitoringData> monitoringData;

    ExamNode* insertExamBST(ExamNode* node, string id, string subject, time_t time, int duration) {
        if (!node) return new ExamNode(id, subject, time, duration);

        if (time < node->examTime)
            node->left = insertExamBST(node->left, id, subject, time, duration);
        else
            node->right = insertExamBST(node->right, id, subject, time, duration);

        return node;
    }

    ExamNode* searchExamBST(ExamNode* node, time_t time) {
        if (!node || node->examTime == time) return node;

        if (time < node->examTime)
            return searchExamBST(node->left, time);
        return searchExamBST(node->right, time);
    }

    void showInstructions() {
        cout << "\n[INFO] Exam Instructions:\n";
        cout << "- Keep your camera and microphone ON during the exam.\n";
        cout << "- Do not switch browser tabs or applications.\n";
        cout << "- Sit in a well-lit room with your face visible.\n";
        cout << "- Avoid background noise, talking, or using electronic devices.\n";
    }

    void cameraMicCheck(const string& studentId) {
        cout << "\n[CHECK] Camera and Mic Check for Student " << studentId << ":\n";
        cout << "- Camera: Detected (simulated)\n";
        cout << "- Microphone: Working (simulated)\n";
        addLog(studentId, "Camera and Mic check completed");
    }

    void faceDetectionStatus(const string& studentId) {
        cout << "\n[STATUS] Face Detection for Student " << studentId << ":\n";
        if (students.find(studentId) != students.end() && students[studentId].isAuthenticated) {
            cout << "- Face Detected (simulated)\n";
            addLog(studentId, "Face detection successful");
        } else {
            cout << "- Face Not Detected or Student Not Authenticated\n";
            addLog(studentId, "Face detection failed");
        }
    }

public:
    SmartExamSystem() : examRoot(nullptr), logHead(nullptr) {}

    void registerStudent(string id, string name, string faceData, string aadhar) {
        students.emplace(id, Student(id, name, faceData, aadhar));
        cheatingScores[id] = 0;
        cout << "[OK] Student " << name << " registered successfully!\n";
    }

    bool authenticateStudent(string id, string faceData) {
        if (students.find(id) != students.end()) {
            if (students[id].faceData == faceData) {
                students[id].isAuthenticated = true;
                addLog(id, "Authenticated successfully");
                cout << "[OK] Student " << id << " authenticated successfully!\n";
                return true;
            }
        }
        addLog(id, "Authentication failed");
        cout << "[ERROR] Authentication failed for student " << id << "!\n";
        return false;
    }

    void verifyStudentIdentity(string id, string name, string aadhar) {
        cout << "\n[ID CHECK] Student Identity Verification for " << id << ":\n";
        if (aadhar.length() != 12 || !all_of(aadhar.begin(), aadhar.end(), ::isdigit)) {
            cout << "[ERROR] Invalid Aadhar Number. Must be 12 digits.\n";
            addLog(id, "Identity verification failed: Invalid Aadhar");
            return;
        }

        if (students.find(id) != students.end()) {
            students[id].name = name;
            students[id].aadhar = aadhar;
            cout << "\n[OK] Identity Verified!\n";
            cout << "Name: " << name << "\n";
            cout << "Student ID: " << id << "\n";
            cout << "Aadhar: " << aadhar << "\n";

            ofstream file("verified_students.txt", ios::app);
            if (file.is_open()) {
                file << "Name: " << name << "\n";
                file << "Student ID: " << id << "\n";
                file << "Aadhar: " << aadhar << "\n";
                file << "-----------------------------\n";
                file.close();
                cout << "[OK] Student info saved to verified_students.txt\n";
                addLog(id, "Identity verified and saved");
            } else {
                cout << "[ERROR] Could not write to verified_students.txt\n";
                addLog(id, "Failed to save identity verification");
            }
        } else {
            cout << "[ERROR] Student " << id << " not found!\n";
            addLog(id, "Identity verification failed: Student not found");
        }
    }

    void startExam(string studentId, string examId) {
        if (students.find(studentId) != students.end() && students[studentId].isAuthenticated) {
            if (!students[studentId].aadhar.empty()) {
                students[studentId].isTakingExam = true;
                monitoringData[studentId] = MonitoringData();
                addLog(studentId, "Started exam " + examId);
                cout << "[OK] Exam " << examId << " started for student " << studentId << "\n";
            } else {
                cout << "[ERROR] Cannot start exam: Student " << studentId << " identity not verified!\n";
                addLog(studentId, "Exam start failed: Identity not verified");
            }
        } else {
            cout << "[ERROR] Cannot start exam: Student " << studentId << " not authenticated!\n";
            addLog(studentId, "Exam start failed: Not authenticated");
        }
    }

    void scheduleExam(string id, string subject, time_t time, int duration) {
        examRoot = insertExamBST(examRoot, id, subject, time, duration);
        cout << "[OK] Exam " << subject << " scheduled successfully!\n";
    }

    bool isExamScheduled(time_t time) {
        return searchExamBST(examRoot, time) != nullptr;
    }

    void addLog(string studentId, string activity) {
        LogNode* newLog = new LogNode(studentId, activity, time(nullptr));
        newLog->next = logHead;
        logHead = newLog;
    }

    void reportSuspiciousBehavior(string studentId, string report) {
        if (students.find(studentId) != students.end()) {
            time_t now = time(nullptr);
            char timeStr[26];
            ctime_r(&now, timeStr);
            timeStr[24] = '\0';

            ofstream file("reports.txt", ios::app);
            if (file.is_open()) {
                file << "Time: " << timeStr << "\n";
                file << "Student ID: " << studentId << "\n";
                file << "Report: " << report << "\n";
                file << "--------------------------\n";
                file.close();
                cout << "[OK] Report saved to reports.txt\n";
                addLog(studentId, "Suspicious behavior reported: " + report);
            } else {
                cout << "[ERROR] Could not write to reports.txt\n";
                addLog(studentId, "Failed to save suspicious behavior report");
            }

            cheatingScores[studentId] += 10;
            cout << "[!] Cheating score for " << studentId << ": " << cheatingScores[studentId] << " points\n";

            if (cheatingScores[studentId] >= 30) {
                cout << "[ALERT] Cheating threshold exceeded for " << studentId << "! Exam terminated.\n";
                terminateExam(studentId);
            }
        } else {
            cout << "[ERROR] Student " << studentId << " not found!\n";
        }
    }

    bool detectAnomaly(string studentId) {
        int authCount = 0;
        time_t currentTime = time(nullptr);
        LogNode* current = logHead;

        while (current) {
            if (current->studentId == studentId &&
                current->activity == "Authenticated successfully" &&
                (currentTime - current->timestamp) < 300) {
                authCount++;
            }
            current = current->next;
        }

        if (monitoringData.find(studentId) != monitoringData.end()) {
            if (monitoringData[studentId].audioLevel > 70.0) {
                reportSuspiciousBehavior(studentId, "Suspicious audio detected");
                return true;
            }
            if (monitoringData[studentId].movementCount > 5) {
                reportSuspiciousBehavior(studentId, "Suspicious movement detected");
                return true;
            }
            if (monitoringData[studentId].isTalking) {
                reportSuspiciousBehavior(studentId, "Talking with others detected");
                return true;
            }
            if (monitoringData[studentId].isUsingPhone) {
                reportSuspiciousBehavior(studentId, "Phone usage detected");
                return true;
            }
        }

        return authCount > 1;
    }

    void monitorStudent(string studentId, double audioLevel, int movementCount, bool isTalking, bool isUsingPhone) {
        if (students.find(studentId) != students.end() && students[studentId].isTakingExam) {
            monitoringData[studentId].audioLevel = audioLevel;
            monitoringData[studentId].movementCount = movementCount;
            monitoringData[studentId].isTalking = isTalking;
            monitoringData[studentId].isUsingPhone = isUsingPhone;
            monitoringData[studentId].lastChecked = time(nullptr);

            if (detectAnomaly(studentId)) {
                terminateExam(studentId);
            }
        } else {
            cout << "[ERROR] Cannot monitor: Student " << studentId << " not taking exam or not found!\n";
        }
    }

    void terminateExam(string studentId) {
        if (students.find(studentId) != students.end() && students[studentId].isTakingExam) {
            students[studentId].isTakingExam = false;
            addLog(studentId, "Exam terminated due to suspicious activity");
            cout << "[ALERT] Exam terminated for student " << studentId << "\n";
        }
    }

    void displayLogs() {
        LogNode* current = logHead;
        cout << "\n[LOGS] Monitoring Logs:\n";
        while (current) {
            char timeStr[26];
            ctime_r(&current->timestamp, timeStr);
            timeStr[24] = '\0';
            cout << "[" << timeStr << "] Student " << current->studentId
                 << ": " << current->activity << "\n";
            current = current->next;
        }
    }

    void storeResult(string studentId, string examId, int score) {
        if (students.find(studentId) != students.end()) {
            if (students[studentId].isTakingExam) {
                addLog(studentId, "Exam " + examId + " score: " + to_string(score));
                students[studentId].isTakingExam = false;
                cout << "[OK] Result stored for student " << studentId << "\n";
            } else {
                cout << "[ERROR] Cannot store result: Student " << studentId << " is not taking an exam!\n";
            }
        } else {
            cout << "[ERROR] Student " << studentId << " not found!\n";
        }
    }

    void runChatbot() {
        string choice, studentId, name, aadhar, faceData, examId, report;
        int score;
        double audioLevel;
        int movementCount;
        bool isTalking, isUsingPhone;

        while (true) {
            cout << "\n==============================\n";
            cout << "Smart Exam Invigilation System - Menu\n";
            cout << "==============================\n";
            cout << "1. View Exam Instructions\n";
            cout << "2. Camera & Mic Check\n";
            cout << "3. Face Detection Status\n";
            cout << "4. Report Suspicious Behavior\n";
            cout << "5. Verify Student Identity\n";
            cout << "6. Register Student\n";
            cout << "7. Authenticate Student\n";
            cout << "8. Schedule Exam\n";
            cout << "9. Start Exam\n";
            cout << "10. Monitor Student\n";
            cout << "11. Store Exam Result\n";
            cout << "12. Display Logs\n";
            cout << "13. Exit\n";
            cout << "Enter your choice: ";
            getline(cin, choice);

            if (choice == "1") {
                showInstructions();
            } else if (choice == "2") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cameraMicCheck(studentId);
            } else if (choice == "3") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                faceDetectionStatus(studentId);
            } else if (choice == "4") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Type your observation: ";
                getline(cin, report);
                reportSuspiciousBehavior(studentId, report);
            } else if (choice == "5") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter Full Name: ";
                getline(cin, name);
                cout << "Enter Aadhar Number (12 digits): ";
                getline(cin, aadhar);
                verifyStudentIdentity(studentId, name, aadhar);
            } else if (choice == "6") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter Full Name: ";
                getline(cin, name);
                cout << "Enter Face Data (simulated): ";
                getline(cin, faceData);
                cout << "Enter Aadhar Number (12 digits): ";
                getline(cin, aadhar);
                registerStudent(studentId, name, faceData, aadhar);
            } else if (choice == "7") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter Face Data (simulated): ";
                getline(cin, faceData);
                authenticateStudent(studentId, faceData);
            } else if (choice == "8") {
                cout << "Enter Exam ID: ";
                getline(cin, examId);
                cout << "Enter Subject: ";
                string subject;
                getline(cin, subject);
                cout << "Enter Duration (minutes): ";
                int duration;
                cin >> duration;
                cin.ignore();
                scheduleExam(examId, subject, time(nullptr) + 3600, duration);
            } else if (choice == "9") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter Exam ID: ";
                getline(cin, examId);
                startExam(studentId, examId);
            } else if (choice == "10") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter Audio Level (decibels): ";
                cin >> audioLevel;
                cout << "Enter Movement Count: ";
                cin >> movementCount;
                cout << "Is student talking? (1 for yes, 0 for no): ";
                cin >> isTalking;
                cout << "Is student using phone? (1 for yes, 0 for no): ";
                cin >> isUsingPhone;
                cin.ignore();
                monitorStudent(studentId, audioLevel, movementCount, isTalking, isUsingPhone);
            } else if (choice == "11") {
                cout << "Enter Student ID: ";
                getline(cin, studentId);
                cout << "Enter Exam ID: ";
                getline(cin, examId);
                cout << "Enter Score: ";
                cin >> score;
                cin.ignore();
                storeResult(studentId, examId, score);
            } else if (choice == "12") {
                displayLogs();
            } else if (choice == "13") {
                cout << "\n[EXIT] Thank you. Exam session ended.\n";
                break;
            } else {
                cout << "[ERROR] Invalid input. Please enter 1–13.\n";
            }
        }
    }
};

int main() {
    SmartExamSystem system;
    system.runChatbot();
    return 0;
}