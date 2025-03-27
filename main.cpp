#include <iostream>
#include "donor.h"
#include "recipient.h"
#include "donation.h"
#include "reporting.h"
#include <random>
#include <limits> // For numeric_limits

using namespace std;

void displayMenu() {
    cout << "-------------------------------------\n";
    cout << "1. Register Donor\n";
    cout << "2. Create Donation\n";
    cout << "3. Donation Report\n";
    cout << "4. Distribution Report\n";
    cout << "5. Donor Report\n";
    cout << "6. Overall Summary\n";
    cout << "7. Distribution Summary\n";
    cout << "8. Donor Rankings\n";
    cout << "9. Delete Donors\n";
    cout << "10. Food Requests\n";
    cout << "11. Distributed Food\n";
    cout << "12. Clear All Recipients Data\n";
    cout << "E. Exit\n";
    cout << "Choose an option: ";
}

bool isValidDate(const string& date) {
    // Basic format check
    if (date.length() != 10 || date[2] != '-' || date[5] != '-') 
        return false;
    
    // Check day/month ranges
    int day = stoi(date.substr(0,2));
    int month = stoi(date.substr(3,2));
    
    return (day >= 1 && day <= 31) && (month >= 1 && month <= 12);
}

void displayAvailableDonors(const DonorManager& dm) {
    cout << "\nRegistered Donors:\n";
    for (const auto& donor : dm.getDonors()) {
        cout << "- " << donor.get_name() << endl;
    }
}

int getValidatedInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (!(cin >> value)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between " 
                 << min << " and " << max << ".\n";
            continue;
        }
        if (value >= min && value <= max) {
            cin.ignore();  // Clear the newline
            return value;
        }
        cout << "Please enter a number between " << min << " and " << max << ".\n";
    }
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100, 999);
    DonorManager donorManager;
    RecipientLinkedList recipients; // Use RecipientLinkedList instead of vector<recipient>
    recipients.setAutoSave(false);
    Reporting report(donorManager, recipients);

    recipients.loadFromFile();
    report.cleanupOrphanedDonations(donorManager, recipients);
    const int DEFAULT_IDS[] = {101, 102};
    const string DEFAULT_NAMES[] = {"Food Bank", "Shelter"};
    
    for (int i = 0; i < 2; i++) {
        if (!recipients.findRecipientById(DEFAULT_IDS[i])) {
            // Create temporary recipient object
            recipient newRec(DEFAULT_NAMES[i], DEFAULT_IDS[i]);
            
            // Explicitly initialize all fields
            newRec.set_total_kg(0);
            newRec.set_donation_count(0); 
            newRec.add_money(0);   // Add this line
            
            // Add to list
            recipients.addRecipient(newRec);
        }
    }

    recipients.setAutoSave(true);
    


    string input;  // Store input as a string

    do {
        displayMenu();
        cin >> input;
        cin.ignore();  // Clear the newline character

        if (input == "e" || input == "E") {
            cout << "Exiting...\n";
            break;  // Exit the loop
        }

        // Convert input to integer for numeric choices
        try {

            int choice = stoi(input);  // String to integer
            switch (choice) {
                case 1: {
                string name, contact;
                cout << "Enter donor name: ";
                getline(cin, name);
                cout << "Enter donor contact details: ";
                getline(cin, contact);
                int id = dist(gen);
                donorManager.register_donor(name, contact, id);
                cout << "Your ID is " << id << endl;
                cout << "Donor registered successfully!\n";
                //recipients.addRecipient(recipient(name, id));  // This adds the donor as a recipient
                break;
            }
            case 2: {
                int donationType;
                cout << "Choose donation type:\n"
                     << "1. Food\n"
                     << "2. Money\n"
                     << "Enter choice: ";
                cin >> donationType;
                cin.ignore();
            
                if (donationType == 1) {
                    // Existing food donation logic
                    string donorName, foodType, date;
                    int recipientId, quantity;
                    
                //cout << "Enter donor ID: ";
                //cin >> donorId;
                //cin.ignore();
            
                // Display available options first
                cout << "\nAvailable Recipients:\n";
                recipients.displayAllRecipients();
                cout << "\nRegistered Donors:\n";
                for (const auto& donor : donorManager.getDonors()) {
                    cout << "- " << donor.get_name() << endl;
                }

            
                // Get donation details
                cout << "\nEnter donor name (or 'cancel' to abort): ";
                getline(cin, donorName);
                if (donorName == "cancel") break;

                int donorId = -1;
for (const auto& donor : donorManager.getDonors()) {
    if (donor.get_name() == donorName) {
        donorId = donor.get_id();
        break;
    }
}
if (donorId == -1) {
    cout << "Donor not found. Returning to menu.\n";
    continue;
}
            
                cout << "Enter recipient ID: ";
                if (!(cin >> recipientId)) { // Check if input failed
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Returning to menu.\n";
                    break;
                }
                cin.ignore();
                
                cout << "Enter food type: ";
                getline(cin, foodType);
                
                cout << "Enter quantity (in kg): ";
                if (!(cin >> quantity) || quantity <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid quantity. Returning to menu.\n";
                    break;
                }
                cin.ignore();
                
                cout << "Enter donation date (DD-MM-YYYY): ";
                getline(cin, date);
                if (!isValidDate(date)) {
                    cout << "Invalid date format. Returning to menu.\n";
                    break;
                }
            
                // Validate donor and recipient
                bool donorValid = false;
                for (const auto& donor : donorManager.getDonors()) {
                    if (donor.get_name() == donorName) {
                        donorValid = true;
                        break;
                    }
                }
                
                if (!donorValid) {
                    cout << "Donor not found. Returning to menu.\n";
                    break;
                }
            
                if (!recipients.findRecipientById(recipientId)) {
                    cout << "Recipient not found. Returning to menu.\n";
                    break;
                }
            
                // All validations passed - create donation
                Donation donation(donorId, donorName, recipientId, foodType, quantity, date);
        processDonation(donorManager, recipients, donation);
        report.addDonation(donation);
                cout << "\nDonation recorded successfully!\n";
                break;
            } 
            else if (donationType == 2) {
                // Money donation logic
                string donorName, date;
                float amount;
                int recipientId;
            
                // Clear input buffer first
                //cin.clear();
                //cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
                // Show available recipients and donors FIRST
                cout << "\nAvailable Recipients:\n";
                recipients.displayAllRecipients();
                
                cout << "\nRegistered Donors:\n";
                for (const auto& donor : donorManager.getDonors()) {
                    cout << "- " << donor.get_name() << endl;
                }
            
                // Get donation details
                cout << "\nEnter donor name (or 'cancel' to abort): ";
                if (!getline(cin, donorName)) {
                    cin.clear();
                    cout << "Input error. Returning to menu.\n";
                    continue;
                }
                if (donorName == "cancel") break;
            
                cout << "Enter recipient ID: ";
                while (!(cin >> recipientId)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number: ";
                }
                cin.ignore(); // Clear newline
            
                cout << "Enter amount to donate ($): ";
                while (!(cin >> amount) || amount <= 0) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid amount. Please enter positive number: ";
                }
                cin.ignore(); // Clear newline
            
                cout << "Enter donation date (DD-MM-YYYY): ";
                while (!getline(cin, date) || !isValidDate(date)) {
                    if (!cin) cin.clear();
                    cout << "Invalid date. Use DD-MM-YYYY format: ";
                }
            
                // Verify donor exists
                bool donorValid = false;
                int donorId = -1;
                for (const auto& donor : donorManager.getDonors()) {
                    if (donor.get_name() == donorName) {
                        donorValid = true;
                        donorId = donor.get_id();
                        break;
                    }
                }
            
                if (!donorValid) {
                    cout << "Donor not found. Returning to menu.\n";
                    continue;
                }
            
                if (!recipients.findRecipientById(recipientId)) {
                    cout << "Recipient not found. Returning to menu.\n";
                    continue;
                }
            
                Donation moneyDonation(donorId, donorName, recipientId, amount, date);
                processDonation(donorManager, recipients, moneyDonation);
                report.addDonation(moneyDonation);
                cout << "\nMoney donation recorded successfully!\n";
                break;
            }
            case 3: {
                int sortChoice = getValidatedInt(
                    "Sort donations by:\n"
                    "1. Quantity (highest first)\n"
                    "2. Date (newest first)\n"
                    "3. Money (highest first)\n"
                    "4. Back to Main Menu\n"
                    "Choose option: ", 1, 4);
                
                if (sortChoice == 4) break;
                report.generateDonationReport(sortChoice - 1);  // Convert to 0-based index
                break;
            }
            case 4: {
                cout << "\n=== Distribution Report ===\n";
                RecipientNode* current = recipients.getHead();
                while (current) {
                    current->rec.display();
                    cout << "Money Received: $" << fixed << setprecision(2) 
                         << current->rec.get_total_money() << "\n";
                    cout << "Total Donations: " << current->rec.get_donation_count() << "\n";
                    cout << "--------------------------\n";
                    current = current->next;
                }
                cout << "==========================\n";
                break;
            }
            case 5:
                report.generateDonorReport();
                break;
            case 6:
                report.generateOverallSummary();
                break;
                case 7: {
                    float totalMoney = 0;
                    int totalDonations = 0;
                    RecipientNode* current = recipients.getHead();
                    while (current) {
                        totalMoney += current->rec.get_total_money();
                        totalDonations += current->rec.get_donation_count();
                        current = current->next;
                    }
                
                    cout << "\n=== Distribution Summary ===\n";
                    cout << "Total Recipients: " << recipients.getSize() << endl;
                    cout << "Total Food Distributed: " << recipients.getTotalDistributedFood() << " kg\n";
                    cout << "Total Money Distributed: $" << fixed << setprecision(2) << totalMoney << "\n";
                    cout << "Total Donations Received: " << totalDonations << "\n";
                    cout << "===========================\n";
                    break;
                }
            case 8: 
                report.displayDonorRankings();
                break;
                case 9: {
                    donorManager.listDonorIDs();
                    int id;
                    cout << "Enter donor ID to delete: ";
                    cin >> id;
                    cin.ignore();
                    
                    if (donorManager.delete_donor(id)) {
                        // Remove associated donations
                        auto& donations = report.getDonations(); // Add getDonations() to Reporting class
                        donations.erase(
                            remove_if(donations.begin(), donations.end(),
                                [id](const Donation& d) {
                                    return d.getDonorId() == id;
                                }),
                            donations.end()
                        );
                        cout << "Donor and their donations deleted successfully.\n";
                    }
                    break;
                }
                case 10: {
                    int recipientId, quantity;
                    cout << "Enter recipient ID requesting food: ";
                    cin >> recipientId;
                    cout << "Enter quantity needed (kg): ";
                    cin >> quantity;
                    cin.ignore();
                    
                    recipient* rec = recipients.findRecipientById(recipientId);
                    if (rec) {
                        rec->request_food(quantity);
                        cout << "Food request added to queue." << endl;
                    } else {
                        cout << "Recipient not found!" << endl;
                    }
                    break;
                }
                case 11: {
                    int recipientId;
                    cout << "Enter recipient ID to distribute food: ";
                    cin >> recipientId;
                    cin.ignore();
                    
                    recipient* rec = recipients.findRecipientById(recipientId);
                    if (rec) {
                        if (rec->distribute_food()) {
                            recipients.forceSave();
                        }
                    } else {
                        cout << "Recipient not found!" << endl;
                    }
                    break;
                }
                case 12: {
                    char confirm;
                    cout << "WARNING: This will permanently delete all recipients data!\n";
                    cout << "Are you sure? (y/n): ";
                    cin >> confirm;
                    cin.ignore();
                    
                    if (tolower(confirm) == 'y') {
                        try {
                            recipients.clearDataFile();
                        } catch (const exception& e) {
                            cerr << "Error: " << e.what() << endl;
                        }
                    } else {
                        cout << "Operation cancelled.\n";
                    }
                    break;
                }
            
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
    } catch (...) {  // Handle non-numeric input (e.g., "abc")
        cout << "Invalid input. Please enter a number or 'e' to exit.\n";
    }
} while (true);  // Loop until 'e' is entered

//recipients.forceSave();
//report.forceSaveAll();
return 0;
}