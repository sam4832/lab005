#include <iostream>
#include <cstring>
#include <iomanip>
#include <cstdio>  // for sscanf

using namespace std;

struct Trip {
    int id;
    char* fio;
    char* date_out;
    char* date_back;
    char* destination;
    char* purpose;
    double daily;
    Trip* next;
};

int counter = 1;

// Проверка корректности даты (DD.MM.YYYY, год 2020-2030, день 1-31, месяц 1-12)
bool isValidDate(const char* date) {
    int day, month, year;
    if (sscanf(date, "%d.%d.%d", &day, &month, &year) != 3) return false;
    if (year < 2020 || year > 2030) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;  // Упрощённо, без учёта месяцев с 28-30 днями
    return true;
}

// Упрощённый расчёт дней
int getDays(const char* d1, const char* d2) {
    int day1, mon1, year1, day2, mon2, year2;
    sscanf(d1, "%d.%d.%d", &day1, &mon1, &year1);
    sscanf(d2, "%d.%d.%d", &day2, &mon2, &year2);
    int totalDays1 = year1 * 365 + mon1 * 30 + day1;
    int totalDays2 = year2 * 365 + mon2 * 30 + day2;
    return totalDays2 - totalDays1 + 1;
}

// Создание новой записи с правилами и валидациями
Trip* createTrip() {
    Trip* t = new Trip;
    t->id = counter++;
    char buf[256];

    cout << "FIO (5-60 symbols): ";
    cin.getline(buf, 256);
    if (strlen(buf) < 5 || strlen(buf) > 60) {
        cout << "FIO length error!\n";
        delete t;
        return nullptr;
    }
    t->fio = new char[strlen(buf) + 1];
    strcpy(t->fio, buf);

    do {
        cout << "Date out (DD.MM.YYYY): ";
        cin.getline(buf, 256);
    } while (!isValidDate(buf));
    t->date_out = new char[strlen(buf) + 1];
    strcpy(t->date_out, buf);

    do {
        cout << "Date back (DD.MM.YYYY): ";
        cin.getline(buf, 256);
    } while (!isValidDate(buf));
    t->date_back = new char[strlen(buf) + 1];
    strcpy(t->date_back, buf);

    cout << "Destination: ";
    cin.getline(buf, 256);
    t->destination = new char[strlen(buf) + 1];
    strcpy(t->destination, buf);

    cout << "Purpose: ";
    cin.getline(buf, 256);
    t->purpose = new char[strlen(buf) + 1];
    strcpy(t->purpose, buf);

    do {
        cout << "Daily allowance (0.0 - 100000.0): ";
        cin >> t->daily;
        cin.ignore();
    } while (t->daily < 0.0 || t->daily > 100000.0);

    int days = getDays(t->date_out, t->date_back);
    if (days > 45) {
        cout << "Error: trip more than 45 days - forbidden!\n";
        delete[] t->fio;
        delete[] t->date_out;
        delete[] t->date_back;
        delete[] t->destination;
        delete[] t->purpose;
        delete t;
        return nullptr;
    }
    if (days > 15) {
        t->daily *= 1.10;  // +10%
        cout << "Duration >15 days -> daily increased by 10%\n";
    }

    t->next = nullptr;
    return t;
}

// Добавление в конец
Trip* addToEnd(Trip* head) {
    Trip* t = createTrip();
    if (!t) return head;
    if (!head) return t;
    Trip* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = t;
    return head;
}

// Добавление в начало (срочная)
Trip* addToBegin(Trip* head) {
    Trip* t = createTrip();
    if (!t) return head;
    t->next = head;
    return t;
}

// Вывод всех с днями и суммой
void printAll(Trip* head) {
    if (!head) {
        cout << "No records\n\n";
        return;
    }

    double totalSum = 0.0;
    Trip* cur = head;
    int i = 1;
    cout << "\n" << string(150, '-') << "\n";
    cout << left << setw(5) << "ID" << setw(25) << "FIO" << setw(15) << "Date out" << setw(15) << "Date back"
         << setw(25) << "Destination" << setw(12) << "Daily" << setw(10) << "Days" << setw(15) << "Sum" << "\n";
    cout << string(150, '-') << "\n";

    while (cur) {
        int days = getDays(cur->date_out, cur->date_back);
        double sum = days * cur->daily;
        totalSum += sum;

        cout << left << setw(5) << cur->id << setw(25) << cur->fio << setw(15) << cur->date_out << setw(15) << cur->date_back
             << setw(25) << cur->destination << setw(12) << fixed << setprecision(2) << cur->daily
             << setw(10) << days << setw(15) << sum << "\n";
        cur = cur->next;
    }
    cout << string(150, '-') << "\n";
    cout << "TOTAL DAILY SUM: " << totalSum << " rub.\n\n";
}

