#ifndef REPORTING_H
#define REPORTING_H

#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include "donation.h"
#include "recipient.h"
#include "donor.h"

class Reporting {
private:
    std::vector<Donation> donations;
    RecipientLinkedList& recipients;
    DonorManager& donorManager;
    const std::string dataFile = "donations.dat";

    void loadDonations() {
        std::ifstream in(dataFile);
        if (in) {
            donations.clear();
            while (in.peek() != EOF) {
                donations.push_back(Donation::load(in));
            }
        }
    }

    void saveDonations() {
        std::ofstream out(dataFile);
        for (const auto& d : donations) {
            d.save(out);
        }
    }

    

    void rankByDonationFrequency() {
        const std::vector<Donor>& donors = donorManager.getDonors();
        std::vector<Donor> sortedDonors = donors;

        std::sort(sortedDonors.begin(), sortedDonors.end(),
            [](const Donor& a, const Donor& b) {
                return a.get_donation_frequency() > b.get_donation_frequency();
            });

        std::cout << "\n=== Donor Rankings by Frequency ===\n";
        std::cout << "Rank\tName\t\tDonations\n";
        std::cout << "--------------------------------\n";
        for (size_t i = 0; i < sortedDonors.size(); i++) {
            std::cout << i+1 << ".\t" << sortedDonors[i].get_name() 
                     << "\t\t" << sortedDonors[i].get_donation_frequency() << std::endl;
        }
        std::cout << "================================\n\n";
    }

    void rankByTotalKgDonated() {
        const std::vector<Donor>& donors = donorManager.getDonors();
        std::vector<Donor> sortedDonors = donors;

        for (auto& donor : sortedDonors) {
            float totalKg = 0;
            for (const auto& donation : donations) {
                if (donation.getDonorName() == donor.get_name()) {
                    totalKg += donation.getQuantity();
                }
            }
            donor.set_temp_kg(totalKg);
        }

        std::sort(sortedDonors.begin(), sortedDonors.end(),
            [](const Donor& a, const Donor& b) {
                return a.get_temp_kg() > b.get_temp_kg();
            });

        std::cout << "\n=== Donor Rankings by Kg Donated ===\n";
        std::cout << "Rank\tName\t\tKg Donated\n";
        std::cout << "--------------------------------\n";
        for (size_t i = 0; i < sortedDonors.size(); i++) {
            std::cout << i+1 << ".\t" << sortedDonors[i].get_name() 
                     << "\t\t" << sortedDonors[i].get_temp_kg() << " kg" << std::endl;
        }
        std::cout << "================================\n\n";
    }

    void rankByTotalMoneyDonated() {
        const std::vector<Donor>& donors = donorManager.getDonors();
        std::vector<Donor> sortedDonors = donors;
    
        std::sort(sortedDonors.begin(), sortedDonors.end(),
            [](const Donor& a, const Donor& b) {
                return a.get_money_donated() > b.get_money_donated();
            });
    
        std::cout << "\n=== Donor Rankings by Money Donated ===\n";
        std::cout << "Rank\tName\t\tAmount Donated\n";
        std::cout << "--------------------------------\n";
        for (size_t i = 0; i < sortedDonors.size(); i++) {
            std::cout << i+1 << ".\t" << sortedDonors[i].get_name() 
                     << "\t\t$" << std::fixed << std::setprecision(2) 
                     << sortedDonors[i].get_money_donated() << std::endl;
        }
        std::cout << "================================\n\n";
    }

public:
    Reporting(DonorManager& dm, RecipientLinkedList& r) : donorManager(dm), recipients(r) {
        loadDonations();
    }

    ~Reporting() {
        saveDonations();
    }

    std::vector<Donation>& getDonations() { return donations; }

    void addDonation(const Donation& donation) {
        donations.push_back(donation);
    }

    void generateDonationReport(int sortType = 0) {  // 0=quantity, 1=date, 2=money
        if (donations.empty()) {
            cout << "No donations recorded.\n";
            return;
        }
    
        // Create a copy for sorting
        auto sortedDonations = donations;
    
        // Sort based on user choice
        switch (sortType) {
            case 1:  // Date (newest first)
                std::sort(sortedDonations.begin(), sortedDonations.end(),
                    [](const Donation& a, const Donation& b) {
                        return a.getDate() > b.getDate();  // Implement date comparison
                    });
                break;
            case 2:  // Money (highest first)
                std::sort(sortedDonations.begin(), sortedDonations.end(),
                    [](const Donation& a, const Donation& b) {
                        return a.isMoneyDonation() && 
                               (b.isMoneyDonation() ? a.getMoneyAmount() > b.getMoneyAmount() : true);
                    });
                break;
            default:  // Quantity (highest first)
                std::sort(sortedDonations.begin(), sortedDonations.end(),
                    [](const Donation& a, const Donation& b) {
                        return a.getQuantity() > b.getQuantity();
                    });
        }
    
        float totalMoney = 0;
        float totalFood = 0;
    
        cout << "\n=== Donation Report ===\n";
        for (const auto& donation : sortedDonations) {
            donation.printDetails();
            if (donation.isMoneyDonation()) {
                totalMoney += donation.getMoneyAmount();
            } else {
                totalFood += donation.getQuantity();
            }
        }
    
        cout << "══════════════════════\n"
             << "Total Money Donated: $" << fixed << setprecision(2) << totalMoney << "\n"
             << "Total Food Donated: " << totalFood << " kg\n"
             << "══════════════════════\n";
    }

