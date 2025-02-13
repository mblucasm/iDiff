# iDiff - Instagram User and List Difference Finder

## Table of contents
1. [Description](#description)
2. [Features](#features)
3. [Requirements](#requirements)
4. [Installation](#installation)
5. [Usage](#usage)
6. [License](#license)
7. [Contact](#contact)
8. [Examples](#examples)

## Description
iDiff is a simple tool for comparing two lists of usernames or general items. It identifies elements that appear in the second list but are missing from the first. Additionally, it can parse Instagram `<div>` elements to extract usernames automatically, making it especially useful for comparing Instagram followers.

## Features
- Compare two lists of usernames or general items.
- Extract usernames from Instagram HTML snippets.
- Display missing users/items in a clean format.

## Requirements
- A C compiler (GCC, Clang, etc.).
- `stb_ds.h` (included in the project).

## Installation
Clone the repository and compile the program:
```sh
git clone https://github.com/mblucasm/iDiff.git
cd iDiff
gcc -O3 -o idiff src/main.c src/include/slice.c src/include/error.c
```

## Usage
Run the program and provide two lists of items:
```sh
./idiff list1.txt list2.txt
```

## License
This project is licensed under the "Do What The Fuck You Want To" Public License. See LICENSE for details.

## Contact
For inquiries, suggestions, or questions, contact:
- **Lucas Martín Borrero** - [mblucasm@gmail.com](mailto:mblucasm@gmail.com)

## For Beginners
If you're not familiar with programming or using a terminal, don't worry! Here’s a detailed guide to help you download, install, compile, and run the project, even if you’ve never used a terminal before.

### 1. **Open the Terminal**
To interact with your system and run commands, you'll need to open the terminal.

- **Windows:** Open **PowerShell** or **CMD** (Command Prompt) to run commands.
- **Mac:** Open **Terminal** from your applications or search bar.

### 2. **Download the Project**
There are two ways you can download the project:

- **Option 1: Using Git**
  
    If you have `Git` installed, you can clone the project directly into your system by running in the terminal:
    ```sh
    git clone https://github.com/mblucasm/iDiff.git
    ```
    This will create a copy of the project folder on your computer.

- **Option 2: Downloading as ZIP**

    If you don't have Git, you can download the project as a **ZIP** file.
    - Go to the [project GitHub page](https://github.com/mblucasm/iDiff).
    - Click the green button that says **"Code"** and then select **Download ZIP**.
    - Once downloaded, extract the ZIP file to a folder on your computer.

### 3. **Install the Compiler (if you don't already have one)**
You'll need a compiler to turn the source code into an executable program.

- **For Windows:** I recommend **w64devkit**. It's easy to install and doesn't give problems, great for beginners. (Any other C compiler works)
    1. Download **w64devkit** from [here](https://github.com/skeeto/w64devkit).
    2. Run the installer, and during installation, make sure to **add w64devkit to your system's PATH**. This allows you to run commands like `gcc` from anywhere in the terminal. (If you don't do this during installation, you can also do it afterwards)
    3. To verify the installation, open the terminal and type:
    ```sh
    gcc --version
    ```
    4. If you see the version of GCC, the installation was successful.

    **Note:** gcc is the name of the compiler w64devkit provides, if you install another C compiler e.g. `clang` the command should be: clang --version

- **For Mac:** **Clang** is usually pre-installed. If it’s not, you can install it by running:
    ```sh
    xcode-select --install
    ```
    Just as in Windows any other C compiler works.

### 4. **Navigate to the project folder**
Once the project is downloaded and the compiler is installed, you'll need to navigate to the project folder in the terminal.
1. Open the terminal
2. Use the `cd` command to go to the directory where you downloaded or cloned the project
```sh
cd path/to/iDiff
```

### 5. **Compile the program**
Now that you're in the project folder, you can compile the code into an executable program.
- For GCC (Windows or Mac):
    - In The terminal, type:
    ```sh
    gcc -O3 -o idiff src/main.c src/include/slice.c src/include/error.c
    ```
    This command will compile the code and create an executable file called idiff.
- With other compilers just do the same as stated above but replace **gcc** with the name of your compiler.

### 5. **Run the program**
Once the program is compiled, you can run it with the following command:
```sh
./idiff list1.txt list2.txt
```
Where list1.txt and list2.txt are .txt files containing the lists to compare. The name of the files does not strictly need to be list1.txt and list2.txt, this is just to ilustrate.

**That's it!** You've successfully downloaded, compiled, and run the project. If you encounter any issues, there are many online tutorials and videos that explain how to set up and use compilers and terminals, so don't hesitate to check them out for more help.

**Remember: The goal of this project is to be as simple and accessible as possible. Enjoy using it, and feel free to reach out if you need help!**

## Examples
@TODO