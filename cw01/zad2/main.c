#include <stdlib.h>
#include <stdio.h>

#include <sys/times.h>
#include <time.h>

#ifdef DYNAMIC_TEST

#include <dlfcn.h>

#endif

#include "../zad1/addrbook.h"
#include "data.h"

typedef struct Timing {
    struct timespec realtime;
    clock_t usertime;
    clock_t systime;
} Timing;

typedef struct AvgTiming {
    Timing sums;
    int count;
} AvgTiming;

Timing timing_start(void) {
    struct tms mtms;
    times(&mtms);

    struct timespec rtsp;
    clock_gettime(CLOCK_REALTIME, &rtsp);

    Timing t;
    t.realtime = rtsp;
    t.usertime = mtms.tms_utime;
    t.systime = mtms.tms_stime;
    return t;
}

void timing_end(Timing *timing) {
    Timing cur = timing_start();
    long nanodiff = cur.realtime.tv_nsec - timing->realtime.tv_nsec;
    timing->realtime.tv_sec = cur.realtime.tv_sec - timing->realtime.tv_sec + nanodiff / 1000000000L;
    timing->realtime.tv_nsec = nanodiff % 1000000000L;
    timing->usertime = cur.usertime - timing->usertime;
    timing->systime = cur.systime - timing->systime;
}

void timing_print(const Timing *timing) {
    printf("real: %.3lfms, user: %.3lfms, sys: %.3lfms",
           (timing->realtime.tv_sec * 1000.0 + timing->realtime.tv_nsec / 1000000.0),
           (timing->usertime * 1000.0) / CLOCKS_PER_SEC,
           (timing->systime * 1000.0) / CLOCKS_PER_SEC);
}

AvgTiming avgtiming_new(void) {
    AvgTiming t;
    t.sums.realtime.tv_sec = 0;
    t.sums.realtime.tv_nsec = 0;
    t.sums.usertime = 0;
    t.sums.systime = 0;
    t.count = 0;
    return t;
}

void avgtiming_report(AvgTiming *avg, const Timing *timing) {
    long nanosum = avg->sums.realtime.tv_nsec + timing->realtime.tv_nsec;
    avg->sums.realtime.tv_sec = avg->sums.realtime.tv_sec + timing->realtime.tv_sec + nanosum / 1000000000L;
    avg->sums.realtime.tv_nsec = nanosum % 1000000000L;
    avg->sums.usertime += timing->usertime;
    avg->sums.systime += timing->systime;
    avg->count += 1;
}

void avgtiming_end(AvgTiming *avg, Timing *timing) {
    timing_end(timing);
    avgtiming_report(avg, timing);
}

Timing avgtiming_summarize(const AvgTiming *avg) {
    if (avg->count == 0) {
        Timing t;
        t.realtime.tv_sec = 0;
        t.realtime.tv_nsec = 0;
        t.usertime = 0;
        t.systime = 0;
        return t;
    }

    Timing t;
    t.realtime.tv_sec = avg->sums.realtime.tv_sec / avg->count;
    t.realtime.tv_nsec =
            (avg->sums.realtime.tv_sec % avg->count) * 1000000000L + avg->sums.realtime.tv_nsec / avg->count;
    t.usertime = avg->sums.usertime / avg->count;
    t.systime = avg->sums.systime / avg->count;
    return t;
}

void avgtiming_print(AvgTiming *t) {
    Timing summary = avgtiming_summarize(t);
    printf("measurements: %d, averages [", t->count);
    timing_print(&summary);
    printf("]");
}

typedef void (*BenchAction)(void);

typedef void (*AvgBenchAction)(AvgTiming *t);

void bench(const char *name, BenchAction action) {
    printf("%s - ", name);
    Timing t = timing_start();
    action();
    timing_end(&t);
    timing_print(&t);
    printf("\n");
}

void avgbench(const char *name, AvgBenchAction action) {
    printf("%s - ", name);
    AvgTiming t = avgtiming_new();
    action(&t);
    avgtiming_print(&t);
    printf("\n");
}

AddrBook *llbook;
AddrBook *bstbook;

void create_ll() {
    llbook = addr_book_new(ABB_LinkedList);
}

void create_bst() {
    bstbook = addr_book_new(ABB_Bst);
    addr_book_rebuild(bstbook, ABB_Bst, ABS_ByName);
}