    void generateDistributionReport() {
        if (recipients.getSize() == 0) {
            std::cout << "No recipients available for reporting." << std::endl;
            return;
        }
        
        // Calculate total money received per recipient
        std::unordered_map<int, float> moneyReceived;
        for (const auto& donation : donations) {
            if (donation.isMoneyDonation()) {
                moneyReceived[donation.getRecipientId()] += donation.getMoneyAmount();
            }
        }
        
        std::cout << "=== Recipient Distribution Report ===\n";
        RecipientNode* current = recipients.getHead();
        while (current) {
            current->rec.display();
            if (moneyReceived.count(current->rec.get_id())) {
                std::cout << "Total Money Received: $" << std::fixed << std::setprecision(2) 
                          << moneyReceived[current->rec.get_id()] << std::endl;
            }
            std::cout << "--------------------------------\n";
            current = current->next;
        }
    }

    void generateDonorReport() {
        const auto& donors = donorManager.getDonors();
        if (donors.empty()) {
            std::cout << "No donors available for reporting." << std::endl;
            return;
        }
    
        std::cout << "=== Donor Report ===\n";
        std::cout << "ID\tName\t\tContact\t\tDonations\n";
        std::cout << "------------------------------------------------\n";
        
        for (const auto& donor : donors) {
            std::cout << donor.get_id() << "\t"
                     << donor.get_name() << "\t\t"
                     << donor.get_contact_details() << "\t\t"
                     << donor.get_donation_frequency() << std::endl;
        }
        std::cout << "================================================\n";
    }

    void generateOverallSummary() {
        int totalDonations = donations.size();
        int totalQuantity = std::accumulate(donations.begin(), donations.end(), 0,
                                      [](int sum, const Donation& donation) {
                                          return sum + (donation.isMoneyDonation() ? 0 : donation.getQuantity());
                                      });
        float totalMoney = std::accumulate(donations.begin(), donations.end(), 0.0f,
                                      [](float sum, const Donation& donation) {
                                          return sum + (donation.isMoneyDonation() ? donation.getMoneyAmount() : 0);
                                      });
    
        std::cout << "Overall Summary of Donations:" << std::endl;
        std::cout << "Total Donations: " << totalDonations << std::endl;
        std::cout << "Total Food Donated: " << totalQuantity << " kg" << std::endl;
        std::cout << "Total Money Donated: $" << std::fixed << std::setprecision(2) << totalMoney << std::endl;
        std::cout << "----------------------------------" << std::endl;
    }
    

    void generateDistributionSummary() {
        int totalRecipients = recipients.getSize();
        int totalDistributedFood = recipients.getTotalDistributedFood();
        float totalMoney = std::accumulate(donations.begin(), donations.end(), 0.0f,
                                      [](float sum, const Donation& donation) {
                                          return sum + (donation.isMoneyDonation() ? donation.getMoneyAmount() : 0);
                                      });
    
        std::cout << "Overall Summary of Distributions:" << std::endl;
        std::cout << "Total Recipients: " << totalRecipients << std::endl;
        std::cout << "Total Food Distributed: " << totalDistributedFood << " kg" << std::endl;
        std::cout << "Total Money Distributed: $" << std::fixed << std::setprecision(2) << totalMoney << std::endl;
        std::cout << "----------------------------------" << std::endl;
    }
    void displayDonorRankings() {
        int choice;
        std::cout << "\nRank Donors By:\n"
                 << "1. Donation Frequency\n"
                 << "2. Total Kg Donated\n"
                 << "3. Total Money Donated\n"
                 << "4. Back to Main Menu\n"
                 << "Choose ranking type: ";
        std::cin >> choice;
        std::cin.ignore();
    
        switch (choice) {
            case 1:
                rankByDonationFrequency();
                break;
            case 2:
                rankByTotalKgDonated();
                break;
            case 3:
                rankByTotalMoneyDonated();
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice!\n";
        }
    }

    void removeDonationsByDonorId(int donorId) {
        auto it = remove_if(donations.begin(), donations.end(),
            [donorId](const Donation& d) {
                return d.getDonorId() == donorId;
            });
        donations.erase(it, donations.end());
        saveDonations(); // Immediately update the file
    }

    void cleanupOrphanedDonations(const DonorManager& donorManager, const RecipientLinkedList& recipients) {
        donations.erase(
            std::remove_if(donations.begin(), donations.end(),
                [&donorManager, &recipients](const Donation& d) {
                    bool donorExists = false;
                    bool recipientExists = recipients.findRecipientById(d.getRecipientId()) != nullptr;
                    
                    for (const auto& donor : donorManager.getDonors()) {
                        if (donor.get_id() == d.getDonorId()) {
                            donorExists = true;
                            break;
                        }
                    }
                    
                    return !donorExists || !recipientExists;
                }),
            donations.end()
        );
        saveDonations();
    }
    
    void forceSaveAll() {
        saveDonations();  // Explicitly save donations
        // Add debug output to verify saving
        std::cout << "DEBUG: Saved " << donations.size() << " donations\n";
    }
};

#endif // REPORTING_H
