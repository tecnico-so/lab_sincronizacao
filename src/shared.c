#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N (4)
#define ALICE_DEPOSIT_AMOUNT ((ssize_t)1)
#define BOB_WITHDRAW_AMOUNT ((ssize_t)1)

typedef struct {
    ssize_t balance;
    ssize_t n_operations;
} account_t;

typedef struct {
    account_t *account;
    ssize_t n_deposits;
} alice_args_t;

typedef struct {
    account_t *account;
    ssize_t n_withdrawals;
} bob_args_t;

void account_init(account_t *account) {
    account->balance = 0;
    account->n_operations = 0;
}

void account_deposit(account_t *account, size_t amount) {
    account->balance += amount;
    account->n_operations += 1;
}

ssize_t account_withdraw(account_t *account, size_t amount) {
    if (account->balance >= amount) {
        account->balance -= amount;
        account->n_operations += 1;
        return amount;
    }

    return -1;
}

void account_print_info(account_t *account) {
    printf("account info: balance= %zd$, #operations = %zd\n", account->balance,
           account->n_operations);
}

void *alice_thread_fn(void *arg) {
    alice_args_t const *args = (alice_args_t const *)arg;

    size_t total_deposited = 0;
    for (size_t i = 0; i < args->n_deposits; i++) {
        account_deposit(args->account, ALICE_DEPOSIT_AMOUNT);
        total_deposited += ALICE_DEPOSIT_AMOUNT;
    }

    printf("alice deposited a total of: %zd$\n", total_deposited);
    return NULL;
}

void *bob_thread_fn(void *arg) {
    bob_args_t const *args = (bob_args_t const *)arg;

    size_t total_withdrawn = 0;
    size_t failed_withdrawals = 0;
    for (size_t i = 0; i < args->n_withdrawals; i++) {
        ssize_t ret = account_withdraw(args->account, BOB_WITHDRAW_AMOUNT);
        if (ret != -1) {
            total_withdrawn += ret;
        } else {
            failed_withdrawals += 1;
        }
    }

    printf("bob withdrew a total of %zd$, failing to withdraw %zu times\n",
           total_withdrawn, failed_withdrawals);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t tid[2];

    ssize_t total_ops = 0;
    if (argc > 1) {
        total_ops = atoll(argv[1]);
    }

    account_t account;
    account_init(&account);

    alice_args_t alice_args = {.n_deposits = total_ops, .account = &account};

    bob_args_t bob_args = {.n_withdrawals = total_ops, .account = &account};

    if (pthread_create(&tid[0], NULL, alice_thread_fn, (void *)&alice_args) != 0) {
        fprintf(stderr, "failed to create alice thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&tid[1], NULL, bob_thread_fn, (void *)&bob_args) != 0) {
        fprintf(stderr, "failed to create bob thread: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    printf("end of financial history\n");
    account_print_info(&account);

    return 0;
}
