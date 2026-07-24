#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_FILE "employee_records.dat"

typedef struct
{
    int id;
    char name[32];
    double salary;
} Employee;

static int write_all(
    int fd,
    const void *buffer,
    size_t bytes)
{
    const char *position = (const char *)buffer;
    size_t total = 0;

    while (total < bytes)
    {
        ssize_t written = write(
            fd,
            position + total,
            bytes - total);

        if (written < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        total += (size_t)written;
    }

    return 0;
}

static int read_all(
    int fd,
    void *buffer,
    size_t bytes)
{
    char *position = (char *)buffer;
    size_t total = 0;

    while (total < bytes)
    {
        ssize_t received = read(
            fd,
            position + total,
            bytes - total);

        if (received == 0)
        {
            return 1;
        }

        if (received < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        total += (size_t)received;
    }

    return 0;
}

static off_t get_record_offset(size_t index)
{
    return (off_t)(index * sizeof(Employee));
}

static int move_to_record(int fd, size_t index)
{
    off_t result = lseek(
        fd,
        get_record_offset(index),
        SEEK_SET);

    return result == (off_t)-1 ? -1 : 0;
}

static void display_employee(
    const char *label,
    const Employee *employee)
{
    printf(
        "%s: id=%d, name=%s, salary=%.2f\n",
        label,
        employee->id,
        employee->name,
        employee->salary);
}

int main(void)
{
    Employee records[] = {
        {101, "Aarav Sharma", 55000.00},
        {102, "Meera Das", 62000.00},
        {103, "Rohan Singh", 58000.00}};

    const size_t record_count =
        sizeof(records) / sizeof(records[0]);

    int fd = open(
        DATA_FILE,
        O_CREAT | O_TRUNC | O_RDWR,
        S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < record_count; ++i)
    {
        if (
            write_all(
                fd,
                &records[i],
                sizeof(Employee)) == -1)
        {
            perror("write");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    printf(
        "Created %s and wrote %zu fixed-size "
        "employee records.\n",
        DATA_FILE,
        record_count);

    /*
     * Update record at index 1 directly without
     * rewriting the entire file.
     */
    const size_t update_index = 1;
    Employee selected;

    if (move_to_record(fd, update_index) == -1)
    {
        perror("lseek before read");
        close(fd);
        return EXIT_FAILURE;
    }

    int result = read_all(
        fd,
        &selected,
        sizeof(Employee));

    if (result != 0)
    {
        if (result < 0)
        {
            perror("read");
        }
        else
        {
            fprintf(
                stderr,
                "Unexpected end of file.\n");
        }

        close(fd);
        return EXIT_FAILURE;
    }

    display_employee("Before update", &selected);

    selected.salary = 67500.00;

    if (move_to_record(fd, update_index) == -1)
    {
        perror("lseek before update");
        close(fd);
        return EXIT_FAILURE;
    }

    if (
        write_all(
            fd,
            &selected,
            sizeof(Employee)) == -1)
    {
        perror("write update");
        close(fd);
        return EXIT_FAILURE;
    }

    display_employee("After update", &selected);

    /*
     * Retrieve the third record directly using its
     * calculated byte offset.
     */
    const size_t retrieve_index = 2;
    Employee retrieved;

    if (move_to_record(fd, retrieve_index) == -1)
    {
        perror("lseek before retrieval");
        close(fd);
        return EXIT_FAILURE;
    }

    result = read_all(
        fd,
        &retrieved,
        sizeof(Employee));

    if (result != 0)
    {
        if (result < 0)
        {
            perror("read retrieval");
        }
        else
        {
            fprintf(
                stderr,
                "Unexpected end of file.\n");
        }

        close(fd);
        return EXIT_FAILURE;
    }

    display_employee(
        "Retrieved record at index 2",
        &retrieved);

    if (close(fd) == -1)
    {
        perror("close");
        return EXIT_FAILURE;
    }

    printf("File descriptor closed successfully.\n");

    return EXIT_SUCCESS;
}