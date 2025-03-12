#include <iostream>
#include <cstdlib> // For system()
#include <algorithm> // For shuffle
#include <random> // For random device

using namespace std;

void clearScreen() {
    cout << "\033[2J\033[H"; // ANSI escape code to clear screen
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
        cerr << "gptfdisk is required for ";
        exit(1);
    }
}

void selectUSBDevice() {
    while (true) { 
        clearScreen();
        string usb = "No USB Drive Selected";
        string usbInitialSelection; 
        char selection;
        cout << "Please select a USB Device from the list (ensure that it is the flash drive)\n\n";
        system("sudo fdisk -l | grep -E '^Disk /dev/[a-z]+|^Disk /dev/nvme[0-9]+'");
        cout << "\nPlease type the identifier of the desired USB Flash Drive (ex. /dev/sda, /dev/sdb)\n";
        cin >> usbInitialSelection;

        clearScreen();
        system(("DEVICE=\"" + usbInitialSelection + "\"; DEVNAME=$(basename \"$DEVICE\"); "
                "[ -b \"$DEVICE\" ] && echo \"Size: $(lsblk -no SIZE \"$DEVICE\" | head -n 1) | "
                "Model: $(lsblk -no MODEL \"$DEVICE\") | "
                "Removable: $(cat /sys/class/block/$DEVNAME/removable)\" || "
                "echo \"Device $DEVICE does not exist.\"").c_str());

        cout << "\nDoes this look right? (y/n)";
        cin >> selection;
        
        if (selection == 'y') {
            usb = usbInitialSelection;
            break;
        }
        else {
            cout << "Redo";
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

int main () {
    selectUSBDevice(); 
}