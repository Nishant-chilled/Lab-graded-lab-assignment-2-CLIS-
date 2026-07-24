#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define CHILD_COUNT 3
#define TIMEOUT_SECONDS 5
#define GRACE_SECONDS 2

typedef struct
{
    pid_t pid;
    time_t start_time;
    bool active;
    bool term_sent;
    bool kill_sent;
} ChildInfo;

static void run_child(
    int number,
    int work_seconds,
    bool ignore_sigterm)
{
    if (ignore_sigterm)
    {
        if (signal(SIGTERM, SIG_IGN) == SIG_ERR)
        {
            perror("signal");
            _exit(EXIT_FAILURE);
        }
    }

    printf(
        "Child %d started: PID=%ld, planned work=%d second(s)%s\n",
        number,
        (long)getpid(),
        work_seconds,
        ignore_sigterm
            ? ", intentionally ignoring SIGTERM"
            : "");

    for (int second = 1; second <= work_seconds; ++second)
    {
        sleep(1);

        printf(
            "Child %d (PID=%ld) progress: %d/%d\n",
            number,
            (long)getpid(),
            second,
            work_seconds);
    }

    printf(
        "Child %d (PID=%ld) completed normally.\n",
        number,
        (long)getpid());

    _exit(EXIT_SUCCESS);
}

static void print_status(pid_t pid, int status)
{
    if (WIFEXITED(status))
    {
        printf(
            "Parent reaped PID=%ld; exit status=%d.\n",
            (long)pid,
            WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf(
            "Parent reaped PID=%ld; terminated by signal %d.\n",
            (long)pid,
            WTERMSIG(status));
    }
}

int main(void)
{
    /*
     * Disable output buffering so messages do not become delayed
     * or duplicated around fork().
     */
    setvbuf(stdout, NULL, _IONBF, 0);

    const int work_time[CHILD_COUNT] = {1, 3, 20};

    ChildInfo children[CHILD_COUNT] = {0};

    int created = 0;

    printf(
        "Parent PID=%ld creating %d child processes.\n",
        (long)getpid(),
        CHILD_COUNT);

    for (int i = 0; i < CHILD_COUNT; ++i)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            break;
        }

        if (pid == 0)
        {
            /*
             * The last child deliberately ignores SIGTERM.
             * This demonstrates SIGKILL escalation.
             */
            run_child(
                i + 1,
                work_time[i],
                i == CHILD_COUNT - 1);
        }

        children[i].pid = pid;
        children[i].start_time = time(NULL);
        children[i].active = true;

        created++;

        printf(
            "Parent created child %d with PID=%ld.\n",
            i + 1,
            (long)pid);
    }

    int active_children = created;

    while (active_children > 0)
    {
        time_t now = time(NULL);

        for (int i = 0; i < created; ++i)
        {
            if (!children[i].active)
            {
                continue;
            }

            int status = 0;

            pid_t result = waitpid(
                children[i].pid,
                &status,
                WNOHANG);

            if (result == children[i].pid)
            {
                print_status(result, status);

                children[i].active = false;
                active_children--;

                continue;
            }

            if (result == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                perror("waitpid");

                children[i].active = false;
                active_children--;

                continue;
            }

            double elapsed = difftime(
                now,
                children[i].start_time);

            if (
                elapsed >= TIMEOUT_SECONDS &&
                !children[i].term_sent)
            {
                printf(
                    "PID=%ld exceeded %d seconds; "
                    "sending SIGTERM.\n",
                    (long)children[i].pid,
                    TIMEOUT_SECONDS);

                if (
                    kill(children[i].pid, SIGTERM) == -1 &&
                    errno != ESRCH)
                {
                    perror("kill(SIGTERM)");
                }

                children[i].term_sent = true;
            }
            else if (
                elapsed >= TIMEOUT_SECONDS + GRACE_SECONDS &&
                children[i].term_sent &&
                !children[i].kill_sent)
            {
                printf(
                    "PID=%ld ignored SIGTERM; "
                    "sending SIGKILL.\n",
                    (long)children[i].pid);

                if (
                    kill(children[i].pid, SIGKILL) == -1 &&
                    errno != ESRCH)
                {
                    perror("kill(SIGKILL)");
                }

                children[i].kill_sent = true;
            }
        }

        if (active_children > 0)
        {
            sleep(1);
        }
    }

    /*
     * Defensive final wait to make sure that no terminated
     * child remains as a zombie.
     */
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }

    printf(
        "All child processes have been monitored and reaped.\n");

    return EXIT_SUCCESS;
}