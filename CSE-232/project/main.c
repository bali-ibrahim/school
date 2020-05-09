#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHAR_PER_LINE 40
#define MAX_LINES 30
#define DEFAULT_NODE_INT -1
#define DEFAULT_NEW_LINE "\r\n"
struct node
{
    int statno;                        // statement number
    char statement[MAX_CHAR_PER_LINE]; // max. 40 characters
    // TODO: why next is an int as opposed to a node?
    int next; // points to the textbuffer[] index of the next statement
};

struct node textbuffer[MAX_LINES]; // max. 30 lines

const struct node DEFAULT_NODE = {
    .statno = DEFAULT_NODE_INT,
    .statement = 0,
    .next = DEFAULT_NODE_INT};

int head = DEFAULT_NODE_INT; // points to the first valid statement in textbuffer[]

void initialize_textbuffer(void)
{

    // initialize buffer
    for (int i = 0; i < MAX_LINES; ++i)
    {
        textbuffer[i] = DEFAULT_NODE;
    }
}

void edit(char *filename)
{
    char ch;
    FILE *fp;

    fp = fopen(filename, "r"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

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
        strncpy(line.statement, line_buffer, MAX_CHAR_PER_LINE);
        if (i > 0)
            textbuffer[i - 1].next = i;
        textbuffer[i] = line;
        ++i;
    }
    head = 0;
    fclose(fp);
}

void save(char *filename)
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
        if (textbuffer[i].statement == NULL)
            continue;
        fputs(textbuffer[i].statement, fp);

        // skip if last line
        if (textbuffer[i].next == DEFAULT_NODE_INT)
            continue;
        // add empty lines
        for (int i = 0; i < (textbuffer[textbuffer[i].next].statno - textbuffer[i].statno); ++i)
        {
            fputs(DEFAULT_NEW_LINE, fp);
        }
    }

    fclose(fp);
}

void delete (int statno)
{
    if (textbuffer[head].statno == statno)
    {
        int new_head = textbuffer[head].next;
        textbuffer[head] = DEFAULT_NODE;
        head = new_head;
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
        }
    }
}

void insert(int statno, char *stat)
{
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
        }
    }
}

int main(int argc, char const *argv[])
{
    for (int i = 0; i < 20; i++)
    {
        initialize_textbuffer();
        edit("new.txt");
        // TODO: doesn't work
        delete (1);
        save("saved.txt");
    }
    return 0;
}
