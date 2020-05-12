#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_CHAR_PER_LINE 40
#define MAX_LINES 30
#define MAX_CHANGES 20
#define DEFAULT_NODE_INT -1
#define DEFAULT_NEW_LINE "\r\n"
#define DFS_INSERT 1
#define DFS_DELETE 2
#define DFS_OTHERWISE 0
#define DEFAULT_DFS_SAVE_END -1
#define DEFAULT_REVISION 0
#define LATEST_REVISION MAX_CHANGES
#define DIFF_LINE_MAX_LENGTH (3 * MAX_CHAR_PER_LINE)
struct dfs_format
{
    char *scanf;
    char *printf;
};
const struct dfs_format DFS_FORMAT = {
    // https://stackoverflow.com/a/2854510/7032856
    // these should be identical(ish)
    .scanf = "%d %d %[^\n\r]",
    .printf = "%d %d %s",
};

struct node
{
    int statno;                        // statement number
    char statement[MAX_CHAR_PER_LINE]; // max. 40 characters
    // TODO: why next is an int as opposed to a node?
    int next; // points to the textbuffer[] index of the next statement
};
const struct node DEFAULT_NODE = {
    .statno = DEFAULT_NODE_INT,
    .statement = 0,
    .next = DEFAULT_NODE_INT};
struct dfs
{
    int code; // 1-insertion 2-deletion 0-otherwise
    int statno;
    char statement[MAX_CHAR_PER_LINE]; // only for insertion
};

const struct dfs DEFAULT_DFS = {
    .code = DEFAULT_NODE_INT,
    .statno = DEFAULT_NODE_INT,
    .statement = 0,
};

struct node textbuffer[MAX_LINES]; // max. 30 lines
int head = DEFAULT_NODE_INT;       // points to the first valid statement in textbuffer[]
struct dfs diffs[MAX_CHANGES];     // max. 20 changes
int version = 0;                   // version number
char *filename;
char *dif_filename;

void initialize_textbuffer(void)
{

    // initialize buffer
    for (int i = 0; i < MAX_LINES; ++i)
    {
        textbuffer[i] = DEFAULT_NODE;
    }
}

void _diffs_flush(void)
{

    // initialize buffer
    for (int i = 0; i < MAX_CHANGES; ++i)
    {
        diffs[i] = DEFAULT_DFS;
    }
}

int _is_empty(int idx)
{
    return textbuffer[idx].statno == DEFAULT_NODE_INT;
}
int _first_empty_index()
{
    for (int i = 0; i < MAX_LINES; ++i)
    {
        if (_is_empty(i))
            return i;
    }

    return MAX_LINES;
}
int _dfs_is_empty(int idx)
{
    return diffs[idx].statno == DEFAULT_NODE_INT;
}
int _dfs_first_empty_index()
{
    for (int i = 0; i < MAX_CHANGES; ++i)
    {
        if (_dfs_is_empty(i))
            return i;
    }

    return MAX_CHANGES;
}

int _file_exists(char *fname)
{
    return (access(fname, F_OK) != -1);
}

void dfs_insert(int statno, char *stat)
{
    struct dfs df = {
        .code = DFS_INSERT,
        .statno = statno,
    };
    strcpy(df.statement, stat);
    diffs[_dfs_first_empty_index()] = df;
}

void dfs_delete(int statno)
{
    struct dfs df = {
        .code = DFS_DELETE,
        .statno = statno,
    };
    diffs[_dfs_first_empty_index()] = df;
}

void dfs_reset_version()
{
    version = 0;
    FILE *fp;
    fp = fopen(dif_filename, "w");

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%d%s", version, DEFAULT_NEW_LINE);
    fclose(fp);
}

int dfs_file_max_version()
{
    int _version;
    FILE *fp;
    if (_file_exists(dif_filename))
    {
        fp = fopen(dif_filename, "r");
        char line_buffer[DIFF_LINE_MAX_LENGTH];
        while (fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL)
        {
            sscanf(line_buffer, "%d", &_version);
            while (fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL)
            {
                // revision end
                int code;
                sscanf(line_buffer, "%d", &code);
                if (code == DEFAULT_DFS_SAVE_END)
                    break;
            }
        }
        fclose(fp);
    }
    else
    {
        dfs_reset_version();
        _version = version;
    }
    return _version;
}

