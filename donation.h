#ifndef DONATION_H
#define DONATION_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "recipient.h"
#include "donor.h"

class Donation {
private:
    std::string donorName;
    int recipientId;
    std::string foodType;
    int quantity;
    std::string date;
    bool isMoney;
    float moneyAmount;
    int donorId;

public:
    // Constructor for food donation
    Donation(int dId, std::string dName, int rId, std::string fType, int qty, std::string dt)
    : donorId(dId), donorName(dName), recipientId(rId), foodType(fType), 
      quantity(qty), date(dt), isMoney(false), moneyAmount(0) {}

    // Constructor for money donation
    Donation(int dId, std::string dName, int rId, float amount, std::string dt)
    : donorId(dId), donorName(dName), recipientId(rId), foodType(""), quantity(0), date(dt), 
      isMoney(true), moneyAmount(amount) {}

     // File I/O
     void save(std::ofstream& out) const {
        out << donorId << "\n"
            << donorName << "\n"
            << recipientId << "\n"
            << foodType << "\n"
            << quantity << "\n"
            << date << "\n"
            << isMoney << "\n"
            << moneyAmount << "\n";
        
        // Add debug output
        std::cout << "DEBUG: Saving donation - "
          << "Donor: " << donorName 
          << ", Recipient: " << recipientId
          << ", Type: " << (isMoney ? "Money" : "Food")
          << ", Amount: " << (isMoney ? moneyAmount : quantity)
          << std::endl;
    }

    static Donation load(std::ifstream& in) {
        std::string name, food, date;
        int id, qty;
        float money;
        bool isMoney;
        int donorId;
        in >> donorId; in.ignore();
        
        std::getline(in, name);
        in >> id; in.ignore();
        std::getline(in, food);
        in >> qty; in.ignore();
        std::getline(in, date);
        in >> isMoney; in.ignore();
        in >> money; in.ignore();
        
        if (isMoney) {
            return Donation(donorId, name, id, money, date);
        } else {
            return Donation(donorId, name, id, food, qty, date);
        }
    }

    // Getters
    std::string getDonorName() const { return donorName; }
    int getRecipientId() const { return recipientId; }
    std::string getFoodType() const { return foodType; }
    int getQuantity() const { return quantity; }
    std::string getDate() const { return date; }
    bool isMoneyDonation() const { return isMoney; }
    float getMoneyAmount() const { return moneyAmount; }
    int getDonorId() const { return donorId; }

    void printDetails() const {
        if (isMoney) {
            std::cout << "Date: " << date << " | "
                  << "Donor: " << donorName << " | "
                  << "Recipient ID: " << recipientId << " | "
                  << "Donation: Money | "
                  << "Amount: $" << std::fixed << std::setprecision(2) << moneyAmount << std::endl;
        } else {
            std::cout << "Date: " << date << " | "
                  << "Donor: " << donorName << " | "
                  << "Recipient ID: " << recipientId << " | "
                  << "Food: " << foodType << " | "
                  << "Quantity: " << quantity << " kg" << std::endl;
        }
    }

int getYear() const { return stoi(date.substr(6, 4)); } // YYYY
int getMonth() const { return stoi(date.substr(3, 2)); } // MM
int getDay() const { return stoi(date.substr(0, 2)); }   // DD

bool isNewerThan(const Donation& other) const {
    // Compare years
    if (getYear() != other.getYear()) 
        return getYear() > other.getYear();
    // Compare months if same year
    if (getMonth() != other.getMonth())
        return getMonth() > other.getMonth();
    // Compare days if same month/year
    return getDay() > other.getDay();
}
};

void bubbleSort(std::vector<Donation>& donations) {
    int n = donations.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (donations[j].getQuantity() < donations[j + 1].getQuantity()) {
                std::swap(donations[j], donations[j + 1]);
            }
        }
    }
}

void processDonation(DonorManager& donorManager, RecipientLinkedList& recipients, const Donation& donation) {
    if (donation.isMoneyDonation()) {
        donorManager.track_money_donation(donation.getDonorName(), donation.getMoneyAmount());
        recipient* rec = recipients.findRecipientById(donation.getRecipientId());
        if (rec) {
            rec->add_money(donation.getMoneyAmount());
            rec->set_donation_count(rec->get_donation_count() + 1);
            recipients.forceSave();
        }
    } else {
        recipient* rec = recipients.findRecipientById(donation.getRecipientId());
        if (rec) {
            donorManager.track_donation(donation.getDonorName(), *rec, donation.getQuantity());
            recipients.forceSave();
        }
    }
}
#endif