void fill_ll(AvgTiming *t) {
    for (int i = 0; i < 1000; i++) {
        Timing tt = timing_start();
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
        int id = (int) data[i][0];
#pragma GCC diagnostic pop
        const char *name = data[i][1];
        const char *phone = data[i][2];
        addr_book_add(llbook, addr_new(id, name, phone));
        avgtiming_end(t, &tt);
    }
}

void fill_bst(AvgTiming *t) {
    for (int i = 0; i < 1000; i++) {
        Timing tt = timing_start();
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
        int id = (int) data[i][0];
#pragma GCC diagnostic pop
        const char *name = data[i][1];
        const char *phone = data[i][2];
        addr_book_add(bstbook, addr_new(id, name, phone));
        avgtiming_end(t, &tt);
    }
}

void search_ll_opt(void) {
    addr_book_search_by_id(llbook, 1);
}

void search_ll_pes(void) {
    addr_book_search_by_id(llbook, 1000);
}

void search_bst_opt(void) {
    addr_book_search_by_name(bstbook, "Norma Williamson");
}

void search_bst_pes(void) {
    addr_book_search_by_name(bstbook, "Tina Gonzales");
}

void remove_ll_opt(void) {
    addr_book_remove(llbook, addr_book_search_by_id(llbook, 1));
}

void remove_ll_pes(void) {
    addr_book_remove(llbook, addr_book_search_by_id(llbook, 1000));
}

void remove_bst_opt(void) {
    addr_book_remove(bstbook, addr_book_search_by_name(bstbook, "Norma Williamson"));
}

void remove_bst_pes(void) {
    addr_book_remove(bstbook, addr_book_search_by_name(bstbook, "Tina Gonzales"));
}

void llid2llname(void) {
    addr_book_rebuild(llbook, ABB_LinkedList, ABS_ByName);
};

void llname2bstid(void) {
    addr_book_rebuild(llbook, ABB_Bst, ABS_ById);
};

void bstid2bstphone(void) {
    addr_book_rebuild(llbook, ABB_Bst, ABS_ByPhone);
};

void bstphone2llphone(void) {
    addr_book_rebuild(llbook, ABB_LinkedList, ABS_ByPhone);
};

void bstname2llphone(void) {
    addr_book_rebuild(bstbook, ABB_LinkedList, ABS_ByPhone);
};

void dynamic_test(void) {
#ifdef DYNAMIC_TEST
    void *dlhandle = dlopen("/lib64/libm.so.6", RTLD_LAZY);
    if (!dlhandle) {
        fprintf(stderr, "%s\n", dlerror());
    }

    double (*dynamic_cos)(double) = (double (*)(double)) dlsym(dlhandle, "cos");

    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
    }

    printf("\nFunny cosine: %f\n", (*dynamic_cos)(2.0));

    dlclose(dlhandle);
#endif
}

int main(int argc, char *argv[]) {
    bench("Creating new ll-ab", create_ll);
    bench("Creating new bst-ab (sorted by name)", create_bst);
    avgbench("Filling ll-ab", fill_ll);
    avgbench("Filling bst-ab (sorted by name)", fill_bst);
    bench("Searching in ll-ab (optimistic)", search_ll_opt);
    bench("Searching in ll-ab (pesimistic)", search_ll_pes);
    bench("Searching in bst-ab (optimistic)", search_bst_opt);
    bench("Searching in bst-ab (pesimistic)", search_bst_pes);
    bench("Removing from ll-ab (optimistic)", remove_ll_opt);
    bench("Removing from ll-ab (pesimistic)", remove_ll_pes);
    bench("Removing from bst-ab (optimistic)", remove_bst_opt);
    bench("Removing from bst-ab (pesimistic)", remove_bst_pes);
    bench("Rebuilding ll-ab id -> ll-ab name", llid2llname);
    bench("Rebuilding ll-ab name -> bst-ab id", llname2bstid);
    bench("Rebuilding bst-ab id -> bst-ab phone", bstid2bstphone);
    bench("Rebuilding bst-ab phone -> ll-ab phone", bstphone2llphone);
    bench("Rebuilding bst-ab name -> ll-ab phone", bstname2llphone);

    dynamic_test();

    addr_book_free(bstbook);
    addr_book_free(llbook);

    return 0;
}