void dfs_save()
{

    FILE *fp;
    unsigned int line_no = 0;
    if (_file_exists(dif_filename) && dfs_file_max_version() != DEFAULT_REVISION)
    {
        fp = fopen(dif_filename, "r+");

        // find where the current revision ends
        char line_buffer[DIFF_LINE_MAX_LENGTH];
        int _version;
        while (fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL)
        {
            ++line_no;
            sscanf(line_buffer, "%d", &_version);
            while (fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL)
            {
                ++line_no;
                // revision end
                int code;
                sscanf(line_buffer, "%d", &code);
                if (code == DEFAULT_DFS_SAVE_END)
                    break;
            }
            if (_version == version)
                break;
        }
    }
    else
    {
        fp = fopen(dif_filename, "w");
    }

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%d%s", ++version, DEFAULT_NEW_LINE);
    for (int i = 0;
         i < MAX_CHANGES && diffs[i].statno != DEFAULT_NODE_INT;
         ++i)
    {
        fprintf(fp, DFS_FORMAT.printf, diffs[i].code, diffs[i].statno, diffs[i].statement);
        fprintf(fp, DEFAULT_NEW_LINE);
        ++line_no;
    }

    fprintf(fp, "%d%s", DEFAULT_DFS_SAVE_END, DEFAULT_NEW_LINE);
    ++line_no;
    _diffs_flush();
    fclose(fp);

    // remove rest of the file

    char *tmp_file = strdup(dif_filename);
    tmp_file = strcat(tmp_file, "~");
    FILE *t_fp;

    t_fp = fopen(tmp_file, "w");
    fp = fopen(dif_filename, "r");

    char line_buffer[DIFF_LINE_MAX_LENGTH];
    for (unsigned int i = 0; i <= line_no && fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL; ++i)
    {
        fprintf(t_fp, line_buffer);
        // fprintf(t_fp, DEFAULT_NEW_LINE);
    }
    fclose(fp);
    fclose(t_fp);

    // possible you have to remove old file here before
    if (!rename(tmp_file, dif_filename))
    {
        printf("Rename Error");
    }
    free(tmp_file);
}

void dfs_read()
{

    _diffs_flush();
    FILE *fp;
    fp = fopen(dif_filename, "r");

    if (fp == NULL)
    {
        dfs_reset_version();
        return;
    }

    // TODO: calculate better line length or make it dynamic
    char line_buffer[DIFF_LINE_MAX_LENGTH];
    while (fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL)
    {

        int _version;
        sscanf(line_buffer, "%d", &_version);
        // read just until the desired version
        if (_version > version || _version == DEFAULT_REVISION)
            return;

        while (fgets(line_buffer, DIFF_LINE_MAX_LENGTH, fp) != NULL)
        {
            // revision end
            int code;
            sscanf(line_buffer, "%d", &code);
            if (code == DEFAULT_DFS_SAVE_END)
                break;
            struct dfs df = DEFAULT_DFS;
            sscanf(line_buffer, DFS_FORMAT.scanf, &df.code, &df.statno, df.statement);
            diffs[_dfs_first_empty_index()] = df;
        }
    }

    fclose(fp);
}

void _save_file()
{
    FILE *fp;

    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // add empty lines
    for (int i = 1; i < textbuffer[head].statno; ++i)
    {
        fputs(DEFAULT_NEW_LINE, fp);
    }

    for (int i = head;
         i != DEFAULT_NODE_INT;
         i = textbuffer[i].next)
    {
        if (_is_empty(i))
            continue;
        fputs(textbuffer[i].statement, fp);

        // skip if last line
        if (textbuffer[i].next == DEFAULT_NODE_INT)
            continue;
        // add empty lines
        for (int j = 0; j < (textbuffer[textbuffer[i].next].statno - textbuffer[i].statno - 1); ++j)
        {
            fputs(DEFAULT_NEW_LINE, fp);
        }
    }

    fclose(fp);
}

