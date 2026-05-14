// Medical Center Management System - Console MVP Prototype
// Course: Software Engineering
// Team: Khaled Sbaih (12220191), Ahmad Alamiyh (12113721)
// Compile: g++ -std=c++17 main.cpp -o MedicalCenterMVP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

struct User {
    string username;
    string password;
    string role;
    bool active = true;
};

struct Doctor {
    int id;
    string name;
    string specialty;
};

struct Patient {
    int id;
    string fullName;
    string phone;
    string dateOfBirth;
    string gender;
};

struct Appointment {
    int id;
    int patientId;
    int doctorId;
    string date;
    string time;
    string reason;
    string status; // Scheduled, Checked-in, Completed, Cancelled
};

struct Consultation {
    int id;
    int appointmentId;
    int patientId;
    int doctorId;
    string symptoms;
    string diagnosis;
    string treatmentPlan;
    string prescription;
};

struct Payment {
    int id;
    int patientId;
    int appointmentId;
    double amount;
    string method;
    string status;
};

class MedicalCenterSystem {
private:
    vector<User> users;
    vector<Doctor> doctors;
    vector<Patient> patients;
    vector<Appointment> appointments;
    vector<Consultation> consultations;
    vector<Payment> payments;
    User currentUser;
    bool loggedIn = false;
    int nextPatientId = 1001;
    int nextAppointmentId = 2001;
    int nextConsultationId = 3001;
    int nextPaymentId = 4001;

    string readLine(const string& prompt) {
        cout << prompt;
        string value;
        getline(cin, value);
        return value;
    }

