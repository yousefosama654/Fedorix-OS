# Fedorix-OS

## 📝 Table of Contents

- [About](#about)
- [System Description](#sys)
- [Get started](#get-started)
  - [Prerequisite](#req)
  - [Installation](#Install)
  - [Running](#running)
- [Technology](#tech)
- [Demo Outputs](#outputs)
- [Contributors](#Contributors)

## 📙 About <a name = "about"></a>

- A CPU scheduler determines an order for the execution of its scheduled processes; it decides which process will run according to a certain data structure that keeps track of the processes in the system and their status.
- A process, upon creation, has one of the three states: Running, Ready, Blocked (doing I/O, using other resources than CPU or waiting on unavailable resource).

## 📃 System Description <a name = "sys"></a>

- Consider a Computer with 1-CPU and infinite memory . Our scheduler implemented the complementary components as sketched in the following diagrams.

📌 NOTE:

> We are supporting the following Scheduling algorithms:

1. HPF (Highest Priority First)
2. STRN (Shortest Time Remaining Next)
3. RR (Round Robin)

<div align='center'><img src="Screenshots/sys1.png"></div>

## 🏁 Getting Started <a name = "get-started"></a>

> This is an list of needed instructions to set up your project locally, to get a local copy up and running follow these
> instructions.

### Prerequisite <a name = "req"></a>

1. Any unix based system
2. GCC compiler installed

### Installation <a name = "Install"></a>

1. **_Clone the repository_**

```sh
$ git clone https://github.com/yousefosama654/Fedorix-OS.git
```

2. **_Navigate to Phase2 Folder_**

```sh
$ cd Phase1
```

### Running <a name = "running"></a>

**_Running program_**

1. **_First remove the old files_**

```sh
$ make clean
```

2. **_Build all the needed files_**

```sh
$ make build
```

3. **_Run the test generator to generate a files of random procceses_**

```sh
$ ./bin/test_generator.out
```

4. **_Now run the scheduler_**

```sh
$ make run
```

## 💻 Built Using <a name = "tech"></a>

- **C Programming Language**


