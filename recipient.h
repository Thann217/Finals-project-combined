#ifndef RECIPIENT_H
#define RECIPIENT_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include "Queue.h"

using namespace std;

class RecipientNode;

class recipient {
private:
    string name;
    int id;
    float totalKgReceived;
    int donationCount;
    Queue foodRequestQueue;
    float totalMoneyReceived;

public:
    recipient(std::string n, int i) : name(n), id(i), totalKgReceived(0), 
                                     donationCount(0), totalMoneyReceived(0) {}

                                     float get_total_money() const { return totalMoneyReceived; }
    void add_money(float amount) { totalMoneyReceived += amount; }

    std::string get_name() const {
        return name;
    }

    int get_id() const {
        return id;
    }

    float get_total_kg() const { return totalKgReceived; }

    int get_donation_count() const { return donationCount; }

    void set_total_kg(float kg) { totalKgReceived = kg; }

    void set_donation_count(int count) { donationCount = count; }

    recipient& operator+=(float food) {
        totalKgReceived += food;
        donationCount++;
        return *this;
    }

    bool verify(int check_id) const {
        return id == check_id;
    }

    void display() const {
        cout << "ID: " << id << "\n"
             << "Name: " << name << "\n"
             << "Total kg received: " << fixed << setprecision(2) << totalKgReceived << "\n"
             << "Total money received: $" << fixed << setprecision(2) << totalMoneyReceived << "\n"
             << "Number of donations: " << donationCount << "\n"
             << "--------------------------------\n";
    }

    void request_food(int quantity) {
        foodRequestQueue.enqueue(id, quantity);
        cout << "Food request pending..." << endl;
    }

    bool distribute_food() {
        if (foodRequestQueue.getSize() > 0) {  // Changed from get_size
            NodePtr node = foodRequestQueue.dequeue();  // Changed from dequeue2
            float quantity = node->get_value();
            totalKgReceived += quantity;
            cout << "✅ Distributed " << quantity << " kg to " << name << "\n";
            delete node;
            return true;
        }
        cout << "⚠️  No pending requests for " << name << "\n";
        return false;
    }
    
    void display_requests() const {
        cout << "Pending requests: " << foodRequestQueue.getSize() << endl;  // Changed from get_size
        foodRequestQueue.displayRequests();
    }

    static void sort_id(std::vector<recipient>& recipients) {
        int n = recipients.size();
        for (int i = 1; i < n; i++) {
            recipient key = recipients[i];
            int j = i - 1;
            while (j >= 0 && recipients[j].get_id() > key.get_id()) {
                recipients[j + 1] = recipients[j];
                j = j - 1;
            }
            recipients[j + 1] = key;
        }
    }
};

class RecipientNode {
    public:
        recipient rec;
        RecipientNode* next;
    
        RecipientNode(const recipient& r) : rec(r), next(nullptr) {}
    };
    

    class RecipientLinkedList {
        private:
            RecipientNode* head;
            RecipientNode* tail;
            int size;
            const string SAVE_FILE = "recipients.dat";
            bool autoSave;
            std::unordered_map<int, RecipientNode*> recipientMap;

            void rebuildMap() {
                recipientMap.clear();
                RecipientNode* current = head;
                while (current) {
                    recipientMap[current->rec.get_id()] = current;
                    current = current->next;
                }
            }
    
            void clear() {
                while (head) {
                    RecipientNode* temp = head;
                    head = head->next;
                    delete temp;
                }
                head = tail = nullptr;
                size = 0;
                recipientMap.clear();
            }
        
            void saveToFile() {
                ofstream file(SAVE_FILE);
                if (!file) {
                    throw runtime_error("Cannot open file for writing");
                }
        
                RecipientNode* current = head;
                while (current) {
                    file << current->rec.get_id() << "\n"
                    << current->rec.get_name() << "\n"
                    << current->rec.get_total_kg() << "\n"
                    << current->rec.get_donation_count() << "\n"
                    << current->rec.get_total_money() << "\n";
                    current = current->next;
                }
            }

        

    public:
    RecipientLinkedList() : head(nullptr), tail(nullptr), size(0), autoSave(true) {
        loadFromFile();
    }

    void setAutoSave(bool enable) { autoSave = enable; }
    
    
    ~RecipientLinkedList() {
        forceSave(); // Use forceSave instead of direct saveToFile
        clear();
    }

    
    void forceSave() {
        saveToFile();
        if(autoSave) {
            cout << "Recipient data saved successfully." << endl;
        }
    }
    void loadFromFile() {
        ifstream in(SAVE_FILE);
        if (!in.is_open()) return;
    
        clear();
        string line;
    
        while (getline(in, line)) {
            try {
                int id = stoi(line);
                string name;
                getline(in, name);
                
                getline(in, line);
                float totalKg = stof(line);
                
                getline(in, line);
                int count = stoi(line);
    
                // Add this for money
                getline(in, line);
                float totalMoney = stof(line);
    
                if (recipientMap.find(id) == recipientMap.end()) {
                    recipient newRec(name, id);
                    newRec.set_total_kg(totalKg);
                    newRec.set_donation_count(count);
                    newRec.add_money(totalMoney);  // Changed from set_total_money
                    
                    bool oldAuto = autoSave;
                    autoSave = false;
                    addRecipient(newRec);
                    autoSave = oldAuto;
                }
            } catch (...) {
                continue;
            }
        }
    }

    RecipientNode* getHead() const { return head; }
    
    void addRecipient(const recipient& rec) {
        if (recipientMap.find(rec.get_id()) != recipientMap.end()) {
            cerr << "Error: Recipient ID " << rec.get_id() << " already exists!\n";
            return;
        }

        RecipientNode* newNode = new RecipientNode(rec);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        recipientMap[rec.get_id()] = newNode;
        size++;
        
        if (autoSave) forceSave();
    }
    
    void updateRecipient(int id, float kg) {
        recipient* rec = findRecipientById(id);
        if (rec) {
            *rec += kg;
            forceSave(); // Save after updating
        }
    }


    recipient* findRecipientById(int id) {
        RecipientNode* current = head;
        while (current) {
            if (current->rec.get_id() == id) {
                return &(current->rec);
            }
            current = current->next;
        }
        return nullptr;
    }

    const recipient* findRecipientById(int id) const {
        RecipientNode* current = head;
        while (current) {
            if (current->rec.get_id() == id) {
                return &(current->rec);
            }
            current = current->next;
        }
        return nullptr;
    }

    void displayAllRecipients() const {
        RecipientNode* current = head;
        while (current) {
            current->rec.display();
            current = current->next;
        }
    }

    int getSize() const { return size; }

    int getTotalDistributedFood() const {
        int total = 0;
        RecipientNode* current = head;
        while (current) {
            total += current->rec.get_total_kg();
            current = current->next;
        }
        return total;
    }

    void refreshForReport() {
        loadFromFile(); // Reload from disk to ensure latest data
    }

    void clearDataFile() {
        // Clear in-memory data
        clear();
        
        // Clear the file
        ofstream file(SAVE_FILE, ios::trunc);
        if (!file) {
            throw runtime_error("Failed to clear recipients file");
        }
        file.close();
        
        cout << "Recipients file cleared successfully.\n";
    }
};

#endif
