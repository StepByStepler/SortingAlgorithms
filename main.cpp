#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

#ifdef __unix__
#include <sys/sysinfo.h>
unsigned long long getFreeMemory() {
    FILE *memInfo = fopen("/proc/meminfo", "r");
    if (memInfo == nullptr)
        return 0;

    char line[256];
    int free = -1, cache = -1, buffers = -1, ram;

    while (fgets(line, sizeof(line), memInfo)) {
        if (free == -1 && sscanf(line, "MemFree: %d kB", &ram) == 1) {
            free = ram;
        } else if (cache == -1 && sscanf(line, "Cached: %d kB", &ram) == 1) {
            cache = ram;
        } else if (buffers == -1 && sscanf(line, "Buffers: %d kB", &ram) == 1) {
            buffers = ram;
        }

        if (free != -1 && cache != -1 && buffers != -1) break;
    }

    fclose(memInfo);
    return (free + cache + buffers) * 1024;
}

#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#include <windows.h>

unsigned long long getFreeMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys;
}
#endif

bool notEnoughMemory(int newArraySize) {
    return getFreeMemory() < newArraySize * sizeof(int);
}

unsigned long getMillisNow() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void printSortTime(int *arr, int size, void (*sort)(int *, int)) {
    unsigned long startMillis = getMillisNow();
    sort(arr, size);
    unsigned long endMillis = getMillisNow();

    cout << "Array was sorted in " << endMillis - startMillis << " ms:" << endl;
    for (int i = 0; i < size; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

void bubbleSort(int *arr, int size) {
    for (int i = 0; i < size - 1; ++i) {
        int jLimit = size - i - 1;
        for (int j = 0; j < jLimit; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

int partition(int *arr, int low, int high) {
    int pivot = arr[high];
    int sIndex = low - 1;

    for (int i = low; i < high; ++i) {
        if (arr[i] <= pivot) {
            sIndex++;
            swap(arr[sIndex], arr[i]);
        }
    }
    swap(arr[sIndex + 1], arr[high]);
    return sIndex + 1;
}

void quickSort(int *arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);

        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

void quickSort(int *arr, int size) {
    quickSort(arr, 0, size - 1);
}

int main() {
    int size, dSize;

    cout << "Input array size: " << endl;
    cin >> size;
    if (size <= 0) return 1;
    if (notEnoughMemory(size)) return 2;

    int *arr = new int[size];

    srand(time(nullptr));
    for (int i = 0; i < size; ++i) {
        arr[i] = (rand() - rand()) % 200;
    }

    printSortTime(arr, size, bubbleSort);

    cout << "Input K: " << endl;
    cin >> dSize;
    if (dSize < 0) return 3;
    size += dSize;
    if (notEnoughMemory(size)) return 4;

    arr = (int *) realloc(arr, size * sizeof(int));
    for (int i = size - dSize; i < size; ++i) {
        arr[i] = (rand() - rand()) % 200;
    }

    printSortTime(arr, size, quickSort);
    delete[] arr;

    return 0;
}