/**
 * ============================================================================
 * Project: Library Management System (PF & Data Structures in C++)
 * Author:  Sohrab Soomro (FAST NUCES Peshawar)
 * Course:  Programming Fundamentals (PF) / Data Structures & Algorithms (DSA)
 *
 * Description:
 *   A complete console-based Library Management System built to manage books,
 *   registered university members, borrowing/return operations, waitlists,
 *   and persistent catalog storage.
 *
 * Core Data Structures & Programming Concepts Used:
 *   1. Binary Search Tree (BST) — O(log n) book lookup and sorted traversal
 *      by Book ID / ISBN.
 *   2. Singly Linked List — Dynamic management of registered library members
 *      and active book loans.
 *   3. FIFO Queue (`std::queue`) — Reservation waitlist when all copies of a
 *      popular book are currently checked out.
 *   4. LIFO Stack (`std::stack`) — Audit log of recent borrow/return actions.
 *   5. File Handling (`fstream`) — Saves and loads catalog records (`catalog.csv`).
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <stack>
#include <iomanip>

using namespace std;

// ----------------------------------------------------------------------------
// 1. BOOK NODE (Binary Search Tree Node + Waitlist Queue)
// ----------------------------------------------------------------------------
struct BookNode {
    int bookId;
    string title;
    string author;
    int totalCopies;
    int availableCopies;
    queue<int> waitlistMemberIds; // FIFO Queue for reservation waitlist

    BookNode* left;
    BookNode* right;

    BookNode(int id, string t, string a, int copies)
        : bookId(id), title(t), author(a), totalCopies(copies), availableCopies(copies),
          left(nullptr), right(nullptr) {}
};

// ----------------------------------------------------------------------------
// 2. MEMBER NODE (Singly Linked List for Registered Users)
// ----------------------------------------------------------------------------
struct MemberNode {
    int memberId;
    string fullName;
    string department;
    int borrowedBookId; // 0 if no book currently borrowed
    MemberNode* next;

    MemberNode(int id, string name, string dept)
        : memberId(id), fullName(name), department(dept), borrowedBookId(0), next(nullptr) {}
};

// ----------------------------------------------------------------------------
// 3. LIBRARY MANAGEMENT ENGINE (BST + Linked List + Stack + File I/O)
// ----------------------------------------------------------------------------
class LibrarySystem {
private:
    BookNode* rootBook;
    MemberNode* headMember;
    stack<string> auditHistory; // LIFO Stack storing recent transaction logs

    // Recursive BST Insertion
    BookNode* insertBookRec(BookNode* node, int id, const string& title, const string& author, int copies) {
        if (node == nullptr) {
            return new BookNode(id, title, author, copies);
        }
        if (id < node->bookId) {
            node->left = insertBookRec(node->left, id, title, author, copies);
        } else if (id > node->bookId) {
            node->right = insertBookRec(node->right, id, title, author, copies);
        } else {
            // Book ID already exists; increment copy count
            node->totalCopies += copies;
            node->availableCopies += copies;
        }
        return node;
    }

    // Recursive BST Search — O(log n) average time complexity
    BookNode* searchBookRec(BookNode* node, int id) const {
        if (node == nullptr || node->bookId == id) return node;
        if (id < node->bookId) return searchBookRec(node->left, id);
        return searchBookRec(node->right, id);
    }

    // In-Order Traversal prints books sorted by Book ID
    void inOrderDisplay(BookNode* node) const {
        if (!node) return;
        inOrderDisplay(node->left);
        cout << left << setw(8) << node->bookId
             << setw(34) << node->title.substr(0, 32)
             << setw(22) << node->author.substr(0, 20)
             << setw(12) << (to_string(node->availableCopies) + "/" + to_string(node->totalCopies))
             << setw(10) << node->waitlistMemberIds.size() << endl;
        inOrderDisplay(node->right);
    }

    void saveCatalogRec(BookNode* node, ofstream& out) const {
        if (!node) return;
        saveCatalogRec(node->left, out);
        out << node->bookId << "," << node->title << "," << node->author << ","
            << node->totalCopies << "," << node->availableCopies << "
";
        saveCatalogRec(node->right, out);
    }

public:
    LibrarySystem() : rootBook(nullptr), headMember(nullptr) {}

    void addBook(int id, const string& title, const string& author, int copies) {
        rootBook = insertBookRec(rootBook, id, title, author, copies);
        auditHistory.push("Added Book [" + to_string(id) + "] " + title + " (" + to_string(copies) + " copies)");
    }

    void registerMember(int memberId, const string& name, const string& dept) {
        MemberNode* newNode = new MemberNode(memberId, name, dept);
        newNode->next = headMember;
        headMember = newNode;
        auditHistory.push("Registered Member [" + to_string(memberId) + "] " + name + " (" + dept + ")");
    }

    MemberNode* findMember(int memberId) const {
        MemberNode* curr = headMember;
        while (curr != nullptr) {
            if (curr->memberId == memberId) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    void issueBook(int memberId, int bookId) {
        MemberNode* member = findMember(memberId);
        if (!member) {
            cout << "  [!] Member ID " << memberId << " not found." << endl;
            return;
        }
        if (member->borrowedBookId != 0) {
            cout << "  [!] Member already has an active borrowed book (ID " << member->borrowedBookId << ")." << endl;
            return;
        }
        BookNode* book = searchBookRec(rootBook, bookId);
        if (!book) {
            cout << "  [!] Book ID " << bookId << " not found in BST catalog." << endl;
            return;
        }

        if (book->availableCopies > 0) {
            book->availableCopies--;
            member->borrowedBookId = bookId;
            string log = "ISSUED: '" + book->title + "' to " + member->fullName;
            auditHistory.push(log);
            cout << "  [OK] " << log << endl;
        } else {
            book->waitlistMemberIds.push(memberId);
            string log = "WAITLISTED: " + member->fullName + " queued for '" + book->title + "'";
            auditHistory.push(log);
            cout << "  [QUEUE] No copies currently available. Added " << member->fullName
                 << " to reservation queue (Position #" << book->waitlistMemberIds.size() << ")." << endl;
        }
    }

    void returnBook(int memberId) {
        MemberNode* member = findMember(memberId);
        if (!member || member->borrowedBookId == 0) {
            cout << "  [!] No active borrowed book found for Member ID " << memberId << "." << endl;
            return;
        }
        int returnedId = member->borrowedBookId;
        BookNode* book = searchBookRec(rootBook, returnedId);
        member->borrowedBookId = 0;

        if (book) {
            string log = "RETURNED: '" + book->title + "' by " + member->fullName;
            auditHistory.push(log);
            cout << "  [OK] " << log << endl;

            // Check FIFO reservation queue to auto-assign to next waiting student
            if (!book->waitlistMemberIds.empty()) {
                int nextMemberId = book->waitlistMemberIds.front();
                book->waitlistMemberIds.pop();
                MemberNode* nextMember = findMember(nextMemberId);
                if (nextMember && nextMember->borrowedBookId == 0) {
                    nextMember->borrowedBookId = returnedId;
                    string autoLog = "AUTO-ISSUED from Queue: '" + book->title + "' -> " + nextMember->fullName;
                    auditHistory.push(autoLog);
                    cout << "  [QUEUE->ISSUED] " << autoLog << endl;
                    return;
                }
            }
            book->availableCopies++;
        }
    }

    void displayCatalog() const {
        cout << "
--------------------------------------------------------------------------------------
";
        cout << left << setw(8) << "ID" << setw(34) << "Book Title" << setw(22) << "Author"
             << setw(12) << "Avail/Tot" << setw(10) << "Waitlist" << endl;
        cout << "--------------------------------------------------------------------------------------
";
        inOrderDisplay(rootBook);
        cout << "--------------------------------------------------------------------------------------
";
    }

    void displayRecentTransactions(int count = 8) const {
        cout << "
=== Recent Audit Log (LIFO Stack) ===" << endl;
        stack<string> temp = auditHistory;
        int shown = 0;
        while (!temp.empty() && shown < count) {
            cout << "  " << (shown + 1) << ". " << temp.top() << endl;
            temp.pop();
            shown++;
        }
    }

    bool exportCatalogCSV(const string& filename) const {
        ofstream out(filename.c_str());
        if (!out.is_open()) return false;
        out << "BookID,Title,Author,TotalCopies,AvailableCopies
";
        saveCatalogRec(rootBook, out);
        out.close();
        return true;
    }
};

// ----------------------------------------------------------------------------
// 4. MAIN DRIVER (Pre-seeded demo + interactive menu)
// ----------------------------------------------------------------------------
int main() {
    LibrarySystem lib;

    // Seed core CS textbooks into the Binary Search Tree
    lib.addBook(104, "Introduction to Algorithms (CLRS)", "Cormen et al.", 3);
    lib.addBook(101, "The C++ Programming Language", "Bjarne Stroustrup", 2);
    lib.addBook(108, "Clean Code: Handbook of Agile Craft", "Robert C. Martin", 1);
    lib.addBook(102, "Data Structures & Algorithm Analysis", "Mark Allen Weiss", 2);
    lib.addBook(110, "Design Patterns: OOP Elements", "Gang of Four", 2);

    // Seed registered FAST NUCES members into the Singly Linked List
    lib.registerMember(2401, "Sohrab Soomro", "BS-CS");
    lib.registerMember(2402, "Afaq Ahmad", "BS-CS");
    lib.registerMember(2403, "Hamza Khan", "BS-SE");

    cout << "==========================================================" << endl;
    cout << "   FAST NUCES Library Management System (C++ PF / DSA)    " << endl;
    cout << "==========================================================" << endl;

    // Demonstrate borrowing, waitlist queueing, and auto-assignment on return
    lib.issueBook(2401, 108); // Sohrab borrows the single copy of Clean Code
    lib.issueBook(2402, 108); // Afaq requests Clean Code -> placed in FIFO Waitlist Queue
    lib.returnBook(2401);     // Sohrab returns Clean Code -> automatically issued to Afaq!

    lib.displayCatalog();
    lib.displayRecentTransactions();
    lib.exportCatalogCSV("catalog_export.csv");

    return 0;
}
