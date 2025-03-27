#ifndef DONOR_H
#define DONOR_H

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include "recipient.h"

class Donor {
private:
    std::string name;
    std::string contactDetails;
    int donationFrequency;
    int id;
    float temp_kg;
    float moneyDonated;

public:
    Donor(std::string n, std::string contact, int id) 
        : name(n), contactDetails(contact), id(id), donationFrequency(0), temp_kg(0), moneyDonated(0) {}

    // Getters
    std::string get_name() const { return name; }
    int get_id() const { return id; }
    std::string get_contact_details() const { return contactDetails; }
    int get_donation_frequency() const { return donationFrequency; }
    float get_temp_kg() const { return temp_kg; }
    float get_money_donated() const { return moneyDonated; }

    void update_contact_details(std::string newContact) {
        contactDetails = newContact;
    }

    void increment_donation_frequency() {
        donationFrequency++;
    }

    void set_temp_kg(float kg) { temp_kg = kg; }
    
    void add_money(float amount) {
        moneyDonated += amount;
    }
};

class DonorManager {
    private:
        std::vector<Donor> donors;
        const std::string dataFile = "donors.dat";
    
        void loadDonors() {
            std::ifstream file(dataFile);
            if (file) {
                donors.clear();
                std::string name, contact;
                int id, freq;
                while (file >> name >> contact >> id >> freq) {
                    Donor d(name, contact, id);
                    for (int i = 0; i < freq; i++) d.increment_donation_frequency();
                    donors.push_back(d);
                }
            }
        }
    
        void saveDonors() {
            std::ofstream file(dataFile);
            for (const auto& donor : donors) {
                file << donor.get_name() << " "
                     << donor.get_contact_details() << " "
                     << donor.get_id() << " "
                     << donor.get_donation_frequency() << "\n";
            }
        }
public:
        DonorManager() { loadDonors(); }
        ~DonorManager() { saveDonors(); }
    
        void register_donor(std::string name, std::string contact, int id) {
            donors.emplace_back(name, contact, id);
        }
    
        void track_donation(std::string name, recipient& rec, int foodAmount) {
            for (auto& donor : donors) {
                if (donor.get_name() == name) {
                    donor.increment_donation_frequency();
                    rec += foodAmount;
                    return;
                }
            }
            std::cout << "Donor not found!" << std::endl;
        }
    
        const std::vector<Donor>& getDonors() const { return donors; }
    
        bool delete_donor(int id) {
            auto it = std::find_if(donors.begin(), donors.end(), 
                [id](const Donor& d) { return d.get_id() == id; });
            
            if (it != donors.end()) {
                donors.erase(it);
                return true;
            }
            return false;
        }
        
        void listDonorIDs() const {
            if (donors.empty()) {
                std::cout << "No donors registered." << std::endl;
                return;
            }
    
            std::cout << "\nRegistered Donor IDs:\n";
            std::cout << "----------------------\n";
            for (const auto& donor : donors) {
                std::cout << "ID: " << donor.get_id() 
                          << " | Name: " << donor.get_name() << std::endl;
            }
            std::cout << "----------------------\n";
        }

        void track_money_donation(std::string name, float amount) {
            for (auto& donor : donors) {
                if (donor.get_name() == name) {
                    donor.increment_donation_frequency();
                    donor.add_money(amount);
                    return;
                }
            }
            std::cout << "Donor not found!" << std::endl;
        }


};
#endif
