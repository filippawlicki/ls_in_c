# LS in C

This is a small project that implements a basic version of the `ls` command in C. It supports several flags that modify its behavior:

- `-l`: Use a long listing format to display file details.
- `-R`: List subdirectories recursively.
- `-s`: Display the size of each file, in blocks.
- `-a`: Do not ignore entries starting with `.`.
- `-i`: Print the index number of each file.

## Prerequisites

Before you can run the script, you need to have the Tiny C Compiler (TCC) installed on your system. If you're using a Debian-based system like Ubuntu, you can install it using the following command:

```bash
sudo apt-get install tcc

## Usage

To compile and run the script in any directory, use the following commands:

```bash
tcc -run main.c -lRsai /path/to/directory
```

Replace `/path/to/directory` with the path to the directory you want to list. If no directory is specified, the script defaults to the current directory.


## Installation

To make the script runnable from anywhere, follow these steps:

1. Make sure your script is executable:

```bash
chmod +x main.c
```

2. Move the script to a directory in your PATH, such as `/usr/local/bin`:

```bash
sudo cp main.c /usr/local/bin/ls_c
```

Now, you can run your script from anywhere using the following command:

```bash
ls_c
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)
