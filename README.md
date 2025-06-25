# Buffy the flouride dispensor

Is an get the highest score game. In it, you clean Dracula's teeth to reach the high score.

# Inspiration

I never use getopt_long(3), or long options, in anything I create.  I decided to give it a try and laughed when
I noticed the Buffy the Vampire Slayer references in the getopt_long EXAMPLE section of OpenBSD, and MacOS.

```
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