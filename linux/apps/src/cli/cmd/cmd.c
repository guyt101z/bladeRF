#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cmd.h"

//mingw doesn't have strtok_r
#ifdef _WIN32 __MINGW32__
/* 
 * public domain strtok_r() by Charlie Gordon
 *
 *   from comp.lang.c  9/14/2007
 *
 *      http://groups.google.com/group/comp.lang.c/msg/2ab1ecbb86646684
 *
 *     (Declaration that it's public domain):
 *      http://groups.google.com/group/comp.lang.c/msg/7c7b39328fefab9c
 */
char* strtok_r(
    char *str, 
    const char *delim, 
    char **nextp)
{
    char *ret;

    if (str == NULL)
    {
        str = *nextp;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        return NULL;
    }

    ret = str;

    str += strcspn(str, delim);

    if (*str)
    {
        *str++ = '\0';
    }

    *nextp = str;

    return ret;
}
#endif

#define DEFINE_CMD(x) int cmd_##x (struct cli_state *, int, char **)
DEFINE_CMD(help);
DEFINE_CMD(load);
DEFINE_CMD(peek);
DEFINE_CMD(poke);
DEFINE_CMD(print);
DEFINE_CMD(rx);
DEFINE_CMD(set);
DEFINE_CMD(tx);
DEFINE_CMD(version);
DEFINE_CMD(open);
DEFINE_CMD(probe);

#define MAX_ARGS    10

struct cmd {
    const char  *name;      /* Name of command */
    int (*exec)(struct cli_state *s, int argc, char **argv);
    const char  *desc;
    const char  *help;
};

static const struct cmd cmd_table[] = {
    {
        .name = "help",
        .exec = cmd_help,
        .desc = "Provide information about specified command",
        .help =
            "help <command>\n"
            "\n"
            "Provides extended help, like this, on any command.\n"
    },
    {
        .name = "load",
        .exec = cmd_load,
        .desc = "Load FPGA or FX3",
        .help =
            "load <fpga|fx3> <filename>\n"
            "\n"
            "Load an FPGA bitstream or program the FX3's SPI flash.\n"
    },
    {
        .name = "peek",
        .exec = cmd_peek,
        .desc = "Peek a memory location",
        .help =
            "peek <lms|trimdac|si> <address>\n"
            "\n"
            "The peek command can read any of the devices hanging off\n"
            "the FPGA which includes the LMS6002D transceiver, VCTCXO\n"
            "trim DAC or the Si5338 clock generator chip.\n"
            "\n"
            "Examples\n"
            "--------\n"
            "  bladeRF> peek si ...\n"
    },
    {
        .name = "poke",
        .exec = cmd_poke,
        .desc = "Poke a memory location",
        .help =
            "poke <lms|trimdac|si> <address> <data>\n"
            "\n"
            "The poke command can write any of the devices hanging off\n"
            "the FPGA which includes the LMS6002D transceiver, VCTCXO\n"
            "trim DAC or the Si5338 clock generator chip.\n"
            "\n"
            "Examples\n"
            "-------\n"
            "  bladeRF> poke lms ...\n"
    },
    {
        .name = "print",
        .exec = cmd_print,
        .desc = "Print information about the device",
        .help =
            "print <param>\n"
            "\n"
            "The print command takes a parameter to print.  The parameter\n"
            "is one of:\n"
            "\n"
            "   bandwidth       Bandwidth settings\n"
            "   config          Overview of everything\n"
            "   frequency       Frequency settings\n"
            "   lmsregs         LMS6002D register dump\n"
            "   loopback        Loopback settings\n"
            "   mimo            MIMO settings\n"
            "   pa              PA settings\n"
            "   pps             PPS settings\n"
            "   refclk          Reference clock settings\n"
            "   rxvga1          Gain setting of RXVGA1 in dB (range: )\n"
            "   rxvga2          Gain setting of RXVGA2 in dB (range: )\n"
            "   samplerate      Samplerate settings\n"
            "   trimdac         VCTCXO Trim DAC settings\n"
            "   txvga1          Gain setting of TXVGA1 in dB (range: )\n"
            "   txvga2          Gain setting of TXVGA2 in dB (range: )\n"
    },
    {
        .name = "open",
        .exec = cmd_open,
        .desc = "Open a bladeRF device",
        .help = "open <device>\n"
                "\n"
                "Open the specified device for use with successive commands.\n"
                "Any previously opened device will be closed.\n",
    },
    {
        .name = "probe",
        .exec = cmd_probe,
        .desc = "List attached bladeRF devices",
        .help = "probe\n"
                "\n"
                "Search for attached bladeRF device and print a list\n"
                "of results.\n",
    },
    {
        .name = "quit",
        .exec = NULL,   /* Default action on NULL exec function is to quit */
        .desc = "Exit the CLI",
        .help = "Exit the CLI\n"
    },
    {
        .name = "rx",
        .exec = cmd_rx,
        .desc = "Receive IQ samples",
        .help =
            " rx <filename> [format] [# samples]\n"
            "\n"
            "Receive IQ samples and write them to the specified file.\n"
            "TODO\n"
    },
    {
        .name = "tx",
        .exec = cmd_tx,
        .desc = "Transmit IQ samples",
        .help =
            " tx <filename> TODO\n"
            "\n"
            "Receive IQ samples and write them to the specified file.\n"
            "TODO\n"
    },
    {
        .name = "set",
        .exec = cmd_set,
        .desc = "Set device settings",
        .help =
            "set <param> <arguments>\n"
            "\n"
            "The set command takes a parameter and an arbitrary number of\n"
            "arguments for that particular command.  The parameter is one\n"
            "of:\n"
            "\n"
            "   bandwidth       Bandwidth settings\n"
            "   config          Overview of everything\n"
            "   frequency       Frequency settings\n"
            "   lmsregs         LMS6002D register dump\n"
            "   loopback        Loopback settings\n"
            "   mimo            MIMO settings\n"
            "   pa              PA settings\n"
            "   pps             PPS settings\n"
            "   refclk          Reference clock settings\n"
            "   rxvga1          Gain setting of RXVGA1 in dB (range: )\n"
            "   rxvga2          Gain setting of RXVGA2 in dB (range: )\n"
            "   samplerate      Samplerate settings\n"
            "   trimdac         VCTCXO Trim DAC settings\n"
            "   txvga1          Gain setting of TXVGA1 in dB (range: )\n"
            "   txvga2          Gain setting of TXVGA2 in dB (range: )\n"
    },
    {
        .name = "version",
        .exec = cmd_version,
        .desc = "Device and firmware versions",
        .help =
            "version\n"
            "\n"
            "Prints version information for device and firmware.\n"
    },
    /* Always terminate the command entry with a completely NULL entry */
    {
        .name = NULL,
        .exec = NULL,
        .desc = "",
        .help = ""
    }
};

