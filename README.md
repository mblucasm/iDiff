# iDiff - Instagram User and List Difference Finder

## Table of Contents

1. [Description](#description)
2. [Features](#features)
3. [Requirements](#requirements)
4. [Installation](#installation)
5. [Usage](#usage)
6. [License](#license)
7. [Contact](#contact)
8. [For Beginners](#for-beginners)
9. [Examples](#examples)

## Description

**iDiff** is a lightweight, yet powerful tool designed to analyze Instagram’s downloaded data and determine who you follow but doesn’t follow you back. If you download your Instagram data and run iDiff with `--instagram-folder=<path_to_instagram_data>`, it will automatically extract and compare your followers and following lists. This makes it an excellent tool for tracking unfollowers or simply understanding your connections better.

Additionally, iDiff can be used to compare any two lists of usernames or general items, identifying which elements appear in the second list but not in the first.

## Features

- Identify people you follow but who don’t follow you back on Instagram.
- Extract followers/following lists from **Instagram's downloaded data**.
- Compare any two lists of usernames or general items.
- Parse \<div> elements extracted from inspecting followers/following in the browser.
- Display missing users/items in a clean, structured format.

## Requirements

- A **C compiler** (such as GCC, Clang, etc.).
  *(Tested with ****GCC 13.2.0****. Older versions (e.g., ****6.X****) may not work properly.)*
- `stb_ds.h` (already included in the project, no need to install separately).

## Installation

To install and compile iDiff, follow these steps:

```sh
git clone https://github.com/mblucasm/iDiff.git
cd iDiff
gcc -O2 -o idiff src/main.c src/include/slice.c src/include/error.c
```

If you're new to programming or using a terminal, **don't worry!** I have a dedicated section: [For Beginners](#for-beginners). There, you'll find a detailed step-by-step guide explaining everything, from downloading the project to running it, in a way that's easy to follow—even if you've never used a terminal before.

## Usage

To use iDiff, simply run the following command:

```sh
./idiff list1.txt list2.txt
```

Where `list1.txt` and `list2.txt` contain the lists you want to compare. The program will output the elements that appear in the second list but not in the first.

If you run `./idiff` without arguments, it will display the **usage guide** explaining how to use the program properly.

Additionally, you can specify an **Instagram data folder** directly with:

```sh
./idiff --instagram-folder=<path_to_instagram_data>
```

This allows the program to automatically locate and extract **followers/following lists** from the downloaded Instagram data and compare them, showing you who you follow but doesn’t follow you back.

## License

This project is licensed under the **Do What The Fuck You Want To Public License (WTFPL)**. See `LICENSE` for details.

## Contact

For inquiries, suggestions, or questions, contact:

- **Lucas Martín Borrero** - [mblucasm@gmail.com](mailto\:mblucasm@gmail.com)

## For Beginners

If you've never used a terminal or compiled a program before, this section will guide you step by step.

### 1. **Opening the Terminal**

To run commands, you'll need to open the terminal:

- **Windows:** Open **PowerShell** or **CMD (Command Prompt)**.
- **Mac:** Open **Terminal** from your applications or search bar.

### 2. **Downloading the Project**

There are two ways to get iDiff:

- **Option 1: Using Git** (Recommended if you have Git installed)

  ```sh
  git clone https://github.com/mblucasm/iDiff.git
  ```

  This will create a folder named `iDiff` in your current directory.

- **Option 2: Downloading as a ZIP file**

  - Visit the [GitHub repository](https://github.com/mblucasm/iDiff).
  - Click the green **"Code"** button and select **Download ZIP**.
  - Extract the ZIP file into a folder.

### 3. **Checking if You Already Have a Compiler Installed**

To check if you already have a compiler installed, open the terminal and type:

```sh
<compiler> --version
```

Replace `<compiler>` with `gcc`, `clang`, or another compiler name. If a version number appears, you already have a compiler installed. If not, follow the next step to install one.

### 4. **Installing a Compiler**

A C compiler is required to compile the program. If you don't have one, follow these steps:

- **Windows:**

  - Download **w64devkit** from [this link](https://github.com/skeeto/w64devkit) → **Go to Releases** and download the `.exe` that suits your device (**x64** for 64-bit systems, **x86** for 32-bit systems).
  - Run the `.exe` file to extract w64devkit.
  - w64devkit provides **gcc** and **cc** as compilers.
  - To ensure the compiler is available in the terminal, [**add it to your system's PATH**](#41-adding-the-compiler-to-your-systems-path).

- **Mac:**

  - If `clang --version` didn't show a version, install Clang by running:
    ```sh
    xcode-select --install
    ```

#### 4.1 **Adding the Compiler to Your System's PATH**
The **PATH** is an environment variable that tells your operating system where to look for executable files. If a program is in a directory listed in the PATH, you can run it from any terminal window without specifying the full path.

To add a program (e.g., `gcc` from w64devkit) to your PATH:

- **Windows:**
  1. Open **System Properties** (Win + R, type `sysdm.cpl`, press Enter).
  2. Go to the **Advanced** tab and click **Environment Variables**.
  3. Under **System Variables**, find `Path` and click **Edit**.
  4. Click **New** and add the path to the w64devkit `bin` folder.
  5. Click **OK** and restart the terminal.

### 5. **Navigating to the Project Folder**

Once you have the project downloaded and the compiler installed:

1. Open the terminal.
2. Use the `cd` command to move into the project directory:
   ```sh
   cd path/to/iDiff
   ```

### 6. **Compiling and Running the Program**

Now, compile the code into an executable program.

For **GCC**:

```sh
gcc -O2 -o idiff src/main.c src/include/slice.c src/include/error.c
```

For **Clang (or other compilers)**, replace `gcc` with the name of your compiler:

```sh
clang -O2 -o idiff src/main.c src/include/slice.c src/include/error.c
```

This will generate an executable file called `idiff`.

#### Running iDiff

- **Compare two lists:**

  ```sh
  ./idiff list1.txt list2.txt
  ```

  This will show the elements that are in `list2.txt` but not in `list1.txt`. (The name of the files does not strictly need to be list1.txt and list2.txt, this is just to ilustrate)

- **Get help and usage information:**

  ```sh
  ./idiff
  ```

- **Compare Instagram followers/following from downloaded data (easy way):**

  ```sh
  ./idiff --instagram-folder=<path_to_instagram_data>
  ```

  This will automatically locate and compare your `followers.html` and `following.html` files.

That's it! You've successfully downloaded, compiled, and run the project. If you encounter any issues, there are many online tutorials and videos that explain how to set up and use compilers and terminals, so don't hesitate to check them out for more help.

**Remember:** The goal of this project is to be as simple and accessible as possible. Enjoy using it, and feel free to reach out if you need help!

## Examples

@TODO