    int readInt(const string& prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Invalid number. Please try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    double readDouble(const string& prompt) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value && value > 0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Invalid amount. Please enter a positive value.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    bool requireRole(const vector<string>& allowedRoles) {
        if (!loggedIn) return false;
        return find(allowedRoles.begin(), allowedRoles.end(), currentUser.role) != allowedRoles.end();
    }

    Doctor* findDoctor(int id) {
        for (auto& d : doctors) if (d.id == id) return &d;
        return nullptr;
    }

    Patient* findPatient(int id) {
        for (auto& p : patients) if (p.id == id) return &p;
        return nullptr;
    }

    Appointment* findAppointment(int id) {
        for (auto& a : appointments) if (a.id == id) return &a;
        return nullptr;
    }

    bool isSlotAvailable(int doctorId, const string& date, const string& time, int ignoreAppointmentId = -1) {
        for (const auto& a : appointments) {
            if (a.id != ignoreAppointmentId && a.doctorId == doctorId && a.date == date && a.time == time && a.status != "Cancelled") {
                return false;
            }
        }
        return true;
    }

    void seedData() {
        users = {
            {"admin", "admin123", "Administrator", true},
            {"doctor", "doctor123", "Doctor", true},
            {"reception", "rec123", "Receptionist", true}
        };
        doctors = {
            {1, "Dr. Omar Nasser", "General Medicine"},
            {2, "Dr. Lina Ahmad", "Pediatrics"},
            {3, "Dr. Sami Saleh", "Dermatology"}
        };
        patients.push_back({nextPatientId++, "Ali Mansour", "0599000000", "2001-04-10", "Male"});
        patients.push_back({nextPatientId++, "Sara Khalil", "0599111111", "1998-09-21", "Female"});
    }

public:
    MedicalCenterSystem() { seedData(); }

    void login() {
        cout << "\n=== Login ===\n";
        string username = readLine("Username: ");
        string password = readLine("Password: ");
        for (const auto& u : users) {
            if (u.username == username && u.password == password && u.active) {
                currentUser = u;
                loggedIn = true;
                cout << "Login successful. Role: " << currentUser.role << "\n";
                return;
            }
        }
        cout << "Invalid username/password or inactive account.\n";
    }

    void listDoctors() {
        cout << "\nDoctors:\n";
        for (const auto& d : doctors) {
            cout << d.id << " - " << d.name << " (" << d.specialty << ")\n";
        }
    }

    void registerPatient() {
        if (!requireRole({"Receptionist", "Administrator"})) { cout << "Access denied.\n"; return; }
        cout << "\n=== Register New Patient ===\n";
        Patient p;
        p.id = nextPatientId++;
        p.fullName = readLine("Full name: ");
        p.phone = readLine("Phone number: ");
        p.dateOfBirth = readLine("Date of birth (YYYY-MM-DD): ");
        p.gender = readLine("Gender: ");
        if (p.fullName.empty() || p.phone.empty() || p.dateOfBirth.empty() || p.gender.empty()) {
            cout << "Registration failed. All fields are mandatory.\n";
            nextPatientId--;
            return;
        }
        patients.push_back(p);
        cout << "Patient registered successfully. Patient ID: " << p.id << "\n";
    }

    void searchPatients() {
        if (!requireRole({"Receptionist", "Doctor", "Administrator"})) { cout << "Access denied.\n"; return; }
        cout << "\n=== Search Patients ===\n";
        string key = readLine("Enter patient ID, name, or phone: ");
        bool found = false;
        for (const auto& p : patients) {
            if (to_string(p.id) == key || p.fullName.find(key) != string::npos || p.phone.find(key) != string::npos) {
                cout << "ID: " << p.id << " | Name: " << p.fullName << " | Phone: " << p.phone
                     << " | DOB: " << p.dateOfBirth << " | Gender: " << p.gender << "\n";
                found = true;
            }
        }
        if (!found) cout << "No matching patients found.\n";
    }

    void bookAppointment() {
        if (!requireRole({"Receptionist", "Administrator"})) { cout << "Access denied.\n"; return; }
        cout << "\n=== Book Appointment ===\n";
        int patientId = readInt("Patient ID: ");
        if (!findPatient(patientId)) { cout << "Patient not found.\n"; return; }
        listDoctors();
        int doctorId = readInt("Doctor ID: ");
        if (!findDoctor(doctorId)) { cout << "Doctor not found.\n"; return; }
        string date = readLine("Date (YYYY-MM-DD): ");
        string time = readLine("Time (HH:MM): ");
        if (!isSlotAvailable(doctorId, date, time)) {
            cout << "Double-booking prevented. This doctor already has an appointment at the selected date and time.\n";
            return;
        }
        string reason = readLine("Visit reason: ");
        appointments.push_back({nextAppointmentId++, patientId, doctorId, date, time, reason, "Scheduled"});
        cout << "Appointment booked successfully.\n";
    }

    void listAppointments() {
        if (!requireRole({"Receptionist", "Doctor", "Administrator"})) { cout << "Access denied.\n"; return; }
        cout << "\n=== Appointments ===\n";
        for (const auto& a : appointments) {
            Patient* p = const_cast<MedicalCenterSystem*>(this)->findPatient(a.patientId);
            Doctor* d = const_cast<MedicalCenterSystem*>(this)->findDoctor(a.doctorId);
            cout << "Appointment ID: " << a.id << " | Patient: " << (p ? p->fullName : "Unknown")
                 << " | Doctor: " << (d ? d->name : "Unknown") << " | " << a.date << " " << a.time
                 << " | Status: " << a.status << " | Reason: " << a.reason << "\n";
        }
        if (appointments.empty()) cout << "No appointments recorded.\n";
    }

    void checkInPatient() {
        if (!requireRole({"Receptionist", "Administrator"})) { cout << "Access denied.\n"; return; }
        int id = readInt("Appointment ID to check in: ");
        Appointment* a = findAppointment(id);
        if (!a || a->status == "Cancelled") { cout << "Valid appointment not found.\n"; return; }
        a->status = "Checked-in";
        cout << "Patient checked in successfully.\n";
    }

    void cancelAppointment() {
        if (!requireRole({"Receptionist", "Administrator"})) { cout << "Access denied.\n"; return; }
        int id = readInt("Appointment ID to cancel: ");
        Appointment* a = findAppointment(id);
        if (!a) { cout << "Appointment not found.\n"; return; }
        a->status = "Cancelled";
        cout << "Appointment cancelled.\n";
    }

    void recordConsultation() {
        if (!requireRole({"Doctor", "Administrator"})) { cout << "Access denied.\n"; return; }
        cout << "\n=== Record Consultation ===\n";
        int appointmentId = readInt("Appointment ID: ");
        Appointment* a = findAppointment(appointmentId);
        if (!a || (a->status != "Checked-in" && a->status != "Scheduled")) {
            cout << "Appointment must exist and be active.\n"; return;
        }
        Consultation c;
        c.id = nextConsultationId++;
        c.appointmentId = appointmentId;
        c.patientId = a->patientId;
        c.doctorId = a->doctorId;
        c.symptoms = readLine("Symptoms: ");
        c.diagnosis = readLine("Diagnosis: ");
        c.treatmentPlan = readLine("Treatment plan: ");
        c.prescription = readLine("Prescription: ");
        consultations.push_back(c);
        a->status = "Completed";
        cout << "Consultation saved and appointment marked as completed.\n";
    }

    void recordPayment() {
        if (!requireRole({"Receptionist", "Administrator"})) { cout << "Access denied.\n"; return; }
        cout << "\n=== Record Payment ===\n";
        int appointmentId = readInt("Appointment ID: ");
        Appointment* a = findAppointment(appointmentId);
        if (!a) { cout << "Appointment not found.\n"; return; }
        double amount = readDouble("Amount: ");
        string method = readLine("Payment method (Cash/Card/Insurance): ");
        payments.push_back({nextPaymentId++, a->patientId, appointmentId, amount, method, "Paid"});
        cout << "Payment recorded successfully.\n";
    }

    void generateReport() {
        if (!requireRole({"Administrator"})) { cout << "Access denied.\n"; return; }
        double revenue = 0;
        for (const auto& p : payments) revenue += p.amount;
        cout << "\n=== Administrative Report ===\n";
        cout << "Total patients: " << patients.size() << "\n";
        cout << "Total doctors: " << doctors.size() << "\n";
        cout << "Total appointments: " << appointments.size() << "\n";
        cout << "Completed consultations: " << consultations.size() << "\n";
        cout << fixed << setprecision(2) << "Total revenue: " << revenue << "\n";
    }

    void showMenu() {
        while (true) {
            if (!loggedIn) login();
            if (!loggedIn) continue;
            cout << "\n=== Medical Center Management System ===\n";
            cout << "1. Register patient\n";
            cout << "2. Search patients\n";
            cout << "3. Book appointment\n";
            cout << "4. List appointments\n";
            cout << "5. Check-in patient\n";
            cout << "6. Cancel appointment\n";
            cout << "7. Record consultation\n";
            cout << "8. Record payment\n";
            cout << "9. Generate admin report\n";
            cout << "10. List doctors\n";
            cout << "0. Exit\n";
            int choice = readInt("Choose: ");
            switch (choice) {
                case 1: registerPatient(); break;
                case 2: searchPatients(); break;
                case 3: bookAppointment(); break;
                case 4: listAppointments(); break;
                case 5: checkInPatient(); break;
                case 6: cancelAppointment(); break;
                case 7: recordConsultation(); break;
                case 8: recordPayment(); break;
                case 9: generateReport(); break;
                case 10: listDoctors(); break;
                case 0: cout << "Goodbye.\n"; return;
                default: cout << "Invalid option.\n";
            }
        }
    }
};

int main() {
    MedicalCenterSystem system;
    system.showMenu();
    return 0;
}
