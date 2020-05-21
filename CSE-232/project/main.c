#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_CHAR_PER_LINE 40
#define MAX_LINES 30
#define MAX_CHANGES 20
#define DEFAULT_NODE_INT -1
#define DEFAULT_NEW_LINE "\n"
#define DFS_INSERT 1
#define DFS_DELETE 2
#define DFS_OTHERWISE 0
#define DEFAULT_DFS_SAVE_END -1
#define DEFAULT_REVISION 0
#define LATEST_REVISION -1
#define DIFF_LINE_MAX_LENGTH (3 * MAX_CHAR_PER_LINE)
#define MAX_INPUT_LENGTH 256
#define SUCCESSFUL 1
#define UNSUCCESSFUL 0
#define READ_TEXT "r"
#define WRITE_TEXT "w"
#define EMPTY_STR ""

void dfs_apply(void);
void _interpret(FILE *fp, const char *input);
void _read_arguments(FILE *fp, const int *statno, const char *statement);
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
int version = DEFAULT_REVISION;    // version number
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
    return textbuffer[idx].statno == DEFAULT_NODE.statno;
}

int _first_empty_index()
{
    for (int i = 0; i < MAX_LINES; ++i)
    {
        if (_is_empty(i))
            return i;
    }

    return DEFAULT_NODE.next;
}

