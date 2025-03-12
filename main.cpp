#include <iostream>
#include <cstdlib> // For system()
#include <string>

using namespace std;

string usb = "No Drive Selected"; 

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
        clearScreen();
        string usbInitialSelection; 
        char selection;
        cout << "Please select the drive from the list (ensure that it is the flash drive)\n\n";
        system("sudo fdisk -l | grep -E '^Disk /dev/[a-z]+|^Disk /dev/nvme[0-9]+'");
        cout << "\nPlease type the identifier of the desired USB Flash Drive (ex. /dev/sda, /dev/sdb)\n";
        cin >> usbInitialSelection;
        clearScreen();

        system(("DEVICE=\"" + usbInitialSelection + "\"; DEVNAME=$(basename \"$DEVICE\"); "
                "[ -b \"$DEVICE\" ] && echo \"Size: $(lsblk -no SIZE \"$DEVICE\" | head -n 1) | "
                "Model: $(lsblk -no MODEL \"$DEVICE\") | "
                "Removable: $(cat /sys/class/block/$DEVNAME/removable)\" || "
                "echo \"Device $DEVICE does not exist.\"").c_str());

        cout << "\nDoes this look right? (y/n)\n";
        cin >> selection;
        
        if (selection == 'y') {
            cout << "\nAre you sure? This drive may be WIPED later. (y/n)\n";
            cin >> selection;
            if (selection == 'y') {
                usb = usbInitialSelection;
                break;
            }
            else {

            }
        }
        else {

        }

    }
}

void partToGPT() {
	string buffer;
	clearScreen();
	cout << "Partition to GPT will DELETE the contents of your flash drive. Please back up any important data before continuing\n.";
	cout << "Type accept to continue, or press Control + C";
	cin >> buffer;
	if (buffer == "accept") {
	//add gpt format command, but firstly import the desired USB device.
    }
}

void menu() {
    while (true) {
        int selection;
        clearScreen();
        cout << "FlashBurn\n\n";
        if (usb == "No Drive Selected") {
            cout << "Selected Flash Drive: \033[31m" + usb + "\033[0m\n\n";  // Red if no drive selected
        } else {
            cout << "Selected Flash Drive: \033[34m" + usb + "\033[0m\n\n";  // Blue otherwise
        }

        cout << "1. Select a flash drive\n";
        cin >> selection;

        switch (selection) {
            case 1:
                selectUSBDevice();
                break;
            default:
                cout << "Wrong selection";
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