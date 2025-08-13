# BROP 

**Author : Byte Reaper**

This project demonstrates a basic **BROP (Blind Return Oriented Programming)** technique to differentiate between correct and incorrect return addresses.  
It works by jumping to a range of memory addresses and using `SIGSEGV` / `SIGILL` signal handlers to skip invalid addresses safely.

## How It Works
1. Define a range of addresses to scan.
2. Overwrite a return address in a simulated stack buffer.
3. Jump to each candidate address.
4. If a signal is caught (segmentation fault ), skip that address.
5. If the process continues normally, the address is considered a valid gadget.

## Features
- **Signal Handling**: Captures and bypasses crashes caused by bad addresses.
- **Stack Offset Simulation**: Mimics overwriting a return address in the stack.
- **Address Scanning**: Iterates through executable memory ranges.
- **Process Verification**: Checks if the target process continues running.

## Requirements
- GCC or Clang compiler.
- Linux x86_64 environment.
- Basic knowledge of exploitation and ROP.

## Usage
```
    gcc brop.c -o brop
    ./brop
```
## Example Output : 
```
[+] Pointer a() : 0x4011a0
[+] Pointer b() : 0x4011b0
[+] Found address in stack (0x4011a0)
[+] Found address in stack (0x4011b0)
[+] I started trying to find the address using BROP...
[-] Segfault/Illegal instruction caught, skipping address...
[+] Word found in ps aux PID !
```
## License :


MIT License
