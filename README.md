# 🦷 Buffy the Fluoride Dispenser

Buffy is a terminal-based game where you play as a dental hygienist tasked with cleaning a patient's fangs using one of six randomly chosen tools. Your goal is to clean all teeth before your fluoride supply runs out.


## 🎮 Gameplay Overview

- Each turn, Buffy selects a random tool to clean a tooth.
- You must choose which tooth to clean based on its condition and the tool's effectiveness.
- The game ends when:
  - All teeth are cleaned successfully.
  - You run out of fluoride.

## 🧪 Command-Line Options

Buffy supports several long options via `getopt_long(3)`:

```c
static struct option longopts[] = {
  { "buffy",     no_argument,       NULL, 'b' },
  { "fluoride",  required_argument, NULL, 'f' },
  { "daggerset", no_argument,       &daggerset, 1 },
  { NULL,        0,                 NULL, 0 }
};
```

### Available Flags

| Option        | Description                                                                 |
|---------------|-----------------------------------------------------------------------------|
| `-b`, `--buffy`     | Activates non-Buffy mode where you are addressed by your login name.     |
| `-f <file>`, `--fluoride <file>` | Loads fluoride configuration or game data from the specified file. |
| `--daggerset` | The gamne will randomly choose one of three daggers made from different materials. |


## 📜 License

This project is licensed under the BSD Zero Clause License. See the [LICENSE](LICENSE) file for details.

# Inspiration

I decided to use `getopt_long(3)`, or long options, in my next creation. I decided to read the manual and laughed when I noticed the Buffy the Vampire Slayer references in the [`getopt_long` EXAMPLES section](https://man.openbsd.org/getopt_long#EXAMPLES) of OpenBSD and macOS.
The game will always have at a minimum the options specified in the description below.

```c
     /* options descriptor */
     static struct option longopts[] = {
             { "buffy",      no_argument,            NULL,           'b' },
             { "fluoride",   required_argument,      NULL,           'f' },
             { "daggerset",  no_argument,            &daggerset,     1 },
             { NULL,         0,                      NULL,           0 }
     };

     bflag = 0;
     while ((ch = getopt_long(argc, argv, "bf:", longopts, NULL)) != -1) {
             switch (ch) {
             case 'b':
                     bflag = 1;
                     break;
             case 'f':
                     if ((fd = open(optarg, O_RDONLY, 0)) == -1)
                             err(1, "unable to open %s", optarg);
                     break;
             case 0:
                     if (daggerset) {
                             fprintf(stderr,"Buffy will use her dagger to "
                                 "apply fluoride to dracula's teeth\n");
```