#include <iostream>
#include <cstdlib> // For system()
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

string usb = "No Drive Selected"; 

bool doesDiskExist(const string& disk) {
    return fs::exists(disk);  
}

void selectedFlash() {
    if (usb == "No Drive Selected") {
        cout << "Selected Flash Drive: \033[31m" + usb + "\033[0m\n\n";  // Red if no drive selected
    } else {
        cout << "Selected Flash Drive: \033[34m" + usb + "\033[0m\n\n";  // Blue otherwise
    }
}

void moreSelectedFlashInfo() {
    system(("DEVICE=\"" + usb + "\"; DEVNAME=$(basename \"$DEVICE\"); "
                "[ -b \"$DEVICE\" ] && echo \"Size: $(lsblk -no SIZE \"$DEVICE\" | head -n 1) | "
                "Model: $(lsblk -no MODEL \"$DEVICE\") | "
                "Removable: $(cat /sys/class/block/$DEVNAME/removable)\" || "
                "echo \"Device $DEVICE does not exist.\"").c_str());
}

void clearScreen() {
    cout << "\033[2J\033[H" << flush; // ANSI escape code to clear screen
}

bool GPTFDiskInstallCheck() {
    return system("command -v gptfdisk > /dev/null 2>&1") == 0;
}

void installGptfdisk() {
    clearScreen();
    char response;
    cout << "gptfdisk is not installed. Would you like to install it now? (y/n): ";
    cin >> response;

    if (response == 'y' || response == 'Y') {
        system("sudo pacman -S gptfdisk");
    } else {
        cerr << "gptfdisk is required for creating GPT partition schemes!";
        exit(1);
    }
}

void selectUSBDevice() {
    while (true) { 
        string buffer;
        clearScreen();
        string usbInitialSelection; 
        char selection;
        cout << "Please select the drive from the list (ensure that it is the flash drive)\n\n";
        system("sudo fdisk -l | grep -E '^Disk /dev/[a-z]+|^Disk /dev/nvme[0-9]+'");
        cout << "\nPlease type the identifier of the desired USB Flash Drive (ex. /dev/sda, /dev/sdb)\n";
        cin >> usbInitialSelection;
        clearScreen();

        if (!doesDiskExist(usbInitialSelection)) {
            clearScreen();
            cout << "This drive does not exist!\n\n";
            cout << "Type anything then enter to continue.\n\n";
            cin >> buffer;
            continue;
        }

        system(("DEVICE=\"" + usbInitialSelection + "\"; DEVNAME=$(basename \"$DEVICE\"); "
                "[ -b \"$DEVICE\" ] && echo \"Size: $(lsblk -no SIZE \"$DEVICE\" | head -n 1) | "
                "Model: $(lsblk -no MODEL \"$DEVICE\") | "
                "Removable: $(cat /sys/class/block/$DEVNAME/removable)\" || "
                "echo \"Device $DEVICE does not exist.\"").c_str());

        cout << "\nDoes this look right? (y/n)\n";
        cin >> selection;
        
        if (selection != 'y') {
            continue;
        }

        cout << "\nAre you sure? This drive may be WIPED later. (y/n)\n";
        cin >> selection;
        if (selection == 'y') {
            usb = usbInitialSelection;
            break;
        }
    }
}

void partToGPT() {
    string buffer;
    char finalChoice;
    clearScreen();
    cout << "Partition to GPT\n";
    selectedFlash();
    cout << "\n";
	cout << "Partition to GPT will DELETE the contents of your flash drive. \nPlease back up any important data before continuing.\n";
	cout << "Type accept to continue, anything else will bring you back.\n\n";
	cin >> buffer;
	if ((buffer == "accept" || buffer == "Accept") && usb != "No Drive Selected") {
        cout << flush;
        clearScreen();
        selectedFlash();
        moreSelectedFlashInfo();
        cout << "\n\033[31mFinal Warning. Everything will be wiped. Are you sure? (y/n)\033[0m\n\n";
        cin >> finalChoice;

        if (finalChoice == 'y') {
            string gptWipe = ("echo -e \"o\\ny\\nw\\ny\\n\" | sudo gdisk " + usb);
            system(gptWipe.c_str()); //conv to c string
            cout << flush;
            clearScreen();
            cout << "Success!\nType anything then press enter\n\n";
            cin >> buffer;
        }
    }
    else if (usb == "No Drive Selected") {
        cout << flush;
        clearScreen();
        selectedFlash();
        cout << "Failed!\n\n";
        cout << "Make sure you select a drive!\n";
        cout << "Type something and then enter to continue\n\n";
        cin >> buffer;
    }
    else {
        cout << "\nYou typed something wrong. Aborting.";
        cin >> buffer;
    }
}

void uefiBoot() {
    string isoFileLocation;
	cout << "UEFI Boot USB\n";
	selectedFlash();
    cout << "\nType the full file location of where the ISO is located\n\n";
}

void menu() {
    while (true) {
        int selection;
        clearScreen();
        cout << "FlashBurn\n\n";
        selectedFlash();
        cout << "1. Select a flash drive\n";
        cout << "2, Partition to GPT\n\n";
        cin >> selection;

        switch (selection) {
            case 1:
                selectUSBDevice();
                break;
            case 2:
                partToGPT();
                break;
            default:
                cout << "Wrong selection";
                break;
        }
    }
}

int main() {
    if (!GPTFDiskInstallCheck()) {
        installGptfdisk;
    }
    menu();
    return 0;
}