void _read_file()
{
    FILE *fp;

    fp = fopen(filename, "r"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    initialize_textbuffer();
    int line_no = 0;
    int i = 0;
    char line_buffer[MAX_CHAR_PER_LINE];
    while (fgets(line_buffer, MAX_CHAR_PER_LINE, fp) != NULL)
    {
        ++line_no;
        // skip empty line
        if (!(strcmp(line_buffer, "\n") && strcmp(line_buffer, "\r") && strcmp(line_buffer, "\n\r") && strcmp(line_buffer, "\r\n")))
            continue;
        struct node line = DEFAULT_NODE;
        line.statno = line_no;
        strcpy(line.statement, line_buffer);
        if (i > 0)
            textbuffer[i - 1].next = i;
        textbuffer[i] = line;
        ++i;
    }
    head = 0;
    fclose(fp);
}

void edit()
{
    _read_file();
    dfs_read();
}

void print()
{
    puts(filename);
    for (int i = head;
         i != DEFAULT_NODE_INT;
         i = textbuffer[i].next)
    {
        if (_is_empty(i))
            continue;
        char *statement;
        printf("%d ", textbuffer[i].statno);
        printf(textbuffer[i].statement);
    }
    puts("");
}

void save()
{
    dfs_save();
}

void delete (int statno)
{
    if (textbuffer[head].statno == statno)
    {
        int new_head = textbuffer[head].next;
        textbuffer[head] = DEFAULT_NODE;
        head = new_head;
        dfs_delete(statno);
        return;
    }

    for (int i = head;
         i != DEFAULT_NODE_INT;
         i = textbuffer[i].next)
    {
        struct node *current_line = &textbuffer[i];
        struct node *next_line = &textbuffer[current_line->next];

        if (next_line->statno == statno)
        {
            current_line->next = next_line->next;
            *next_line = DEFAULT_NODE;
            dfs_delete(statno);
            return;
        }
    }
}

void insert(int statno, char *stat)
{
    // TODO: verify that adding newline is required or not
    // ASSUMPTION: a line is defined as characters ending with DEFAULT_NEW_LINE
    char statement[MAX_CHAR_PER_LINE];
    strcpy(statement, stat);
    strcat(statement, DEFAULT_NEW_LINE);

    int new_index = _first_empty_index();
    // buffer is full
    if (new_index == DEFAULT_NODE_INT)
        return;

    // TODO: refuse statno <1 ?

    // TODO: centralize node creation
    struct node line = DEFAULT_NODE;
    strcpy(line.statement, statement);
    line.statno = statno;
    textbuffer[new_index] = line;

    if (statno < textbuffer[head].statno)
    {

        textbuffer[new_index].next = head;
        head = new_index;
        dfs_insert(statno, stat);
        return;
    }

    for (int i = head;
         i != DEFAULT_NODE_INT;
         i = textbuffer[i].next)
    {
        struct node *current_line = &textbuffer[i];
        struct node *next_line = &textbuffer[current_line->next];

        if (current_line->next == DEFAULT_NODE_INT || statno < next_line->statno)
        {
            textbuffer[new_index].next = current_line->next;
            current_line->next = new_index;
            dfs_insert(statno, stat);
            return;
        }
    }
}

void dfs_apply()
{
    for (int i = 0; i < MAX_CHANGES; ++i)
    {
        switch (diffs[i].code)
        {
        case DFS_INSERT:
            insert(diffs[i].statno, diffs[i].statement);
            break;
        case DFS_DELETE:
            delete (diffs[i].statno);
            break;
        default:
            break;
        }
        diffs[i] = DEFAULT_DFS;
    }
}

void commit()
{
    _read_file();
    dfs_read();
    dfs_apply();
    _save_file();
    dfs_reset_version();
}

void interpreter_loop()
{
    // TODO: untested
    char command;
    char *argument;
    int argument2 = LATEST_REVISION;

    while (command != 'X')
    {
        scanf("%c %s %d", &command, argument, &argument2);
        if (!(filename || command == 'E'))
        {
            printf("Please select a file to edit buy entering 'E filename'.");
            continue;
        }
        //
        int statno;
        char *statement;
        switch (command)
        {
        case 'E':
            free(filename);
            filename = strdup(argument);
            free(dif_filename);
            dif_filename = strdup(filename);
            strcat(dif_filename, ".dif");
            version = argument2;
            edit();
            break;
        case 'I':
            // TODO: centralize 'except for line end' strings?
            scanf("%d %[^\n\r]", &statno, statement);
            insert(statno, statement);
            break;
        case 'D':
            scanf("%d", &statno);
            break;
        case 'P':
            print();
            break;
        case 'S':
            save();
            break;
        case 'C':
            commit();
            break;
        default:
            break;
        }
    }
}

int main(int argc, char const *argv[])
{
    // filename = "new.txt";
    // dif_filename = strdup(filename);
    // // strcpy(dif_filename, filename);
    // strcat(dif_filename, ".dif");

    // edit();
    // version = 2;
    // // commit();
    // // edit();
    // // delete (1);
    // // save();
    // // insert(2, "This is the new 2nd line.");
    // // save();
    // // // insert(1, "This is the new 1st line.");
    // insert(4, "This is the new 4th line.");
    // insert(5, "This is the new 5th line.");
    // save();
    // // print();

    // free(dif_filename);

    interpreter_loop();
    return 0;
}