int _dfs_is_empty(int idx)
{
    return diffs[idx].statno == DEFAULT_NODE.statno;
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

void dfs_insert(int statno, const char *stat)
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
    fp = fopen(dif_filename, WRITE_TEXT);

    if (fp == NULL)
    {
        printf("dfs_reset_version %s", dif_filename);
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
        fp = fopen(dif_filename, READ_TEXT);
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
        fp = fopen(dif_filename, WRITE_TEXT);
    }

    if (fp == NULL)
    {
        printf("dfs_save %s", filename);
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%d%s", ++version, DEFAULT_NEW_LINE);
    for (int i = 0;
         i < MAX_CHANGES && diffs[i].statno != DEFAULT_NODE.statno;
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

    t_fp = fopen(tmp_file, WRITE_TEXT);
    fp = fopen(dif_filename, READ_TEXT);

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
        puts("Rename Error");
    }
    free(tmp_file);
}

void dfs_read()
{

    _diffs_flush();
    FILE *fp;
    fp = fopen(dif_filename, READ_TEXT);

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

    fp = fopen(filename, WRITE_TEXT);

    if (fp == NULL)
    {
        printf("_save_file %s", filename);
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    // add empty lines
    for (int i = 1; i < textbuffer[head].statno; ++i)
    {
        fputs(DEFAULT_NEW_LINE, fp);
    }

    for (int i = head;
         i != DEFAULT_NODE.next;
         i = textbuffer[i].next)
    {
        if (_is_empty(i))
            continue;
        fputs(textbuffer[i].statement, fp);

        // skip if last line
        if (textbuffer[i].next == DEFAULT_NODE.next)
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
    //Opening File
    FILE *fileP;
    fileP = fopen(filename, READ_TEXT);
    //Local Variables
    int i = 0;
    int statno = 0;
    char temp[MAX_CHAR_PER_LINE]; //To skip empty lines.

    head = 0;
    if (fileP != NULL)
    {

        initialize_textbuffer();

        while (fgets(temp, MAX_CHAR_PER_LINE, fileP) != NULL)
        {
            statno++;
            if (!(strcmp(temp, "\n") && strcmp(temp, "\r") && strcmp(temp, "\n\r") && strcmp(temp, "\r\n")))
                continue;

            textbuffer[i].statno = statno;
            strcpy(textbuffer[i].statement, temp);
            if (i > 0)
                textbuffer[i - 1].next = i;
            i++;
        }
    }
    else
    {
        printf("%s is not found.\n", filename);
        exit(EXIT_FAILURE);
    }
    //Closing file
    fclose(fileP);
}

void edit()
{
    _read_file();
    dfs_read();
    dfs_apply();
}

void print()
{
    puts(filename);
    for (int i = head;
         i != DEFAULT_NODE.next;
         i = textbuffer[i].next)
    {
        if (_is_empty(i))
            continue;
        char *statement;
        printf("%d ", textbuffer[i].statno);
        printf(textbuffer[i].statement);
    }
    puts(EMPTY_STR);
}

// void save()
// {

// 	int i;
// 	int data;
// 	fp=fopen(file,"w");
// 	for(i=head;i!='\0';i=textbuffer[i].next)
// 	{
// 		data=textbuffer[i].statno;
// 		fprintf(fp,"%d\n",data);
// 	}
// 	fclose(fp);
// }

// void save2()
// {
// 	int i;
// 	int data;
// 	int neg=-1;
// 	if(version==0)
// 	{
// 		fp=fopen(file,"w");
// 		fprintf(fp,"%d\n",version);
// 		for(i=head;i!='\0';i=diffs[i].next)
// 		{
// 			data=diffs[i].statno;
// 			fprintf(fp,"%d\n",data);
// 		}
// 		fprintf(fp,"%d\n",neg);
// 		fclose(fp);
// 	}
// 	else
// 	{
// 		fp=fopen(file,"a");
// 		fprintf(fp,"%d\n",version);
// 		for(i=head;i!='\0';i=diffs[i].next)
// 		{
// 			data=diffs[i].statno;
// 			fprintf(fp,"%d\n",data);
// 		}
// 		fprintf(fp,"%d\n",neg);
// 		fclose(fp);
// 	}
// }

void save()
{
    dfs_save();
}

int delete (int statno)
{

    int iterHead = head;
    int tempHead;

    if (textbuffer[head].statno == statno)
    {
        iterHead = textbuffer[head].next;
        textbuffer[head] = DEFAULT_NODE;
        head = iterHead;
        return SUCCESSFUL;
    }

    while (textbuffer[iterHead].statno != textbuffer[MAX_LINES - 1].statno)
    {
        tempHead = iterHead;
        iterHead = textbuffer[iterHead].next;

        if (textbuffer[iterHead].statno == statno)
        {
            textbuffer[tempHead].next = textbuffer[iterHead].next;
            textbuffer[iterHead] = DEFAULT_NODE;
            return SUCCESSFUL;
        }
    }
    return UNSUCCESSFUL;
}

// int insert(int statno, char *stat)
// {

//     int i, size, count = 0;                            // Initialize size, count, i
//     size = sizeof(textbuffer) / sizeof(textbuffer[0]); //if we dont know size of the array

//     for (i = 0; i < size; i++)
//     { // Loop to check for statement number
//         if (textbuffer[i].statement != " ")
//             count++;
//     }

//     /*struct node *newNode = (struct node*) malloc(sizeof(struct node));*/ //first try of impl.
//     struct node newNode;

//     newNode.statno = statno;
//     strcpy(newNode.statement, stat); //newNode.statement = stat;

//     if (count == 0)
//     {                 // inserts a new node on the front of the list
//         head = count; // head = 0
//         textbuffer[head].statno = newNode.statno;
//         strcpy(textbuffer[head].statement, newNode.statement); //textbuffer[head].statement = newNode.statement;
//         textbuffer[head].next = 1;
//     }

//     else
//     { //Code of inserting a node at position(state number) in the array

//         for (i = 0; i < size; i++)
//         { // Loop to check for statement number
//             // textbuffer[i].next = i+1;  (Is it necessary?)

//             if (textbuffer[i].statno == newNode.statno)
//             {
//                 strcpy(textbuffer[i].statement, newNode.statement); //textbuffer[i].statement = newNode.statement;
//             }

//             if ((textbuffer[i].statno < newNode.statno) && (textbuffer[i + 1].statno > newNode.statno))
//             {
//                 i++; //because we are going to insert the new line at index i+1 since we check if it is bigger than index i equals true

//                 /*struct node *tmp = (struct node*)malloc(sizeof(struct node));*/ //-----> first try of impl.
//                 struct node tmp;

//                 /*tmp = textbuffer[i];		-----> first try of impl.
// 				textbuffer[i]= newNode;
// 				newNode = textbuffer[i+1];
// 				textbuffer[i+1] = tmp;*/

//                 tmp.statno = textbuffer[i].statno;              //second try of impl.
//                 strcpy(tmp.statement, textbuffer[i].statement); //tmp.statement = textbuffer[i].statement;

//                 textbuffer[i].statno = newNode.statno;
//                 strcpy(textbuffer[i].statement, newNode.statement); //textbuffer[i].statement = newNode.statement;

//                 newNode.statno = textbuffer[i + 1].statno;
//                 strcpy(newNode.statement, textbuffer[i + 1].statement); //newNode.statement = textbuffer[i+1].statement;

//                 textbuffer[i + 1].statno = tmp.statno;
//                 strcpy(textbuffer[i + 1].statement, tmp.statement); //textbuffer[i+1].statement = tmp.statement;

//                 int size_left;

//                 while (size_left >= 0)
//                 {
//                     i = i + 2;
//                     size_left = size - i;

//                     /*tmp = textbuffer[i];	-----> first try of implementation
// 					textbuffer[i]= newNode;
// 					newNode = textbuffer[i+1];
// 					textbuffer[i+1] = tmp;*/

//                     tmp.statno = textbuffer[i].statno;              //second try of implementation
//                     strcpy(tmp.statement, textbuffer[i].statement); //tmp.statement = textbuffer[i].statement;

//                     textbuffer[i].statno = newNode.statno;
//                     strcpy(textbuffer[i].statement, newNode.statement); //textbuffer[i].statement = newNode.statement;

//                     newNode.statno = textbuffer[i + 1].statno;
//                     strcpy(newNode.statement, textbuffer[i + 1].statement); //newNode.statement = textbuffer[i+1].statement;

//                     textbuffer[i + 1].statno = tmp.statno;
//                     strcpy(textbuffer[i + 1].statement, tmp.statement); //textbuffer[i+1].statement = tmp.statement;
//                 }
//             }
//         }
//     }
//     return SUCCESSFUL;
// }

int insert(int statno, char *stat)
{
    // TODO: verify that adding newline is required or not
    // ASSUMPTION: a line is defined as characters ending with DEFAULT_NEW_LINE
    char statement[MAX_CHAR_PER_LINE];
    strcpy(statement, stat);
    strcat(statement, DEFAULT_NEW_LINE);

    int new_index = _first_empty_index();
    // buffer is full
    if (new_index == DEFAULT_NODE.next)
        return UNSUCCESSFUL;

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
        return SUCCESSFUL;
    }

    for (int i = head;
         i != DEFAULT_NODE.next;
         i = textbuffer[i].next)
    {
        struct node *current_line = &textbuffer[i];
        struct node *next_line = &textbuffer[current_line->next];

        if (current_line->next == DEFAULT_NODE.next || statno < next_line->statno)
        {
            textbuffer[new_index].next = current_line->next;
            current_line->next = new_index;
            return SUCCESSFUL;
        }
    }
    return UNSUCCESSFUL;
}

void dfs_apply()
{
    for (int i = 0; i < MAX_CHANGES && diffs[i].code != DEFAULT_DFS.code; ++i)
    {
        int should_remove_diff = 0;
        switch (diffs[i].code)
        {
        case DFS_INSERT:
            should_remove_diff = insert(diffs[i].statno, diffs[i].statement);
            break;
        case DFS_DELETE:
            should_remove_diff = delete (diffs[i].statno);
            break;
        default:
            break;
        }
        if (should_remove_diff)
        {
            diffs[i] = DEFAULT_DFS;
        }
        else
        {
            printf("Diff no: %d could not be applied!", i);
            puts(EMPTY_STR);
        }
    }
}

void commit()
{
    _read_file();
    dfs_save();
    dfs_read();
    dfs_apply();
    _save_file();
    dfs_reset_version();
}

void _interpreter_loop()
{
    // TODO: untested
    char command;
    while (command != 'X')
    {
        puts("Please enter command and argument(s).");
        char input[MAX_INPUT_LENGTH];
        fgets(input, MAX_INPUT_LENGTH, stdin);
        sscanf(input, "%c", &command);
        // printf("Command in loop is %c%s", command, DEFAULT_NEW_LINE);
        _interpret(NULL, input);
    }
}

void _interpret(FILE *fp, const char *input)
{
    char command;
    char argument[MAX_INPUT_LENGTH];
    int argument2 = LATEST_REVISION;
    sscanf(input, "%c %s %d", &command, argument, &argument2);

    if (!(filename || command == 'E'))
    {
        printf("Please select a file to edit buy entering 'E {filename} {version_no}'.");
        return;
    }
    int statno;
    char statement[MAX_CHAR_PER_LINE];
    switch (command)
    {
    case 'E':
        free(filename);
        filename = strdup(argument);
        free(dif_filename);
        dif_filename = strdup(filename);
        strcat(dif_filename, ".dif");
        if (argument2 == LATEST_REVISION)
        {
            argument2 = dfs_file_max_version();
        }
        version = argument2;
        edit();
        printf("Started editing version %d of %s", version, filename);
        puts(EMPTY_STR);
        break;
    case 'I':
        puts("Insertion started.");
        puts("{line_no} {statement}");
        _read_arguments(fp, &statno, statement);
        printf("inside _interpret %d %s\n", statno, statement);
        if (insert(statno, statement))
        {
            dfs_insert(statno, statement);
        }
        puts("Insertion finished.");
        break;
    case 'D':
        puts("Deletion started.");
        puts("Please enter the {line_no}");
        _read_arguments(fp, &statno, statement);
        if (delete (statno))
        {
            dfs_delete(statno);
        }
        puts("Deletion finished.");
        break;
    case 'P':
        print();
        break;
    case 'S':
        puts("Save started.");
        save();
        puts("Save finished.");
        break;
    case 'C':
        puts("Commit started.");
        commit();
        puts("Commit finished.");
        break;
    default:
        printf("Command %c could not be intrepreted.", command);
        puts(EMPTY_STR);
        break;
    }
}

void _test(const char *test_filename)
{
    FILE *fp;
    fp = fopen(test_filename, READ_TEXT); // read mode

    if (fp == NULL)
    {
        printf("_test %s", test_filename);
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    char line_buffer[MAX_CHAR_PER_LINE];
    while (fgets(line_buffer, MAX_CHAR_PER_LINE, fp) != NULL)
    {
        _interpret(fp, line_buffer);
        memset(line_buffer, 0, MAX_CHAR_PER_LINE);
    }
    fclose(fp);
}

void _read_arguments(FILE *fp, const int *statno, const char *statement)
{
    char line_buffer[MAX_CHAR_PER_LINE];
    // from a file
    if (fp != NULL)
    {
        fgets(line_buffer, MAX_CHAR_PER_LINE, fp) != NULL;
    }
    // from stdin
    else
    {
        scanf("%[^\n\r]", line_buffer);
    }

    // printf(line_buffer);
    sscanf(line_buffer, "%d %[^\n\r]", statno, statement);
    // printf("inside _read %d %s\n", statno, statement);
}

int main(int argc, char const *argv[])
{
    // ./main.exe test1.txt
    if (argc == 2)
    {
        puts("Running file input mode.");
        _test(argv[1]);
    }
    else
    {
        puts("Running Interactive mode.");
        _interpreter_loop();
    }
    return 0;
}