const struct cmd *get_cmd( char *name )
{
    const struct cmd *rv = NULL;
    int i = 0 ;
    for( i = 0 ; cmd_table[i].name != NULL && rv == 0 ; i++ ) {
        if( strcasecmp( name, cmd_table[i].name ) == 0 ) {
            rv = &(cmd_table[i]) ;
        }
    }

    return rv ;
}

int cmd_help(struct cli_state *s, int argc, char **argv)
{
    int i = 0 ;
    int ret = CMD_RET_OK ;
    const struct cmd *cmd ;

    /* Asking for general help */
    if( argc == 1 ) {
        printf( "\n" ) ;
        for( i = 0 ; cmd_table[i].name != NULL ; i++ ) {
            /* Hidden functions for fun and profit */
            if( cmd_table[i].name[0] == '_' ) continue ;
            printf( "  %-20s%s\n", cmd_table[i].name, cmd_table[i].desc ) ;
        }
        printf( "\n" );

    /* Asking for command specific help */
    } else if( argc == 2 ) {

        /* Iterate through the commands */
        cmd = get_cmd(argv[1]);

        /* If we found it, print the help */
        if( cmd ) {
            printf( "\n%s\n", cmd->help ) ;
        } else {
            /* Otherwise, print that we couldn't find it :( */
            printf( "%s: Could not find help on command %s\n", argv[0], argv[1] ) ;
            ret = CMD_RET_INVPARAM ;
        }
    } else {
        printf( "%s: Incorrect number of arguments (%d)\n", argv[0], argc ) ;
        ret = CMD_RET_INVPARAM ;
    }

    return ret ;
}

const char * cmd_strerror(int error, int lib_error)
{
    switch (error) {
        case CMD_RET_MEM:
            return "\nA fatal memory allocation error has occurred.\n";

        case CMD_RET_MAX_ARGC:
            return "\nNumber of arguments exceeds allowed maximum.\n";

        case CMD_RET_LIBBLADERF:
            return bladerf_strerror(lib_error);

        case CMD_RET_NODEV:
            return "\nError: No devices are currently opened.\n";

        /* Other commands shall print out helpful info from within their
         * implementation */
        default:
            return NULL;
    }
}

/* TODO this will need to take a cli_state * param */
int cmd_handle(struct cli_state *s, const char *line_)
{
    int argc, ret;
    char *saveptr;
    char *line;
    char *token;
    char *argv[MAX_ARGS]; /* TODO dynamically allocate? */

    const struct cmd *cmd;

    if (!(line = strdup(line_)))
        return CMD_RET_MEM;

    argc = 0;
    ret = 0;

    token = strtok_r(line, " \t\r\n", &saveptr);
    while (token) {
        argv[argc++] = token;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }

    if (argc > 0) {
        cmd = get_cmd(argv[0]);

        if (cmd) {
            if (cmd->exec) { /* ---v  will be cmd_handle's cli_state param */
                ret = cmd->exec(s, argc, argv);
            } else {
                ret = CMD_RET_QUIT;
            }
        } else {
            printf( "\nUnrecognized command: %s\n\n", argv[0] ) ;
            ret = CMD_RET_NOCMD;
        }

        if(ret != 0)
            s->last_error = ret;
    }

    free(line);
    return ret;
}
