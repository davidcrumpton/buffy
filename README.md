# Buffy the flouride dispensor

Is a game where you try to reach the highest score possible before running out of flouride.

## License

See the [LICENSE](LICENSE) file in this repository for details.

## Inspiration

I never use `getopt_long(3)`, or long options, in anything I create. I decided to give it a try and laughed when I noticed the Buffy the Vampire Slayer references in the [`getopt_long` EXAMPLES section](https://man.openbsd.org/getopt_long#EXAMPLES) of OpenBSD and macOS.
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