// Удаление по FIO с подсчётом сравнений
Trip* deleteByFIO(Trip* head) {
    char buffer[256];
    cout << "Enter FIO: ";
    cin.getline(buffer, 256);
    Trip* curr = head;
    Trip* prev = nullptr;
    int comparisons = 0;
    while (curr) {
        comparisons++;
        if (strcmp(curr->fio, buffer) == 0) {
            cout << "Deleted record " << curr->id << endl;
            if (prev) {
                prev->next = curr->next;
            } else {
                head = curr->next;
            }
            delete[] curr->fio;
            delete[] curr->date_out;
            delete[] curr->date_back;
            delete[] curr->destination;
            delete[] curr->purpose;
            delete curr;
            cout << "Comparisons: " << comparisons << endl;
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    cout << "Not found" << endl;
    cout << "Comparisons: " << comparisons << endl;
    return head;
}

// Поиск по месту назначения с подсчётом сравнений
void searchByDestination(Trip* head) {
    char dest[256];
    cout << "Enter destination for search: ";
    cin.getline(dest, 256);

    cout << "\nSearch results for \"" << dest << "\":\n";
    cout << string(140, '-') << "\n";
    cout << left << setw(25) << "FIO" << setw(15) << "Date out" << setw(15) << "Date back" << setw(40) << "Purpose" << setw(12) << "Daily" << "\n";
    cout << string(140, '-') << "\n";

    Trip* cur = head;
    bool found = false;
    int comparisons = 0;
    while (cur) {
        comparisons++;
        if (strcmp(cur->destination, dest) == 0) {
            cout << left << setw(25) << cur->fio << setw(15) << cur->date_out << setw(15) << cur->date_back
                 << setw(40) << cur->purpose << setw(12) << fixed << setprecision(2) << cur->daily << "\n";
            found = true;
        }
        cur = cur->next;
    }
    if (!found) cout << "No records found\n";
    cout << string(140, '-') << "\n";
    cout << "Comparisons: " << comparisons << "\n\n";
}

// Сортировка по месту назначения
void sortByDestination(Trip* head) {
    if (!head || !head->next) return;
    bool swapped;
    do {
        swapped = false;
        Trip* cur = head;
        while (cur->next) {
            if (strcmp(cur->destination, cur->next->destination) > 0) {
                swap(cur->fio, cur->next->fio);
                swap(cur->date_out, cur->next->date_out);
                swap(cur->date_back, cur->next->date_back);
                swap(cur->destination, cur->next->destination);
                swap(cur->purpose, cur->next->purpose);
                swap(cur->daily, cur->next->daily);
                swap(cur->id, cur->next->id);
                swapped = true;
            }
            cur = cur->next;
        }
    } while (swapped);
}

// Инвертирование списка
Trip* reverseList(Trip* head) {
    Trip *prev = nullptr, *cur = head, *next;
    while (cur) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    return prev;
}

// Статистика по местам назначения
void showStatistics(Trip* head) {
    if (!head) {
        cout << "List is empty\n";
        return;
    }

    struct DestCount {
        char* dest;
        int count;
        DestCount* next;
    };

    DestCount* stats = nullptr;

    Trip* cur = head;
    while (cur) {
        DestCount* node = stats;
        bool found = false;
        while (node) {
            if (strcmp(node->dest, cur->destination) == 0) {
                node->count++;
                found = true;
                break;
            }
            node = node->next;
        }
        if (!found) {
            DestCount* newNode = new DestCount;
            newNode->dest = new char[strlen(cur->destination) + 1];
            strcpy(newNode->dest, cur->destination);
            newNode->count = 1;
            newNode->next = stats;
            stats = newNode;
        }
        cur = cur->next;
    }

    cout << "\nStatistics by destinations:\n";
    DestCount* s = stats;
    while (s) {
        cout << s->dest << " — " << s->count << " trip";
        if (s->count % 10 == 1 && s->count % 100 != 11) cout << "\n";
        else if (s->count % 10 >= 2 && s->count % 10 <= 4 && (s->count % 100 < 10 || s->count % 100 >= 20)) cout << "s\n";
        else cout << "s\n";
        s = s->next;
    }

    // очистка статистики
    while (stats) {
        DestCount* temp = stats;
        stats = stats->next;
        delete[] temp->dest;
        delete temp;
    }
}

// Освобождение памяти
void freeList(Trip* head) {
    Trip* temp;
    while (head) {
        temp = head;
        head = head->next;
        delete[] temp->fio;
        delete[] temp->date_out;
        delete[] temp->date_back;
        delete[] temp->destination;
        delete[] temp->purpose;
        delete temp;
    }
}

int main() {
    // setlocale(LC_ALL, "Russian");  // Uncomment if needed
    Trip* head = nullptr;
    int choice;
    do {
        cout << "Menu:\n";
        cout << "1. Add trip (to end)\n";
        cout << "2. Add urgent trip (to begin)\n";
        cout << "3. View all\n";
        cout << "4. Delete by FIO\n";
        cout << "5. Search by destination\n";
        cout << "6. Sort by destination\n";
        cout << "7. Reverse list\n";
        cout << "8. Show statistics\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                head = addToEnd(head);
                break;
            case 2:
                head = addToBegin(head);
                break;
            case 3:
                printAll(head);
                break;
            case 4:
                head = deleteByFIO(head);
                break;
            case 5:
                searchByDestination(head);
                break;
            case 6:
                sortByDestination(head);
                cout << "Sorted by destination.\n";
                break;
            case 7:
                head = reverseList(head);
                cout << "List reversed.\n";
                break;
            case 8:
                showStatistics(head);
                break;
            case 0:
                freeList(head);
                cout << "Memory freed. Bye!\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
        cout << "\n";
    } while (choice != 0);
    return 0;
}
